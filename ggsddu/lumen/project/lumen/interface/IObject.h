#ifndef IOBJECT_H
#define IOBJECT_H

#include "interface/Common.h"

#include <iostream>
#include <memory>


/*
    用于架构的基础类之一
*/


class IObject :
    public std::enable_shared_from_this<IObject>        // 其派生类能够获取到有效的共享指针
{
public:

    // 提供虚构造函数,这样所有实现IObject接口的对象能正确的删除
	IObject(void);

    // 空的析构函数
	virtual ~IObject(void);

    // 通过对象指针来获得boost智能指针
	template<typename T>
	std::shared_ptr<T> getSharedPtr(T* p) const
	{
		return (std::dynamic_pointer_cast<T>(p->shared_from_this()));
	}

    // 重载操作符: new
	void* operator new(size_t size);

	// 重载操作符: delete
	void operator delete(void* p,  size_t size);
};

DECLARE_PTR_TYPE(IObject)

#endif // #ifndef IOBJECT_H
