#ifndef COMMON_H
#define COMMON_H


#include <vector>
#include <map>
#include <string>
#include <memory>
#include <any>
#include <cstdint>

/*
    用于包含项目中的公共定义、宏、函数声明、类声明或其他共享的类型信息。
*/


//////////////////////////////////////////////////////////////////////////////////////////////////
//          typedef 
//////////////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_PTR_TYPE(cls) \
    typedef std::shared_ptr< cls > cls##Ptr;

#define DECLARE_WEAKPTR_TYPE(cls) \
    typedef std::weak_ptr< cls > cls##WeakPtr;

typedef std::vector<std::string> CStringVector;
typedef std::vector<std::any> CAnyVector;


//////////////////////////////////////////////////////////////////////////////////////////////////
//          类声明
//////////////////////////////////////////////////////////////////////////////////////////////////
class IObject;
DECLARE_PTR_TYPE(IObject)


//////////////////////////////////////////////////////////////////////////////////////////////////
//          操作符重载
//////////////////////////////////////////////////////////////////////////////////////////////////

// 重载操作符>>=，用于把指定类型的数据转换为std::any数据
template <typename T>
bool operator >>= (const T v, std::any& value)
{
    value = v;
    return (true);
};

// 重载操作符<<=，用于把指定类型的数据转换为 std::any 数据
template <typename T>
bool operator <<= (std::any& value, const T v)
{
    return (v >>= value);
};

// 重载操作符<<=，用于把 std::any 数据转换为指定类型的数据
template <typename T>
bool operator <<= (T& v, const std::any& value)
{
    bool bRst = true;

    try
    {
        v = std::any_cast< T >(value);
    }
    catch (std::bad_any_cast&)
    {
        bRst = false;
    }

    return (bRst);
};

// 重载操作符>>=，用于把 std::any 数据转换为指定类型的数据
template <typename T>
bool operator >>= (const std::any& value, T& v)
{
    return (v <<= value);
};

// 重载操作符>>=，用于把指定类型的数组数据转换为std::any数据
template <typename T>
bool operator >>= (const std::vector<T>& vt, std::any& value)
{
	std::vector<std::any> anyVt;
	for(uint32 i = 0; i < vt.size(); i++)
	{
		std::any  anyV;
		anyV <<= vt[i];
		anyVt.push_back(anyV);
	}
    value = anyVt;
    return (true);
};

// 重载操作符<<=，用于把指定数组数据转换为 std::any 数据
template <typename T>
bool  operator <<= (std::any& value, const std::vector<T>& vt)
{
    return (vt >>= value);
};

// 重载操作符<<=，用于把 std::any 数据转换为指定数组数据
template <typename T>
bool operator <<= (std::vector<T>& vt, const std::any& value)
{
    bool bRst = true;

    try
    {
		std::vector<std::any> anyVt = std::any_cast< std::vector<std::any> >(value);
		for(uint32 i = 0; i < anyVt.size(); i++)
		{
			T v;
			if(v <<= anyVt[i])
			{
				vt.push_back(v);
			}
			else
			{
				bRst = false;
				break;
			}
		}
    }
    catch (std::bad_any_cast&)
    {
        bRst = false;
    }

    return (bRst);
};

// 重载操作符>>=，用于把 std::any 数据转换为指定数组数据
template <typename T>
bool operator >>= (const std::any& value, std::vector<T>& vt)
{
    return (vt <<= value);
};

// 重载操作符>>=，用于把指智能指针转换为 std::any 数据
template <typename T>
bool operator >>= (const std::shared_ptr< T >& obj, std::any& value)
{
    bool bRst = true;
    IObjectPtr tempObj = std::dynamic_pointer_cast<IObject>(obj);
    value = tempObj;
    return (bRst);
};

// 重载操作符<<=，用于把智能指针转换为 std::any 数据
template <typename T>
bool operator <<= (std::any& value, const std::shared_ptr< T >& obj)
{
    return (obj >>= value);
};

// 重载操作符<<=，用于把智能指针转换为 std::any 数据
template <typename T>
bool operator <<= (std::shared_ptr< T >& obj, const std::any& value)
{
    bool bRst = true;

    try
    {
        IObjectPtr tempObj = std::any_cast< IObjectPtr >(value);
        obj = std::dynamic_pointer_cast< T >(tempObj);
    }
    catch (std::bad_any_cast&)
    {
        bRst = false;
    }

    return (bRst);
};

// 重载操作符>>=，用于把指定类型的数据转换为 std::any 数据
template <typename T>
bool operator >>= (const std::any& value, std::shared_ptr< T >& obj)
{
    return (obj <<= value);
};

// 重载操作符>>=，用于把指定智能指针数组转换为 std::any 数据
template <typename T>
bool operator >>= (const std::vector< std::shared_ptr< T > >& objs, std::any& value)
{
	std::vector<std::any> anyVector;
    for (uint32 i = 0; i < objs.size(); i++)
    {
		std::any anyV;
		anyV <<= objs[i];
		anyVector.push_back(anyV);
    }
    value = anyVector;
    return (true);
};

// 重载操作符<<=，用于把智能指针数组转换为 std::any 数据
template <typename T>
bool operator <<= (std::any& value, const std::vector< std::shared_ptr< T > >& objs)
{
    return (objs >>= value);
}

