//-----------------------------------------------------------------------------
/**
 * @file lib_ts.c
 * @author charles-park (charles.park@hardkernel.com)
 * @brief Touchscreen event control library
 * @version 0.1
 * @date 2023-12-01
 *
 * @copyright Copyright (c) 2022
 *
 */
//-----------------------------------------------------------------------------
#ifndef __LIB_TS_H__
#define __LIB_TS_H__

//------------------------------------------------------------------------------
enum {
    eTS_STATUS_UNKNOWN = 0,
    eTS_STATUS_PRESS,
    eTS_STATUS_MOVE,
    eTS_STATUS_RELEASE,
    eTS_STATUS_END
};

typedef struct ts_event__t {
    int     x, y;
    // press, release, move, unknown
    int     status;
}   ts_event_t;

#define	EVENT_COUNT_MAX  256

typedef struct event_queue__t {
    unsigned int    sp;
    unsigned int    ep;
    ts_event_t      event[EVENT_COUNT_MAX];
}   event_queue_t;

typedef struct ts__t {
    int             fd;
    event_queue_t   event_q;
    pthread_t       ts_thread;
}   ts_t;

//-----------------------------------------------------------------------------
// Function prototype define.
//-----------------------------------------------------------------------------
extern int  ts_get_event    (ts_t *p_ts, ts_event_t *get_ts_event);
extern void ts_deinit       (ts_t *p_ts);
extern ts_t *ts_init        (const char *DEVICE_NAME);;

//------------------------------------------------------------------------------

#endif  // #define __LIB_TS_H__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
