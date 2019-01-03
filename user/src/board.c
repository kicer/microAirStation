#include "stm8s.h"
#include "sys.h"
#include "uart.h"
#include "eeprom.h"
#include "board.h"

uint8_t Rx1Buffer[UART_RX_MAXSIZE];

static void config_make_checksum(DevState *pst) {
    pst->chksum = 0;
    uint8_t chksum = 0;
    for(int i=0; i<sizeof(DevState); i++) {
        chksum += *((uint8_t*)pst+i);
    }
    pst->chksum = 0xFF-chksum;
}

static void config_read_state(DevState *pst) {
    if(eeprom_read_config(pst) != sizeof(DevState)) {
        pst->head = 0x55;
        pst->cmd = 0x01;
        pst->len = 4;
        pst->powerCnt = 0;
        pst->actionCnt = 0;
        pst->clearCnt = 0;
        pst->tail = 0xAA;
        config_make_checksum(pst);
    }
}

static void config_update_powerCnt(void *p) {
    DevState st;
    config_read_state(&st);
    st.powerCnt += 1;
    config_make_checksum(&st);
    eeprom_write_config(&st, sizeof(st));
}

static void config_update_actionCnt(void *p) {
    DevState st;
    config_read_state(&st);
    st.actionCnt += 1;
    config_make_checksum(&st);
    eeprom_write_config(&st, sizeof(st));
}

static void config_update_clearCnt(void *p) {
    DevState st;
    config_read_state(&st);
    st.clearCnt += 1;
    config_make_checksum(&st);
    eeprom_write_config(&st, sizeof(st));
}

static void uart_pkgs_cb(void *p) {
    int cmd = *((uint8_t *)p+1);
    if(cmd == 0x01) { /* readState */
        DevState st;
        config_read_state(&st);
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
    sys_task_reg_alarm(60000, config_update_powerCnt, 0);
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