/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  *  Paul: copy/pasted Project 9 and put discrete handling into a call instead of in 'main'
  *
  *  Principle of Debouncer:  the toggle switch state is counted and when it exceeds a threshold,
  *  the debounce is considered complete.  For a pushbutton switch, there is also a
  *  count when the switch is released so as to minimize transients as the switch is
  *  released.
  *
  *  Implementation:  In order to allow multiple switches to use common processing,
  *  a C function call is used (one for toggle switch type 'Toggle_Debounce' and one
  *  for PB_Moment switch type 'PB_Moment_Debounce').
  *  Because the count states cannot be shared between switches, the count states for
  *  each switch are stored outside the function call, then passed at calling time to
  *  the function call.
  *
  *  Also, note that a function call cannot return multiple values.  Therefore, a group
  *  of Common variables are declared and used to receive the particular switch's
  *  stored state values prior to the function call, which uses these  Common
  *  variables.  After the function call return, the main() code then updates the
  *  switch's stored state values with the value of the  Common variables.  Also
  *  note that these are passed to the function call by pointer, although that probably
  *  is not necessary.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Common definitions to enable all function calls */
static uint32_t SWITCH_INPUT;

/* PB_Moment_SW Common definitions to enable debounce function call */
static uint32_t temp_PB_Moment_RELEASE_COUNTER;
static uint32_t temp_PB_Moment_DEBOUNCE_COUNTER;
static uint32_t temp_PB_Moment_DEBOUNCE_COMPLETE;

/* Toggle_SW Common definitions to enable debounce function call */
static uint32_t temp_Toggle_DEBOUNCE_COUNTER;

/* PB_Moment_SW1 - specific definitions */
static uint32_t PB_Moment_SW1_DEBOUNCE_THRESHOLD;
static uint32_t PB_Moment_SW1_RELEASE_COUNTER_THRESHOLD;
static uint32_t PB_Moment_SW1_RELEASE_COUNTER;
static uint32_t PB_Moment_SW1_DEBOUNCE_COUNTER;
static uint32_t PB_Moment_SW1_DEBOUNCE_COMPLETE;
static uint32_t PB_Moment_SW1_SWITCH_OUTPUT;

/* TOGGLE SW - specific definitions USE THESE CASESSSSSS */
static uint32_t TOGGLE_SW1_DEBOUNCE_THRESHOLD;
static uint32_t TOGGLE_SW1_DEBOUNCE_COUNTER;
static uint32_t TOGGLE_SW1_SWITCH_OUTPUT;
static uint32_t Toggle_SW1_SWITCH_OUTPUT_old;

static uint32_t TOGGLE_SW2_DEBOUNCE_THRESHOLD;
static uint32_t TOGGLE_SW2_DEBOUNCE_COUNTER;
static uint32_t TOGGLE_SW2_SWITCH_OUTPUT;
static uint32_t Toggle_SW2_SWITCH_OUTPUT_old;

/* Private function prototypes -----------------------------------------------*/
int PB_Moment_Debounce(uint32_t *PB_Moment_SW1_PRESSED,     uint32_t PB_Moment_SW1_DEBOUNCE_THRESHOLD,     uint32_t PB_Moment_SW1_RELEASE_COUNTER_THRESHOLD, uint32_t* temp_PB_Moment_DEBOUNCE_COUNTER, uint32_t* temp_PB_Moment_RELEASE_COUNTER, uint32_t* PB_Moment_SW1_DEBOUNCE_COMPLETE);
int Toggle_Debounce(uint32_t *Toggle_SW1_PRESSED, uint32_t TOGGLE_SW1_DEBOUNCE_THRESHOLD, uint32_t* temp_Toggle_SW1_DEBOUNCE_COUNTER, uint32_t* Toggle_SW1_SWITCH_OUTPUT_old);


/* Main Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* static GPIO_InitTypeDef  GPIO_InitStruct; */

