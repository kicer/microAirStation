#ifndef _BOARD_H_
#define _BOARD_H_


#define UART_COMM_BAUD     9600

#define TUIGAN_RUN_TIME    5    /* sec */
#define ACTION_LOCK_TIME   60   /* sec */
#define BMQ_RUN_CIRCLE     5    /* nx100 */
#define BMQ_RUN_ANGLE      0    /* 0~100 */

#define EVENT_RECV_PKG     1
#define EVENT_SEND_PKG     2
#define EVENT_BMQ_STOP     3

typedef struct {
    uint8_t head;
    uint8_t cmd;
    uint8_t len;
    uint8_t powerCnt;  /* power on */
    uint8_t actionCnt; /* action */
    uint8_t clearCnt;  /* manual init filter */
    /* === config === */
    uint8_t tuiganRunTime;
    uint8_t actionLockTime;
    uint8_t bmqRunCircle;
    uint8_t bmqRunAngle;
    /* === config === */
    uint8_t chksum;
    uint8_t tail;
} DevState;

typedef struct {
    uint8_t head;
    uint8_t cmd;
    uint8_t len;
    uint8_t step;
    uint8_t chksum;
    uint8_t tail;
} ActionState;

extern int board_init(void);


#endif /* _BOARD_H_ */
