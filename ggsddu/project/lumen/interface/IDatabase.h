#ifndef __IDATABASE_H__
#define __IDATABASE_H__

#include "interface/Common.h"

#include <iostream>
#include <memory>
#include <cstring>
#include <any>

// 报警信息
typedef struct _ALARM_RECORD
{
	int32_t createTime;
	int32_t seqNO;
    std::vector<std::any> data;
} ALARM_RECORD;

bool operator >>= (const ALARM_RECORD& v, std::any& value);
bool operator <<= (std::any& value, const ALARM_RECORD& v);
bool operator <<= (ALARM_RECORD& v, const std::any& value);
bool operator >>= (const std::any& value, ALARM_RECORD& v);

// 报警查找信息
typedef struct _ALARM_RECORD_CONDITION
{
	int32_t createTime;
	int32_t seqNO;
} ALARM_RECORD_CONDITION;


/*
    数据库接口类
*/

class IDatabase;
DECLARE_PTR_TYPE(IDatabase)

class IDatabase
{
public:

    // 增
    virtual uint8_t saveAlarmTable(const ALARM_RECORD& rec) = 0;

    // 删
    virtual uint8_t deleteAlarmTable(const ALARM_RECORD& rec) = 0;

    // 改
    virtual uint8_t updateAlarmTable(const ALARM_RECORD& rec) = 0;

    // 查
    virtual std::vector<ALARM_RECORD>& getAlarmTable(const ALARM_RECORD_CONDITION& condition,  std::vector<ALARM_RECORD>& recs) = 0;

public:
    // IDatabase 的单例实现
	static IDatabasePtr instance(void);
};


#endif  //__IDATABASE_H__