/* executive variables */
static uint32_t intrpt_numloops1; /* set the number of times the loop 1 interrupt will fire before action taken */
static uint32_t intrpt_numloops2; /* set the number of times the loop 2 interrupt will fire before action taken */

static uint32_t intrpt_loopcount1; /* current count of number of loops of Loop 1 */
static uint32_t intrpt_loopcount2; /* current count of number of loops of Loop 2 */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* Initialize Discrete i/o  */
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Initialize TOGGLE_SW1  */
	TOGGLE_SW1_DEBOUNCE_THRESHOLD = 5;
	TOGGLE_SW1_DEBOUNCE_COUNTER = 0;
	Toggle_SW1_SWITCH_OUTPUT_old = 1;

	/* Initialize TOGGLE_SW2  */
	TOGGLE_SW2_DEBOUNCE_THRESHOLD = 5;
	TOGGLE_SW2_DEBOUNCE_COUNTER = 0;
	Toggle_SW2_SWITCH_OUTPUT_old = 1;

	/* Initialize PB_Moment_SW1  */
	PB_Moment_SW1_DEBOUNCE_COUNTER = 0;
	PB_Moment_SW1_RELEASE_COUNTER = 0;
	PB_Moment_SW1_DEBOUNCE_COMPLETE = 0;
	PB_Moment_SW1_DEBOUNCE_THRESHOLD = 5;
	PB_Moment_SW1_RELEASE_COUNTER_THRESHOLD = 3;

	/* STM32F4xx HAL library initialization:
	       - Configure the Flash prefetch, instruction and Data caches
	       - Configure the Systick to generate an interrupt each 1 msec
	       - Set NVIC Group Priority to 4
	       - Global MSP (MCU Support Package) initialization
	     */
	  HAL_Init();

	  /* Configure the system clock to 100 MHz */
	  SystemClock_Config();

	  /*##-1- Enable GPIOA Clock (to be able to program the configuration registers) */
	  __HAL_RCC_GPIOA_CLK_ENABLE();


  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();


  /* Configure the system clock */
  SystemClock_Config();


  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* Executive initialization */

    intrpt_numloops1 = 1000;
    intrpt_numloops2 = 2200;
    intrpt_loopcount1 = 1;
    intrpt_loopcount2 = 1;

	  /****** Executive start ******/
	  while (1)
	  {
	    /****  Frame 1 begin - Frequency = (1 div intrpt_numloops1) ******/
	    while ( (uwTick-intrpt_loopcount1) > intrpt_numloops1 )
	    {
	        /****  Frame 1 - Task 1.1 - Read Discrete Inputs ******/

	        /****  Frame 1 - Task 1.2 - Debounce switches *****/

		  	  /* Toggle_SW1 debounce processing BEGIN ******/
	    	  /*    TOGGLE SWITCH Input - flip lever, then state changes ******/
	    	  /*                        - flip back, then state changes ******/

		  	  SWITCH_INPUT = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
			  temp_Toggle_DEBOUNCE_COUNTER = TOGGLE_SW1_DEBOUNCE_COUNTER;
			  TOGGLE_SW1_SWITCH_OUTPUT = Toggle_Debounce(&SWITCH_INPUT, TOGGLE_SW1_DEBOUNCE_THRESHOLD, &temp_Toggle_DEBOUNCE_COUNTER, &Toggle_SW1_SWITCH_OUTPUT_old);
			  TOGGLE_SW1_DEBOUNCE_COUNTER  = temp_Toggle_DEBOUNCE_COUNTER;

		  	  /* Toggle_SW1 debounce processing END */


			  /* PB_Moment_SW1 debounce processing BEGIN */
	    	  /*    Pushbutton SWITCH Input - press and hold for a specified time in order to avoid mechanical */
	    	  /*    transients, then state changes */
			  /*    Must also release for a specified time before re-pressing is recognized, in order to */
			  /*    avoid mechanical transients in the release process */
			  /*																			*/
			  /*    Note that this processes momentary switches; does not latch the state  */

			  SWITCH_INPUT = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

			  temp_PB_Moment_DEBOUNCE_COUNTER = PB_Moment_SW1_DEBOUNCE_COUNTER;
			  temp_PB_Moment_RELEASE_COUNTER = PB_Moment_SW1_RELEASE_COUNTER;
			  temp_PB_Moment_DEBOUNCE_COMPLETE = PB_Moment_SW1_DEBOUNCE_COMPLETE;

			  PB_Moment_SW1_SWITCH_OUTPUT = PB_Moment_Debounce(&SWITCH_INPUT, PB_Moment_SW1_DEBOUNCE_THRESHOLD, PB_Moment_SW1_RELEASE_COUNTER_THRESHOLD, &temp_PB_Moment_DEBOUNCE_COUNTER, &temp_PB_Moment_RELEASE_COUNTER, &temp_PB_Moment_DEBOUNCE_COMPLETE);

			  PB_Moment_SW1_DEBOUNCE_COUNTER  = temp_PB_Moment_DEBOUNCE_COUNTER;
			  PB_Moment_SW1_RELEASE_COUNTER  = temp_PB_Moment_RELEASE_COUNTER;
			  PB_Moment_SW1_DEBOUNCE_COMPLETE = temp_PB_Moment_DEBOUNCE_COMPLETE;

			  /* PB_Moment_SW1 debounce processing END */

	        /****  Frame 1 - Task 2 - ******/
	        /****  Frame 1 - Task 3 - ******/
	        /****  Frame 1 - Task 8 - Set Discrete Outputs ******/

	    	/**** End of all Tasks in Frame 1  ****/
	        intrpt_loopcount1 = uwTick; /** reset loopcount1 for Frame 1 restart **/
	    }
	    /****  Frame 1 end *********************************************/

	    /****  Frame 2 begin - Frequency = (1 div intrpt_numloops2) ******/
	    while ( (uwTick-intrpt_loopcount2) > intrpt_numloops2 )
	    {
	        /****  Frame 2 - Task 1 ******/
		  	  /* Toggle_SW2 debounce processing BEGIN ******/
	    	  /*    TOGGLE SWITCH Input - flip lever, then state changes ******/
	    	  /*                        - flip back, then state changes ******/

		  	  SWITCH_INPUT = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
			  temp_Toggle_DEBOUNCE_COUNTER = TOGGLE_SW2_DEBOUNCE_COUNTER;
			  TOGGLE_SW2_SWITCH_OUTPUT = Toggle_Debounce(&SWITCH_INPUT, TOGGLE_SW2_DEBOUNCE_THRESHOLD, &temp_Toggle_DEBOUNCE_COUNTER, &Toggle_SW2_SWITCH_OUTPUT_old);
			  TOGGLE_SW2_DEBOUNCE_COUNTER  = temp_Toggle_DEBOUNCE_COUNTER;

		  	  /* Toggle_SW2 debounce processing END */

	        intrpt_loopcount2 = uwTick; /** reset loopcount2 for frame restart **/
	    } /****  Frame 2 end *********************************************/

	  } /****** Executive End ******/
}

