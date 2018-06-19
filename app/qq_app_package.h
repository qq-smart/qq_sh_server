#define QQ_PKG_KEY_CMD                         "cmd"
#define QQ_PKG_KEY_TIMESTAMP                   "timestamp"
#define QQ_PKG_KEY_ID                          "id"
#define QQ_PKG_KEY_MAC                         "mac"
#define QQ_PKG_KEY_PWD                         "pwd"

#define QQ_PKG_VALUE_CMD_CONN                  "connection"
#define QQ_PKG_VALUE_CMD_DISCONN               "disconnection"
#define QQ_PKG_VALUE_CMD_PING                  "ping"
#define QQ_PKG_VALUE_CMD_ADD_DEV               "add_device"
#define QQ_PKG_VALUE_CMD_DEL_DEV               "delete_device"
#define QQ_PKG_VALUE_CMD_GET_DEV_LIST          "get_device_list"
#define QQ_PKG_VALUE_CMD_SET_DEV_STA           "set_device_status"
#define QQ_PKG_VALUE_CMD_DEV_REPORT_STA        "device_report_status"


#define QQ_JSON_PKG_OK_STR                        "{\"err\":0,\"errstr\":\"ok\"}"
#define QQ_JSON_PKG_OK_STR_SIZE                   sizeof(QQ_JSON_PKG_OK_STR)
#define QQ_JSON_PKG_PKG_FAILED_STR                "{\"err\":1,\"errstr\":\"package failed\"}"
#define QQ_JSON_PKG_PKG_FAILED_STR_SIZE           sizeof(QQ_JSON_PKG_PKG_FAILED_STR)
#define QQ_JSON_PKG_MD5_FAILED_STR                "{\"err\":2,\"errstr\":\"md5 failed\"}"
#define QQ_JSON_PKG_MD5_FAILED_STR_SIZE           sizeof(QQ_JSON_PKG_MD5_FAILED_STR)
#define QQ_JSON_PKG_DEVICE_NOTHINGNESS_STR        "{\"err\":3,\"errstr\":\"device nothingness\"}"
#define QQ_JSON_PKG_DEVICE_NOTHINGNESS_STR_SIZE   sizeof(QQ_JSON_PKG_DEVICE_NOTHINGNESS_STR)
#define QQ_JSON_PKG_DEVICE_CONFLICT_STR           "{\"err\":4,\"errstr\":\"device conflict\"}"
#define QQ_JSON_PKG_DEVICE_CONFLICT_STR_SIZE      sizeof(QQ_JSON_PKG_DEVICE_CONFLICT_STR)