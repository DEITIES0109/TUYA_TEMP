/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
//#include "oled.h"
#include "sht30.h"
#include "wifi.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
uint8_t uart_temp;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t recv_dat[6] = {0};
    float temperature = 0.0;
    float humidity = 0.0;
    char ch[50] = {0};
	unsigned long Led_i = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	
    HAL_UART_Receive_IT(&huart1,&uart_temp,1);
	HAL_UART_Receive_IT(&huart2,&uart_temp,1);
	
    //OLED_Init();
    //OLED_Display_On();
    //OLED_Clear();
    SHT30_Reset();
    if (SHT30_Init() == HAL_OK)
	printf("\r\nsht30 init ok\r\n");	
//        OLED_ShowString(0, 0, "sht30 init ok", sizeof("sht30 init ok"));
    else
		printf("\r\nsht30 init fail\r\n");
//        OLED_ShowString(0, 0, "sht30 init fail", sizeof("sht30 init fail"));
	HAL_Delay(2000);
	wifi_protocol_init();
	
	HAL_Delay(2000);
	mcu_set_wifi_mode(SMART_CONFIG);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  
  
  while (1)
  {
	wifi_uart_service();
	//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
	  
	  switch(mcu_get_wifi_work_state())
		{
				case SMART_CONFIG_STATE:
				{
					if(Led_i >= 250)
					{
						Led_i = 0;						HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11);
					}					
				}//处于 Smart 配置状态，即 LED 快闪
				break;
				case AP_STATE:
				{
					if(Led_i >= 1500)
					{
						Led_i = 0;						HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11);
					}	
				}//处于 AP 配置状态，即 LED 慢闪
				break;
				case WIFI_NOT_CONNECTED:
				{
					if(Led_i >= 250)
					{
						Led_i = 0;						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,GPIO_PIN_SET);
					}	
				}//Wi-Fi 配置完成，正在连接路由器，即 LED 常暗
				break;
				case WIFI_CONNECTED:
				{
					if(Led_i >= 250)
					{
						Led_i = 0;						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,GPIO_PIN_RESET);
					}	
				}//路由器连接成功，即 LED 常亮
				break;
				default:break;
		}
		
        if (SHT30_Read_Dat(recv_dat) == HAL_OK) {
            if (SHT30_Dat_To_Float(recv_dat, &temperature, &humidity) == 0) {
                sprintf(ch, "temperature = %d, humidity = %d", (uint32_t) temperature, (uint32_t) humidity);
                //OLED_ShowString(0, 0, ch, strlen(ch));
				printf("\r\ntemperature = %d, humidity = %d\r\n", (uint32_t) temperature, (uint32_t) humidity);
            } else {
                sprintf(ch, "crc check fail.");
				printf("crc check fail.");
                //OLED_ShowString(0, 0, ch, strlen(ch));
            }
        } else {
            sprintf(ch, "read data from sht30 fail.");
			printf("read data from sht30 fail.");
            //OLED_ShowString(0, 0, ch, strlen(ch));
        }
        mcu_dp_value_update(1, (uint32_t) (temperature * 10));
        HAL_Delay(1000);
        mcu_dp_value_update(2, (uint32_t) (humidity));
        HAL_Delay(1000);
		
		//        OLED_Clear();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	 __disable_irq();
    while (1) {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
