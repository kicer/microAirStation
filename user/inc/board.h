#ifndef _BOARD_H_
#define _BOARD_H_


#define EVENT_UART1_PKGS   1

typedef struct {
    uint8_t head;
    uint8_t cmd;
    uint8_t len;
    uint8_t powerCnt;  /* power on */
    uint8_t actionCnt; /* action */
    uint8_t clearCnt;  /* manual init filter */
    uint8_t chksum;
    uint8_t tail;
} DevState;

extern int board_init(void);


#endif /* _BOARD_H_ */
