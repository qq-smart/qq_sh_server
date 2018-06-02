/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


static void qq_signal_handler(int signo);


static int qq_quit = 0;
static int qq_terminate = 0;


qq_signal_t  signals[] = {
    { qq_signal_value(QQ_TERMINATE_SIGNAL),
      "SIG" qq_value(QQ_TERMINATE_SIGNAL),
      "stop",
      qq_signal_handler },

    { qq_signal_value(QQ_SHUTDOWN_SIGNAL),
      "SIG" qq_value(QQ_SHUTDOWN_SIGNAL),
      "quit",
      qq_signal_handler },

    { SIGINT, "SIGINT", "", qq_signal_handler },

    { 0, NULL, "", NULL }
};


qq_int_t
qq_signals_init(void)
{
    qq_signal_t       *sig;
    struct sigaction   sa;

    for (sig = signals; sig->signo != 0; sig++) {
        memset(&sa, 0, sizeof(struct sigaction));
        sa.sa_handler = sig->handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(sig->signo, &sa, NULL) == -1) {
            qq_log_error(errno, "sigaction(%s) failed", sig->signame);
            return QQ_ERROR;
        }
    }

    return QQ_OK;
}


void
qq_process_cycle(qq_cycle_t *cycle)
{
    for (;;) {
        qq_events_process();

        if (qq_terminate || qq_quit) {
            break;
        }
    }
}


static void
qq_signal_handler(int signo)
{
    qq_log_debug("qq_signal_handler(%d)", signo);

    switch (signo) {
    case qq_signal_value(QQ_SHUTDOWN_SIGNAL):
        qq_quit = 1;
        qq_log_debug("shutting down");
        break;
    case qq_signal_value(QQ_TERMINATE_SIGNAL):
    case SIGINT:
        qq_terminate = 1;
        qq_log_debug("exiting");
        break;
    }
}
