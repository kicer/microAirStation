#include "stm8s.h"
#include "sys.h"
#include "uart.h"
#include "board.h"

uint8_t Rx1Buffer[UART_RX_MAXSIZE];

static void uart_pkgs_cb(void *p) {
    int cmd = *((uint8_t *)p+1);
    if(cmd == 0x01) { /* readState */
        DevState st;
        /* todo: build st */
        uart1_flush_output(); /* force output */
        uart1_send((uint8_t *)&st, sizeof(st));
    } else if(cmd == 0x02) { /* filterUpdate */
        /* 1. up */
        /* 2. move */
        /* 3. check */
        /* 4. stop */
        /* 5. down */
    }
}

int board_init(void) {
    sys_task_reg_event(EVENT_UART1_PKGS, uart_pkgs_cb, Rx1Buffer);
    return 0;
}

void uart1_rx_cb(uint8_t ch) {
    /* package: 55 [cmd] [len] <data> [chksum] AA */
    static int idx = 0;
    static int size = 0;
    static uint8_t chksum = 0;
    Rx1Buffer[idx] = ch;
    switch(idx) {
        case 0:
            if(ch == 0x55) idx+=1;
            break;
        case 1:
            if(ch < 0x03) { /* cmd=1,2 */
                idx += 1;
                chksum = ch;
            } else {
                idx = 0;
            }
            break;
        case 2:
            if(ch <= UART_RX_MAXSIZE) {
                idx += 1;
                size = ch;
                chksum += ch;
                if(ch == 0) idx = 0;
            } else {
                idx = 0;
            }
            break;
        default:
            idx += 1;
            chksum += ch;
            break;
    }
    if(idx >= size+4) {
        if(ch == 0xAA) { /* receive a valid package? */
            sys_event_trigger(EVENT_UART1_PKGS);
        }
        idx = 0;
    } else if(idx == size+3) {
        if(chksum != 0) idx = 0;
    }
}