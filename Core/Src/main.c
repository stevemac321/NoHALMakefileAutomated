/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include "stm32f4xx.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PFP */

void enable_default_clock(void);
void uart2_init(void);
void ADC1_init(void);
void dummy() {printf("test complete\n");}

/* USER CODE END PFP */

int main(void)
{

  /* USER CODE BEGIN 1 */
	//enable_default_clock();
	uart2_init();
	ADC1_init();
  /* USER CODE END 1 */

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	printf("hello uart from barebones\n");
	dummy();
#if 0
  while (1)
  {
    /* USER CODE END WHILE */


    /* USER CODE BEGIN 3 */
  }
#endif
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void ADC1_init(void)
{
    // 1. Enable clocks for GPIOA and ADC1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;    // Enable ADC1 clock

    // 2. Configure PA0 as analog input (ADC_IN0)
    GPIOA->MODER |= GPIO_MODER_MODER0;     // MODER0 = 11 (analog mode)
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0;    // No pull-up/pull-down

    // 3. Set ADC prescaler (PCLK2 divided by 4)
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |= ADC_CCR_ADCPRE_0;          // ADCPRE = 01 → PCLK2 / 4

    // 4. Configure ADC1
    ADC1->CR1 = 0;                         // 12-bit resolution, single conversion
    ADC1->CR2 = ADC_CR2_CONT;              // Continuous conversion mode (optional)

    ADC1->SQR1 = 0;                        // One conversion in sequence
    ADC1->SQR3 = 0;                        // Channel 0 (PA0)

    ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;       // Sample time = 144 cycles

    // 5. Enable ADC
    ADC1->CR2 |= ADC_CR2_ADON;             // Power on ADC
    for (volatile int i = 0; i < 1000; ++i); // Brief delay

    // Optional: Start conversion
    // ADC1->CR2 |= ADC_CR2_SWSTART;
}

void enable_default_clock(void) {
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
}

void uart2_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA2 = TX, PA3 = RX
    GPIOA->MODER &= ~((3 << (2 * 2)) | (3 << (3 * 2)));
    GPIOA->MODER |=  ((2 << (2 * 2)) | (2 << (3 * 2)));

    GPIOA->AFR[0] &= ~((0xF << (4 * 2)) | (0xF << (4 * 3)));
    GPIOA->AFR[0] |=  ((7 << (4 * 2)) | (7 << (4 * 3))); // AF7

    USART2->BRR = 139; // 16MHz / 115200 = ~139
    USART2->CR1 = USART_CR1_TE | USART_CR1_UE;
}

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        while (!(USART2->SR & USART_SR_TXE)) { }
        USART2->DR = ptr[i];
    }
    return len;
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	 __asm volatile ("cpsid i" : : : "memory");
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
