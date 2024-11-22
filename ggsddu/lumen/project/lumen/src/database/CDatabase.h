#ifndef __CDATABASE_H__
#define __CDATABASE_H__


#include "interface/IDatabase.h"
#include "interface/CTaskBase.h"

#include <iostream>
#include <mutex>


/*
    用来管理数据库
*/

class CDatabase;
DECLARE_PTR_TYPE(CDatabase)

class CDatabase
    : virtual public IDatabase
    , virtual public CTaskBase
{

    // 数据库类型
    enum EType
    {
        ORACLE,
        SQLSERVER,
        MYSQL,
        REDIS
    };

public:
    // 析构函数
    virtual ~CDatabase(void);
    // CDatabase 的单例对象
    static const CDatabasePtr& instance(void);

    // 初始化数据库
	int8_t initDB();

    // 连接数据库
	int8_t connectDB();

	// 断开与数据库连接
	void disconnectDB();

protected:
    // 运行状态
	virtual int  run();

public:
    // 增
    virtual uint8_t saveAlarmTable(const ALARM_RECORD& rec);

    // 删
    virtual uint8_t deleteAlarmTable(const ALARM_RECORD& rec);

    // 改
    virtual uint8_t updateAlarmTable(const ALARM_RECORD& rec);

    // 查
    virtual std::vector<ALARM_RECORD>& getAlarmTable(const ALARM_RECORD_CONDITION& condition,  std::vector<ALARM_RECORD>& recs);


private:
    // 构造函数
    CDatabase(void);
};


#endif  //__CDATABASE_H__
