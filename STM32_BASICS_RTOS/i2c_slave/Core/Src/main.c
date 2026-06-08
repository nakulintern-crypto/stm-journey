#include "stm32f4xx.h"
#include <stdio.h>

void Clock_Init(void);
void USART3_GPIO_Init(void);
void USART3_Init(void);
void USART3_WriteChar(char ch);
void USART3_WriteString(char *str);
void I2C1_GPIO_Init(void);
void I2C1_Slave_Init(void);
uint8_t I2C1_Slave_ReadByte(void);

static char msg[64];

int main(void)
{
    uint8_t data;

    Clock_Init();

    USART3_GPIO_Init();
    USART3_Init();
    USART3_WriteString("\r\n=== SLAVE BOOT ===\r\n");
    USART3_WriteString("UART OK\r\n");

    I2C1_GPIO_Init();
    I2C1_Slave_Init();
    USART3_WriteString("I2C SLAVE READY @ 0x42\r\n");

    while(1)
    {
        USART3_WriteString("Waiting for Master...\r\n");
        data = I2C1_Slave_ReadByte();

        if(data != 0)
        {
            sprintf(msg, "SUCCESS! Got data: %c (0x%02X)\r\n", data, data);
            USART3_WriteString(msg);
        }
    }
}

void Clock_Init(void)
{
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR = 0x00000000;

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |=  RCC_CFGR_SW_HSI;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

    RCC->CFGR &= ~RCC_CFGR_HPRE;
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
}

void USART3_GPIO_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    /* PD8 and PD9 Alternate Function mode */
    GPIOD->MODER &= ~(0xFU << 16);
    GPIOD->MODER |=  (0xAU << 16);

    /* AF7 configuration for USART3 */
    GPIOD->AFR[1] &= ~(0xFFU);
    GPIOD->AFR[1] |=  (0x77U);
}

void USART3_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    /* 16MHz clock / (115200 baudrate * 16) = ~8.68. Mantissa = 8, Fraction = 0.68 * 16 = 11 -> BRR = 0x8B (139) */
    USART3->BRR  = 139;
    USART3->CR1 |= USART_CR1_TE;
    USART3->CR1 |= USART_CR1_RE;
    USART3->CR1 |= USART_CR1_UE;
}

void USART3_WriteChar(char ch)
{
    while(!(USART3->SR & USART_SR_TXE));
    USART3->DR = ch;
}

void USART3_WriteString(char *str)
{
    while(*str)
        USART3_WriteChar(*str++);
}

void I2C1_GPIO_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* PB8 and PB9 Alternate Function Mode */
    GPIOB->MODER &= ~(0xFU << 16);
    GPIOB->MODER |=  (0xAU << 16);

    /* Open Drain Mode */
    GPIOB->OTYPER |= (1 << 8) | (1 << 9);

    /* High Speed */
    GPIOB->OSPEEDR |= (0xFU << 16);

    /* Enable Internal Pull-ups */
    GPIOB->PUPDR &= ~(0xFU << 16);
    GPIOB->PUPDR |=  (0x5U << 16);

    /* AF4 configuration for I2C1 on high bits */
    GPIOB->AFR[1] &= ~(0xFFU);
    GPIOB->AFR[1] |=  (0x44U);
}

void I2C1_Slave_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    I2C1->CR1 |=  I2C_CR1_SWRST;
    for(volatile int i = 0; i < 1000; i++);
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    for(volatile int i = 0; i < 1000; i++);

    I2C1->CR2  = 16;

    /* Address setting bit 14 requirement + address 0x42 mapped to bits [7:1] */
    I2C1->OAR1 = (1 << 14) | (0x42 << 1);

    I2C1->CR1 |= I2C_CR1_ACK;
    I2C1->CR1 |= I2C_CR1_PE;
}

uint8_t I2C1_Slave_ReadByte(void)
{
    volatile uint32_t temp;
    uint8_t data = 0;
    uint32_t timeout;

    // 1. Wait for address bit match from master
    timeout = 5000000;
    while(!(I2C1->SR1 & I2C_SR1_ADDR) && --timeout);

    if(!timeout)
    {
        USART3_WriteString("TIMEOUT: No ADDR\r\n");
        return 0;
    }

    USART3_WriteString("ADDR matched! ");

    // 2. Clear ADDR sequence correctly (Read SR1 then SR2)
    temp = I2C1->SR1;
    temp = I2C1->SR2;
    (void)temp;

    // 3. Wait for data register to populate
    timeout = 5000000;
    while(!(I2C1->SR1 & I2C_SR1_RXNE) && --timeout);

    if(!timeout)
    {
        USART3_WriteString("TIMEOUT: No Data arrived\r\n");
        return 0;
    }

    // 4. Capture Data Byte
    data = (uint8_t)I2C1->DR;

    // 5. Watch for the Master ending transaction with a STOP flag
    timeout = 5000000;
    while(!(I2C1->SR1 & I2C_SR1_STOPF) && --timeout);

    // 6. Clear STOPF sequence correctly (Read SR1 followed by a safe write back to CR1)
    temp = I2C1->SR1;
    (void)temp;
    I2C1->CR1 = I2C1->CR1;

    // 7. Ensure ACK remains active for subsequent runs
    I2C1->CR1 |= I2C_CR1_ACK;

    return data;
}
