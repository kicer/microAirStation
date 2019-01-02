#include "stm8s.h"
#include "sys.h"

typedef enum {
    TASK_NONE=0,
    TASK_ALARM,
    TASK_TIMER,
    TASK_SIGNAL,
} _TaskType;

typedef struct {
    Task foo;
    void *params;
    int type;
    int argv;
    int ticks;
} TaskStack;

TaskStack _sys_task[TASK_STACK_SIZE];
__IO uint32_t _sys_ticks = 0;
__IO uint32_t _sys_event = 0;

static void CLK_Config(void);
static void TIM4_Config(void);
static int Task_Register(int,int,Task,void*);
static int isEventSet(int evt);

int sys_init(void) {
    CLK_Config();
    TIM4_Config();
    for(int i=0; i<TASK_STACK_SIZE; i++) {
        sys_task_destory(i);
    }
    return 0;
}

int sys_task_reg_timer(int ms, Task foo, void *params) {
    return Task_Register(TASK_TIMER, ms, foo, params);
}

int sys_task_reg_alarm(int ms, Task foo, void *params) {
    return Task_Register(TASK_ALARM, ms, foo, params);
}

int sys_task_reg_signal(int signal, Task foo, void *params) {
    return Task_Register(TASK_SIGNAL, signal, foo, params);
}

int sys_task_destory(int task_id) {
    if(task_id <0) return -1;
    if(task_id >= TASK_STACK_SIZE) return -1;
    _sys_task[task_id].foo = 0;
    _sys_task[task_id].params = 0;
    _sys_task[task_id].argv = 0;
    _sys_task[task_id].ticks = 0;
    _sys_task[task_id].type = TASK_NONE;
    return 0;
}

void sys_run(void) {
    while(1) {
        int evt_systicks = 0;
        wfi();
        if(isEventSet(EVENT_SYSTICKS)) {
            evt_systicks = 1;
            sys_event_clear(EVENT_SYSTICKS);
        }
        for(int i=0; i<TASK_STACK_SIZE; i++) {
            switch(_sys_task[i].type) {
            case TASK_ALARM:
                if(evt_systicks) {
                    _sys_task[i].ticks -= 1;
                    if(_sys_task[i].ticks == 0) {
                        _sys_task[i].foo(_sys_task[i].params);
                        sys_task_destory(i);
                    }
                }
                break;
            case TASK_TIMER:
                if(evt_systicks) {
                    _sys_task[i].ticks -= 1;
                    if(_sys_task[i].ticks == 0) {
                        _sys_task[i].foo(_sys_task[i].params);
                        _sys_task[i].ticks = _sys_task[i].argv;
                    }
                }
                break;
            case TASK_SIGNAL:
                if(isEventSet(_sys_task[i].argv)) {
                    _sys_task[i].foo(_sys_task[i].params);
                    sys_event_clear(_sys_task[i].argv);
                }
                break;
            }
        }
    }
}

int sys_event_trigger(int evt) {
    _sys_event |= (uint32_t)(1<<evt);
    return 0;
}

int sys_event_clear(int evt) {
    _sys_event &= ~((uint32_t)(1<<evt));
    return 0;
}

uint32_t sys_uptime(void) {
    return _sys_ticks;
}


/* === helper functions === */
void _sys_ticks_cb(void) {
    _sys_ticks += 1;
    sys_event_trigger(EVENT_SYSTICKS);
}

static int isEventSet(int evt) {
    return ((_sys_event&((uint32_t)(1<<evt)))!=0);
}

static int Task_Register(int type, int argv, Task foo, void *params) {
    for(int i=0; i<TASK_STACK_SIZE; i++) {
        if(_sys_task[i].type == TASK_NONE) {
            _sys_task[i].foo = foo;
            _sys_task[i].params = params;
            _sys_task[i].argv = argv;
            _sys_task[i].ticks = argv;
            _sys_task[i].type = type;
            return 0;
        }
    }
    return -1;
}

/**
  * @brief  Configure system clock to run at 16Mhz
  * @param  None
  * @retval None
  */
static void CLK_Config(void)
{
    /* Initialization of the clock */
    /* Clock divider to HSI/1 */
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms
  * @param  None
  * @retval None
  */
static void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}
