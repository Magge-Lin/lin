#ifndef MESSAGEID_H
#define MESSAGEID_H

// 用于初始化
const uint16_t MSGID_UNKNOWN                        = 0;
const uint16_t MSGID_INVALID                        = 1;
const uint16_t MSGID_NOTIF                          = 2;


// 用于数据库
const uint16_t MSGID_SAVE_XXX                       = 2001;
const uint16_t MSGID_DELETE_XXX                     = 2007;
const uint16_t MSGID_UPDATE_XXX                     = 2002;
const uint16_t MSGID_GET_XXX                        = 2002;

// RPC
const uint16_t MSGID_ORBTASK_CLIENTSTATUS          = 3001;

#endif // #ifndef MESSAGEID_H