// 重载操作符<<=，用于把 std::any 数据转换为智能指针数组
template <typename T>
bool operator <<= (std::vector< std::shared_ptr< T > >& objs, const std::any& value)
{
    bool bRst = true;
    try
    {
		std::vector<std::any> anyVector = std::any_cast< std::vector<std::any> >(value);
		for (uint32 i = 0; i < anyVector.size(); i++)
        {
			 std::shared_ptr< T > object;
			object <<= anyVector[i];
            objs.push_back(object);
        }
    }
    catch (std::bad_any_cast&)
    {
        bRst = false;
    }

    return (bRst);
};

// 重载操作符>>=，用于把 std::any 数据转换为智能指针数组
template <typename T>
bool operator >>= (const std::any& value, std::vector< std::shared_ptr< T > >& objs)
{
    return (objs <<= value);
};

// 重载操作符>>=，用于把智能指针map转换为 std::any 数据
template <typename T1, typename T2>
bool operator >>= (const std::map< T1, std::shared_ptr< T2 > >& objs, std::any& value)
{
    std::any anyValue;
    std::map< T1, std::any> objMap;

    for (typename std::map< T1, std::shared_ptr< T2 > >::const_iterator pIter = objs.begin(); pIter != objs.end(); pIter++)
    {
        anyValue = std::dynamic_pointer_cast<IObject>(pIter->second);
        objMap.insert(std::make_pair(pIter->first, anyValue));
    }

    value = objMap;
    return (true);
};

// 重载操作符<<=，用于把智能指针map转换为 std::any 数据
template <typename T1, typename T2>
bool operator <<= (std::any& value, const std::map< T1, std::shared_ptr< T2 > >& objs)
{
    return (objs >>= value);
};

// 重载操作符<<=，用于把 std::any 转换为数据智能指针map
template <typename T1, typename T2>
bool operator <<= (std::map< T1, std::shared_ptr< T2 > >& objs, const std::any& value)
{
    bool bRst = true;

    try
    {
        IObjectPtr objValue;
        std::map<T1, std::any> objMap = std::any_cast< std::map<T1, std::any> >(value);

        for (typename std::map< T1, std::any >::const_iterator pIter = objMap.begin(); pIter != objMap.end(); pIter++)
        {
            objValue = std::any_cast<IObjectPtr>(pIter->second);
            objs.insert(std::make_pair(pIter->first, std::dynamic_pointer_cast<T2>(objValue)));
        }
    }
    catch (std::bad_any_cast&)
    {
        bRst = false;
    }

    return (bRst);
};

// 重载操作符<<=，用于把 std::any 转换为数据智能指针map
template <typename T1, typename T2>
bool operator >>= (const std::any& value, std::map< T1, std::shared_ptr< T2 > >& objs)
{
    return (objs <<= value);
};

// 重载操作符>>=，用于把智能指针map转换为 std::any 数据
template <typename T1, typename T2>
bool operator >>= (const std::map< std::shared_ptr< T1 >,  T2 >& objs, std::any& value)
{
    std::any anyValue;
	
    std::map< IObjectPtr, std::any> objMap;

    for (typename std::map< std::shared_ptr< T1 >,  T2 >::const_iterator pIter = objs.begin(); pIter != objs.end(); ++pIter)
    {
		anyValue <<= pIter->second;
        objMap.insert(std::make_pair(std::dynamic_pointer_cast<IObject>(pIter->first), anyValue));
    }

    value = objMap;
    return (true);
};

// 重载操作符<<=，用于把智能指针map转换为 std::any 数据
template <typename T1, typename T2>
bool operator <<= (std::any& value, const std::map< std::shared_ptr< T1 >,  T2 >& objs)
{
    return (objs >>= value);
};

// 重载操作符<<=，用于把 std::any 转换为数据智能指针map
template <typename T1, typename T2>
bool operator <<= (std::map< std::shared_ptr< T1 >,  T2 >& objs, const std::any& value)
{
    bool bRst = true;

	T2 t2Value;

    try
    {
        std::map< IObjectPtr,   std::any > objMap = std::any_cast< std::map<IObjectPtr,  std::any> >(value);

        for (typename std::map< IObjectPtr,   std::any >::const_iterator pIter = objMap.begin(); pIter != objMap.end(); ++pIter)
        {
			t2Value <<= pIter->second;
            objs.insert(std::make_pair(std::dynamic_pointer_cast<T1>(pIter->first),  t2Value));
        }
    }
    catch (std::bad_any_cast&)
    {
        bRst = false;
    }

    return (bRst);
};

// 重载操作符>>=，用于把 std::any 转换为数据智能指针map
template <typename T1, typename T2>
bool operator >>= (const std::any& value, std::map< std::shared_ptr< T1 >,  T2 >& objs)
{
    return (objs <<= value);
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//          用于架构 基础接口类 判断
//////////////////////////////////////////////////////////////////////////////////////////////////

// 用于判断该智能指针是否包含IObject接口
template <typename T>
bool isIObject(const std::shared_ptr<T>& pointer)
{
    bool bRst = true;

    if (std::dynamic_pointer_cast<IObject>(pointer) != NULL)
    {
        bRst = true;
    }
    else
    {
        bRst = false;
    }

    return (bRst);
};








#endif // #ifndef COMMON_H