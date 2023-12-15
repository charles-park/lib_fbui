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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <getopt.h>
#include <pthread.h>
#include <linux/input.h>

//-----------------------------------------------------------------------------
#include "lib_ts.h"

pthread_mutex_t mutex_event = PTHREAD_MUTEX_INITIALIZER;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function prototype define.
//-----------------------------------------------------------------------------
static  int queue_put (event_queue_t *q, ts_event_t *d);
static  int queue_get (event_queue_t *q, ts_event_t *d);
        void *ts_thread_func(void *data);

        int ts_get_event    (ts_t *p_ts, ts_event_t *get_ts_event);
        ts_t *ts_init       (const char *DEVICE_NAME);;
        void ts_deinit      (ts_t *p_ts);

//-----------------------------------------------------------------------------
static int queue_put (event_queue_t *q, ts_event_t *d)
{
    pthread_mutex_lock  (&mutex_event);

    memcpy (&q->event[q->ep], d, sizeof(ts_event_t));

    pthread_mutex_unlock(&mutex_event);

    if (++q->ep >= EVENT_COUNT_MAX)   q->ep = 0;

    // queue overflow
    if (q->ep == q->sp) {
        q->sp++;
        return 0;
    }
    return  1;
}

//------------------------------------------------------------------------------
static int queue_get (event_queue_t *q, ts_event_t *d)
{
    if (q->ep != q->sp) {
        pthread_mutex_lock  (&mutex_event);

        memcpy (d, &q->event[q->sp], sizeof(ts_event_t));

        pthread_mutex_unlock(&mutex_event);

        if (++q->sp >= EVENT_COUNT_MAX)   q->sp = 0;

        return  1;
    }
    // queue empty
    return 0;
}

//------------------------------------------------------------------------------
void *ts_thread_func (void *data)
{
    ts_t *p_ts = (ts_t *)data;
    ts_event_t ts_event;
    int ts_status = eTS_STATUS_UNKNOWN, x, y;

    struct input_event event;

    printf("The touchscreen thread install successfully.\n");

    while(1){
        usleep(1000);
        if(read(p_ts->fd, &event, sizeof(struct input_event))) {
            switch (event.type) {
                case	EV_SYN:
                    switch (ts_status) {
                        case eTS_STATUS_PRESS:
                            ts_status       = eTS_STATUS_MOVE;
                            ts_event.status = eTS_STATUS_PRESS;
                            ts_event.x      = x;
                            ts_event.y      = y;
                            queue_put (&p_ts->event_q, &ts_event);
                            break;
                        case eTS_STATUS_RELEASE:
                            ts_status       = eTS_STATUS_UNKNOWN;
                            ts_event.status = eTS_STATUS_RELEASE;
                            ts_event.x      = x;
                            ts_event.y      = y;
                            queue_put (&p_ts->event_q, &ts_event);
                            break;
                    }
                    break;
                case	EV_KEY:
                    if (event.code == BTN_TOUCH)
                        ts_status = event.value ? eTS_STATUS_PRESS : eTS_STATUS_RELEASE;
                    break;
                case	EV_ABS:
                    if (ts_status == eTS_STATUS_PRESS) {
                        if (event.code == ABS_X)    x = event.value;
                        if (event.code == ABS_Y)    y = event.value;
                    }
                    break;
                default	:
                    printf("event.type = %d, event.code = %d, event.value = %d\n",
                        event.type, event.code, event.value);
                    break;
            }
        }
    }
}

//-----------------------------------------------------------------------------
int ts_get_event (ts_t *p_ts, ts_event_t *get_ts_event)
{
    return queue_get (&p_ts->event_q, get_ts_event);
}

//-----------------------------------------------------------------------------
ts_t *ts_init (const char *DEVICE_NAME)
{
    ts_t    *p_ts;

    if ((p_ts = (ts_t *)malloc (sizeof(ts_t))) == NULL) {
        printf ("%s : malloc error!\n", __func__);
        return NULL;
    }
    memset (p_ts, 0, sizeof(ts_t));

    if (pthread_create (&p_ts->ts_thread, NULL, ts_thread_func, (void *)p_ts)) {
        printf ("%s : cannot create ts_thread!\n", __func__);
        goto exit;
    }

    // touchscreen open
    if ((p_ts->fd = open (DEVICE_NAME, O_RDONLY)) == -1) {
        printf ("%s : cannot open touchscreen device (%s)\n", __func__, DEVICE_NAME);
        goto exit;
    }

    printf ("The touchscreen device was opened successfully.\n");
    return p_ts;

exit:
    ts_deinit (p_ts);
    return NULL;
}

//-----------------------------------------------------------------------------
void ts_deinit (ts_t *p_ts)
{
    if (p_ts->fd)   close (p_ts->fd);
    if (p_ts)       free (p_ts);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
