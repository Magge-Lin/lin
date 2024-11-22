# 死锁：原因 && 检测

> 死锁检测工具：
>
> 动态库编译： gcc -fPIC -shared -g -o libdeadlock.so deadlock.c -lpthread -ldl
>
> 编译example：g++ -o example example.cpp -L. -ldeadlock



## 1、死锁的现象以及原理

![deadlock_01](.\deadlock_01.jpg)

```shell
# 原理：
	如图所示，线程所占有的锁资源，申请其他线程的锁。
```

## 2、死锁检测实现的原理

```
资源获取环可以采用图来存储，使用有向图存储；
线程A获取线程B已占用的锁，则为线程A指向线程B。如何为线程B已占用的锁，运行过程线程B获取成功的锁。
检测原理采用另一个线程定时对图检测是否有环的存在。

首先有三个节点：
	1、lock_after\lock_before\ulock_after   分别记录锁资源  竞争关系、占用、所属线程
```



## 3、pthread_mutex_lock、pthread_mutex_unlock  dlsym的实现方式

```c++
typedef int (*pthread_mutex_lock_t)(pthread_mutex_t *mutex);
pthread_mutex_lock_t pthread_mutex_lock_f = NULL;

typedef int (*pthread_mutex_unlock_t)(pthread_mutex_t *mutex);
pthread_mutex_unlock_t pthread_mutex_unlock_f = NULL;

int pthread_mutex_lock(pthread_mutex_t *mutex) {	
	pthread_t selfid = pthread_self();
	lock_before((uint64_t)selfid, (uint64_t)mutex);
	pthread_mutex_lock_f(mutex);
	lock_after((uint64_t)selfid, (uint64_t)mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	pthread_mutex_unlock_f(mutex);
	pthread_t selfid = pthread_self();
	unlock_after((uint64_t)selfid, (uint64_t)mutex);
}
void init_hook(void) {
	if (!pthread_mutex_lock_f)
		pthread_mutex_lock_f = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if (!pthread_mutex_unlock_f)
		pthread_mutex_unlock_f = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
}
```

## 4、有向图的构建

```c++
typedef unsigned long int uint64;
#define MAX		100
enum Type {PROCESS, RESOURCE};
struct source_type {
	uint64 id;
	enum Type type;
	uint64 lock_id;
	int degress;
};
struct vertex {
	struct source_type s;
	struct vertex *next;
};
struct task_graph {
	struct vertex list[MAX];
	int num;
	struct source_type locklist[MAX];
	int lockidx;
	pthread_mutex_t mutex;
};
struct task_graph *tg = NULL;
int path[MAX+1];
int visited[MAX];
int k = 0;
int deadlock = 0;
struct vertex *create_vertex(struct source_type type) {
	struct vertex *tex = (struct vertex *)malloc(sizeof(struct vertex ));
	tex->s = type;
	tex->next = NULL;
	return tex;
}
int search_vertex(struct source_type type) {
	int i = 0;
	for (i = 0;i < tg->num;i ++) {
		if (tg->list[i].s.type == type.type && tg->list[i].s.id == type.id) {
			return i;
		}
	}
	return -1;
}
void add_vertex(struct source_type type) {
	if (search_vertex(type) == -1) {
		tg->list[tg->num].s = type;
		tg->list[tg->num].next = NULL;
		tg->num ++;
	}
}
int add_edge(struct source_type from, struct source_type to) {
	add_vertex(from);
	add_vertex(to);
	struct vertex *v = &(tg->list[search_vertex(from)]);
	while (v->next != NULL) {
		v = v->next;
	}
	v->next = create_vertex(to);
}
int verify_edge(struct source_type i, struct source_type j) {
	if (tg->num == 0) return 0;
	int idx = search_vertex(i);
	if (idx == -1) {
		return 0;
	}
	struct vertex *v = &(tg->list[idx]);
	while (v != NULL) {
		if (v->s.id == j.id) return 1;
		v = v->next;
	}
	return 0;
}
int remove_edge(struct source_type from, struct source_type to) {
	int idxi = search_vertex(from);
	int idxj = search_vertex(to);
	if (idxi != -1 && idxj != -1) {
		struct vertex *v = &tg->list[idxi];
		struct vertex *remove;
		while (v->next != NULL) {
			if (v->next->s.id == to.id) {
				remove = v->next;
				v->next = v->next->next;
				free(remove);
				break;
			}
			v = v->next;
		}
	}
}

```

## 5、有向图 dfs 判断环的存在

```C
void print_deadlock(void) {
	int i = 0;
	printf("cycle : ");
	for (i = 0;i < k-1;i ++) {
		printf("%ld --> ", tg->list[path[i]].s.id);
	}
	printf("%ld\n", tg->list[path[i]].s.id);
}
int DFS(int idx) {
	struct vertex *ver = &tg->list[idx];
	if (visited[idx] == 1) {
		path[k++] = idx;
		print_deadlock();
		deadlock = 1;
		return 0;
	}
	visited[idx] = 1;
	path[k++] = idx;
	while (ver->next != NULL) {
		DFS(search_vertex(ver->next->s));
		k --;
		ver = ver->next;
	}
	return 1;
}
int search_for_cycle(int idx) {
	struct vertex *ver = &tg->list[idx];
	visited[idx] = 1;
	k = 0;
	path[k++] = idx;
	while (ver->next != NULL) {
		int i = 0;
		for (i = 0;i < tg->num;i ++) {
			if (i == idx) continue;
			visited[i] = 0;
		}
		for (i = 1;i <= MAX;i ++) {
			path[i] = -1;
		}
		k = 1;
		DFS(search_vertex(ver->next->s));
		ver = ver->next;
	}
}
```

## 6、三个源于操作 lock_before、lock_after、unlock_after

```
lock_before
lock_after
unlock_after

分别记录锁资源  竞争关系、占用、所属线程
```

## 7、死锁检测线程的实现

```C
void check_dead_lock(void) {
	int i = 0;
	deadlock = 0;
	for (i = 0;i < tg->num;i ++) {
		if (deadlock == 1) break;
		search_for_cycle(i);
	}
	if (deadlock == 0) {
		printf("no deadlock\n");
	}
}

static void *thread_routine(void *args) {
	while (1) {
		sleep(5);
		check_dead_lock();
	}
}

void start_check(void) {
	tg = (struct task_graph*)malloc(sizeof(struct task_graph));
	tg->num = 0;
	tg->lockidx = 0;
	pthread_t tid;
	pthread_create(&tid, NULL, thread_routine, NULL);
}

```

## 8、工作中遇到的死锁问题

```shell
# 怎么发现的
TLS定位系统软件，主备机频繁切换
# 怎么查的
看门狗打印的日志
检查对应时间的网络数据包有啥异常
# 原因是啥
使用lock()、unlock()  加锁解锁中间有抛出异常.
boost库的大数据类型转换，异常抛出.
```

