#include "stm8s.h"
#include "sys.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static void led_flash(void *p) {
    GPIO_WriteReverse(GPIOB, GPIO_PIN_5);
}

void main(void)
{
    /* device init */
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);

    /* system handler */
    sys_init();
    sys_task_reg_timer(100, led_flash, 0);
    sys_run();

    while(1) WWDG_SWReset(); /* reboot */
}


#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while(1) {
    }
}
#endif
