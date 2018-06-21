#define QQ_APP_TYPE_ANDROID       1
#define QQ_APP_TYPE_IOS           2

#define QQ_DEVICE_TYPE_WIFI       1

#define QQ_MAIN_APP_FLAGS         0x80000000



#define QQ_PKG_KEY_CMD                         "cmd"
#define QQ_PKG_KEY_TIMESTAMP                   "timestamp"
#define QQ_PKG_KEY_DEV_ID                      "dev_id"
#define QQ_PKG_KEY_APP_ID                      "app_id"
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
#define QQ_JSON_PKG_NO_APP                        "{\"err\":4,\"errstr\":\"no app\"}"
#define QQ_JSON_PKG_NO_APP_SIZE                   sizeof(QQ_JSON_PKG_NO_APP)
#define QQ_JSON_PKG_APP_OVERRANGING               "{\"err\":5,\"errstr\":\"app overranging\"}"
#define QQ_JSON_PKG_APP_OVERRANGING_SIZE          sizeof(QQ_JSON_PKG_APP_OVERRANGING)
#define QQ_JSON_PKG_MAIN_APP_ERROR                "{\"err\":6,\"errstr\":\"main app error\"}"
#define QQ_JSON_PKG_MAIN_APP_ERROR_SIZE           sizeof(QQ_JSON_PKG_MAIN_APP_ERROR)
#define QQ_JSON_PKG_DEVICE_CONFLICT_STR           "{\"err\":7,\"errstr\":\"device conflict\"}"
#define QQ_JSON_PKG_DEVICE_CONFLICT_STR_SIZE      sizeof(QQ_JSON_PKG_DEVICE_CONFLICT_STR)
#define QQ_JSON_PKG_INSUFFICIENT_MEMORY           "{\"err\":8,\"errstr\":\"insufficient memory\"}"
#define QQ_JSON_PKG_INSUFFICIENT_MEMORY_SIZE      sizeof(QQ_JSON_PKG_INSUFFICIENT_MEMORY)
