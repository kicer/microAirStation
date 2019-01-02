#ifndef _SYS_H_
#define _SYS_H_


/* init base system
 * 16MHz HSI clock, TIM4 1ms systick
 */
extern int sys_init(void);

typedef void (*Task)(void *params);
extern int sys_task_reg_timer(int ms, Task foo, void *params);
extern int sys_task_reg_alarm(int ms, Task foo, void *params);
extern int sys_task_reg_signal(int signal, Task foo, void *params);
extern int sys_task_destory(int task_id);

/* system event functions
 * evt: 1~30
 */
extern int sys_event_trigger(int evt);
extern int sys_event_clear(int evt);

extern void sys_run(void);

/* system uptime, unit:1ms */
extern uint32_t sys_uptime(void);

#define TASK_STACK_SIZE  8
#define EVENT_SYSTICKS   0


#endif /* _SYS_H_ */