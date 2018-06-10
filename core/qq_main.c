/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"
#include "qq_main.h"


static qq_uint_t   qq_show_help = 0;
static qq_uint_t   qq_show_version = 0;
static qq_uint_t   qq_debug_mode = 0;


static qq_int_t qq_get_options(int argc, char *const *argv);
static void qq_show_help_version_info(void);


int
main(int argc, char *const *argv)
{
    if (qq_get_options(argc, argv) != QQ_OK) {
        return 1;
    }

    if (qq_show_help || qq_show_version) {
        qq_show_help_version_info();
        return 0;
    }

    if (!qq_debug_mode) {
        if (daemon(1, 0) != QQ_OK) {
            qq_log_stderr("daemon(0, 0) failed"); 
            return 1;
        }
    }

    qq_time_init();

    qq_log_init(getpid());

#if (QQ_HAVE_JSON)
    if (qq_cjson_init(QQ_JSON_POOL_SIZE) != 0) {
        qq_log_error(0, "qq_cjson_init() failed");
        return 1;
    }
#endif

    if (qq_cycle_init() == QQ_ERROR) {
        qq_log_error(0, "qq_cycle_init() failed");
        return 1;
    }

    if (qq_signals_init() != QQ_OK) {
        return 1;
    }

    qq_process_cycle(qq_cycle);

    qq_cjson_done();
    qq_cycle_done();
 
    return 0;
}



static qq_int_t
qq_get_options(int argc, char *const *argv)
{
    u_char     *p;
    qq_int_t    i;

    for (i = 1; i < argc; i++) {
        p = (u_char *) argv[i];

        if (*p++ != '-') {
            qq_log_stderr("invalid option: %s", argv[i]);
            return QQ_ERROR;
        }

        while (*p) {
            switch (*p++) {
            case '?':
            case 'h':
                qq_show_help = 1;
                break;
            case 'v':
                qq_show_version = 1;
                break;
            case 'd':
                qq_debug_mode = 1;
                break;
            default:
                qq_log_stderr("invalid option: %c", *(p - 1));
                return QQ_ERROR;
            }
        }
    }

    return QQ_OK;
}

static void
qq_show_help_version_info(void)
{
    if (qq_show_help) {
        qq_write_stderr(
            "Usage: "QQ_SH_SERVER_NAME "[-?hvd]" QQ_LINEFEED
                QQ_LINEFEED
            "Options:" QQ_LINEFEED
            "  -?,-h         : this help" QQ_LINEFEED
            "  -v            : show version and exit" QQ_LINEFEED
            "  -d            : debug mode run" QQ_LINEFEED
        );
    }

    if (qq_show_version) {
        qq_write_stderr("qq_sh_server version: " QQ_SH_SERVER_VERSION QQ_LINEFEED);
    }
}