int Toggle_Debounce(uint32_t* Toggle_PRESSED, uint32_t TOGGLE_DEBOUNCE_THRESHOLD, uint32_t* temp_Toggle_DEBOUNCE_COUNTER, uint32_t* Toggle_SWITCH_OUTPUT_old)
{
	if((*Toggle_PRESSED) != (*Toggle_SWITCH_OUTPUT_old)) /* process if switch position different than stable value */
	{
		*temp_Toggle_DEBOUNCE_COUNTER = *temp_Toggle_DEBOUNCE_COUNTER + 1;
		if((*temp_Toggle_DEBOUNCE_COUNTER == TOGGLE_DEBOUNCE_THRESHOLD)) /* debounce count exceeded */
		{
			*temp_Toggle_DEBOUNCE_COUNTER = 0; /* reset debounce count */
			if (*Toggle_PRESSED == 0)  /* reverse discrete state */
				{
				*Toggle_SWITCH_OUTPUT_old = 0;
			}else
				{
				*Toggle_SWITCH_OUTPUT_old = 1;
				}
		}else
			{
			}
	}else
		{
		*temp_Toggle_DEBOUNCE_COUNTER = 0; /* reset debounce count */
		}
	return(*Toggle_SWITCH_OUTPUT_old);
}

int PB_Moment_Debounce(uint32_t* PB_Moment_PRESSED, uint32_t PB_Moment_DEBOUNCE_THRESHOLD, uint32_t PB_Moment_RELEASE_COUNTER_THRESHOLD, uint32_t* temp_PB_Moment_DEBOUNCE_COUNTER, uint32_t* temp_PB_Moment_RELEASE_COUNTER, uint32_t* temp_PB_Moment_DEBOUNCE_COMPLETE)
{
	if((*PB_Moment_PRESSED == 0) && (*temp_PB_Moment_DEBOUNCE_COUNTER < PB_Moment_DEBOUNCE_THRESHOLD) && (*temp_PB_Moment_DEBOUNCE_COMPLETE == 0))
	    {
	    	/* inc deb cntr */
	    	*temp_PB_Moment_DEBOUNCE_COUNTER = *temp_PB_Moment_DEBOUNCE_COUNTER + 1;
			*temp_PB_Moment_RELEASE_COUNTER = 0; /* prevent release reentry if button pressed during release count */

	    }else if ((*PB_Moment_PRESSED == 1) && (*temp_PB_Moment_DEBOUNCE_COUNTER < PB_Moment_DEBOUNCE_THRESHOLD) && (*temp_PB_Moment_DEBOUNCE_COMPLETE == 0))
	    {
	    	/* reset deb cntr */
	    	*temp_PB_Moment_DEBOUNCE_COUNTER = 0;

	    }else if((*PB_Moment_PRESSED == 0) && (*temp_PB_Moment_DEBOUNCE_COUNTER == 5) && (*temp_PB_Moment_DEBOUNCE_COMPLETE == 0))
	    {
	    	/* set deb cmplt */
	    	/* reset deb cntr */
		*temp_PB_Moment_DEBOUNCE_COMPLETE = 1;
		*temp_PB_Moment_DEBOUNCE_COUNTER = 0;
		*temp_PB_Moment_RELEASE_COUNTER = 0; /* prevent release reentry if button pressed during release count */

	    }else if((*PB_Moment_PRESSED == 0) && (*temp_PB_Moment_DEBOUNCE_COUNTER == 0) && (*temp_PB_Moment_DEBOUNCE_COMPLETE == 1))
	    {
			*temp_PB_Moment_RELEASE_COUNTER = 0; /* prevent release reentry if button pressed during release count */
	    }else if((*PB_Moment_PRESSED == 1) && (*temp_PB_Moment_DEBOUNCE_COUNTER == 0) && (*temp_PB_Moment_DEBOUNCE_COMPLETE == 1) && (*temp_PB_Moment_RELEASE_COUNTER < PB_Moment_RELEASE_COUNTER_THRESHOLD))
	    {
	    	/* inc rel cntr */
		*temp_PB_Moment_RELEASE_COUNTER = *temp_PB_Moment_RELEASE_COUNTER + 1;

	    }else if((*PB_Moment_PRESSED == 1) && (*temp_PB_Moment_DEBOUNCE_COUNTER == 0) && (*temp_PB_Moment_DEBOUNCE_COMPLETE == 1) && (*temp_PB_Moment_RELEASE_COUNTER == PB_Moment_RELEASE_COUNTER_THRESHOLD))
	    {
	    	/* reset deb cmplt */
	    	/* reset rel cntr */
		*temp_PB_Moment_DEBOUNCE_COMPLETE = 0;
		*temp_PB_Moment_RELEASE_COUNTER = 0;
	    }else
	    {
	    }

	return(*temp_PB_Moment_DEBOUNCE_COMPLETE);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
  while (1)
  {
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
