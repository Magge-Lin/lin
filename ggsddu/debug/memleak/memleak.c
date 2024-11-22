

#include "memleak.h"


#define _GNU_SOURCE
#include <dlfcn.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <link.h>


// addr2line -f -e ./memleak -a 0x400b38
void* ConvertToELF(void* addr){
	Dl_info info;
	struct link_map *link;

	dladdr1(addr, &info, (void**)&link, RTLD_DL_LINKMAP);

	return (void*)((size_t)addr - link->l_addr);
}

typedef void *(*malloc_t)(size_t size);
malloc_t malloc_f = NULL;

typedef void (*free_t)(void *ptr);
free_t free_f = NULL;


int enable_malloc_hook = 1;
int enable_free_hook = 1;


void *malloc(size_t size) {

	void *ptr = NULL;
	if (enable_malloc_hook) {
		enable_malloc_hook = 0;
	
		ptr = malloc_f(size);

	// main --> f1() --> f2() --> f3() { __builtin_return_address(0)  }

		void *caller = __builtin_return_address(0);

		char filename[128] = {0};
		sprintf(filename, "./mem/%p.mem", ptr);

		FILE *fp = fopen(filename, "w");
		fprintf(fp, "[+] caller: %p, addr: %p, size: %ld\n", ConvertToELF(caller), ptr, size);

		fflush(fp);
		
		enable_malloc_hook = 1;
	} else {
		ptr = malloc_f(size);
	}
	
	return ptr;
}

void free(void *ptr) {

	if (enable_free_hook) {
		enable_free_hook = 0;
        
        char buff[128] = {0};
		sprintf(buff, "./mem/%p.mem", ptr);

		if (unlink(buff) < 0) { // filename no exist;
			printf("double free: %p\n", ptr);
			return ;
		}

		free_f(ptr);

		enable_free_hook = 1;
	} else {

		free_f(ptr);
		
	}

}

void init_hook(void) {

	if (!malloc_f) {
		malloc_f = dlsym(RTLD_NEXT, "malloc");
	}
	if (!free_f) {
		free_f = dlsym(RTLD_NEXT, "free");
	}
}


void init(void){
	init_hook();
}