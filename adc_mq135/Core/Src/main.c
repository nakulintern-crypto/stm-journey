#include "main.h"
#include <stdio.h>

void SystemClock_Config(void);

void GPIO_Init_Custom(void);
void USART3_Init(void);
void ADC1_Init(void);

uint16_t ADC_Read(void);

void sendchar(char c);
void sendstring(char *str);

/* MAIN FUNCTION */
int main(void)
{
    uint16_t gas_value;
    char msg[50];

    /* HAL Initialization */
    HAL_Init();

    /* Configure System Clock */
    SystemClock_Config();

    /* Initialize GPIO */
    GPIO_Init_Custom();

    /* Initialize USART3 */
    USART3_Init();

    /* Initialize ADC1 */
    ADC1_Init();

    /* Infinite Loop */
    while (1)
    {
        /* Read ADC Value */
        gas_value = ADC_Read();

        /* Convert to String */
        sprintf(msg, "Gas Value = %d\r\n", gas_value);

        /* Send via UART */
        sendstring(msg);

        HAL_Delay(500);
    }
}

/* GPIO Initialization */
void GPIO_Init_Custom(void)
{
    /* Enable GPIOA Clock */
    RCC->AHB1ENR |= (1 << 0);

    /* Enable GPIOD Clock */
    RCC->AHB1ENR |= (1 << 3);

    /* =========================
       PA0 -> ADC1_IN0
       ========================= */

    /* Clear Mode */
    GPIOA->MODER &= ~(3 << 0);

    /* Analog Mode */
    GPIOA->MODER |= (3 << 0);

    /* =========================
       PD8 -> USART3_TX
       ========================= */

    /* Clear Mode */
    GPIOD->MODER &= ~(3 << 16);

    /* Alternate Function Mode */
    GPIOD->MODER |= (2 << 16);

    /* Select AF7 for USART3 */
    GPIOD->AFR[1] &= ~(0xF << 0);

    GPIOD->AFR[1] |= (7 << 0);
}

/* USART3 Initialization */
void USART3_Init(void)
{
    /* Enable USART3 Clock */
    RCC->APB1ENR |= (1 << 18);

    /*
       APB1 Clock = 42 MHz

       Baud Rate = 115200

       USARTDIV = 42MHz / (16 * 115200)
                 = 22.786

       BRR = 0x16D
    */

    USART3->BRR = 0x16D;

    /* Enable Transmitter */
    USART3->CR1 |= (1 << 3);

    /* Enable USART */
    USART3->CR1 |= (1 << 13);
}

/* Send One Character */
void sendchar(char c)
{
    /* Wait Until TX Empty */
    while (!(USART3->SR & (1 << 7)));

    USART3->DR = c;
}

/* Send String */
void sendstring(char *str)
{
    while (*str)
    {
        sendchar(*str++);
    }
}

/* ADC1 Initialization */
void ADC1_Init(void)
{
    /* Enable ADC1 Clock */
    RCC->APB2ENR |= (1 << 8);

    /* ADC OFF Initially */
    ADC1->CR1 = 0;

    ADC1->CR2 = 0;

    /* Sample Time for Channel 0 */
    ADC1->SMPR2 |= (7 << 0);

    /* Channel 0 Sequence */
    ADC1->SQR3 = 0;

    /* Enable ADC */
    ADC1->CR2 |= (1 << 0);

    /* Small Delay */
    for (int i = 0; i < 1000; i++);
}

/* Read ADC Value */
uint16_t ADC_Read(void)
{
    /* Start Conversion */
    ADC1->CR2 |= (1 << 30);

    /* Wait Until Conversion Complete */
    while (!(ADC1->SR & (1 << 1)));

    return ADC1->DR;
}

/* System Clock Configuration */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Enable Power Control Clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Voltage Scaling */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Configure HSE + PLL */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;

    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;

    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;

    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    RCC_OscInitStruct.PLL.PLLM = 4;

    RCC_OscInitStruct.PLL.PLLN = 168;

    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;

    RCC_OscInitStruct.PLL.PLLQ = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while (1);
    }

    /* Configure Clocks */
    RCC_ClkInitStruct.ClockType =
            RCC_CLOCKTYPE_HCLK |
            RCC_CLOCKTYPE_SYSCLK |
            RCC_CLOCKTYPE_PCLK1 |
            RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;

    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        while (1);
    }
}

/* Error Handler */
void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}
