#include "stm32f4xx.h" // Replace with your toolchain's specific header if needed

void Clock_Init(void);
void LED_Init(void);
void I2C1_GPIO_Init(void);
void I2C1_Master_Init(void);
void I2C1_Start(void);
void I2C1_Stop(void);
uint8_t I2C1_WriteAddress(uint8_t addr);
void I2C1_WriteData(uint8_t data);
void delay(void);

int main(void)
{
    Clock_Init();
    LED_Init();
    I2C1_GPIO_Init();
    I2C1_Master_Init();

    while(1)
    {
        // If bus is stuck or busy, force a software reset sequence to clear it
        if(I2C1->SR2 & I2C_SR2_BUSY)
        {
            I2C1->CR1 |=  I2C_CR1_SWRST;
            for(volatile int i = 0; i < 1000; i++);
            I2C1->CR1 &= ~I2C_CR1_SWRST;

            I2C1->CR2   = 16;
            I2C1->CCR   = 80;
            I2C1->TRISE = 17;
            I2C1->CR1  |= I2C_CR1_PE;
        }

        I2C1_Start();

        // Write to Slave address 0x42
        if(I2C1_WriteAddress(0x42))
        {
            I2C1_WriteData('A');
        }

        I2C1_Stop();

        // Toggle Built-in LED (PC13 on Black Pill) to show life
        GPIOC->ODR ^= (1 << 13);
        delay();
    }
}

void Clock_Init(void)
{
    /* Enable HSI (16MHz) and wait until ready */
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    /* Reset clock configuration register */
    RCC->CFGR = 0x00000000;

    /* Select HSI as system clock source */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |=  RCC_CFGR_SW_HSI;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

    /* Set all prescalers to 1 (AHB, APB1, APB2 run at 16MHz) */
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
}

void LED_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER &= ~(3U << (13 * 2));
    GPIOC->MODER |=  (1U << (13 * 2)); // PC13 Output
    GPIOC->ODR   |=  (1 << 13);        // Turn off (Active Low on Black Pill)
}

void I2C1_GPIO_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* PB6 & PB7 Alternate Function Configuration */
    GPIOB->MODER &= ~(0xFU << 12);
    GPIOB->MODER |=  (0xAU << 12);

    /* Open Drain Configuration */
    GPIOB->OTYPER |= (1 << 6) | (1 << 7);

    /* High Speed Configuration */
    GPIOB->OSPEEDR |= (0xFU << 12);

    /* Enable Internal Pull-ups */
    GPIOB->PUPDR &= ~(0xFU << 12);
    GPIOB->PUPDR |=  (0x5U << 12);

    /* Assign AF4 (I2C1) to PB6 and PB7 */
    GPIOB->AFR[0] &= ~(0xFFU << 24);
    GPIOB->AFR[0] |=  (0x44U << 24);
}

void I2C1_Master_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    /* Software Reset the peripheral */
    I2C1->CR1 |=  I2C_CR1_SWRST;
    for(volatile int i = 0; i < 1000; i++);
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    for(volatile int i = 0; i < 1000; i++);

    I2C1->CR2   = 16;    /* APB1 Clock Frequency = 16MHz */
    I2C1->CCR   = 80;    /* 100kHz standard mode configuration */
    I2C1->TRISE = 17;    /* Maximum rise time calculation */

    I2C1->CR1 |= I2C_CR1_PE; // Enable Peripheral
}

void I2C1_Start(void)
{
    uint32_t timeout = 100000;
    while((I2C1->SR2 & I2C_SR2_BUSY) && --timeout);

    I2C1->CR1 |= I2C_CR1_START;

    timeout = 100000;
    while(!(I2C1->SR1 & I2C_SR1_SB) && --timeout);
}

uint8_t I2C1_WriteAddress(uint8_t addr)
{
    volatile uint32_t temp;
    uint32_t timeout;

    I2C1->DR = (addr << 1); // Left shift to match standard I2C layout

    timeout = 100000;
    while(!(I2C1->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) && --timeout);

    // Handle Acknowledgement failure (NACK) safely without hanging
    if(!timeout || (I2C1->SR1 & I2C_SR1_AF))
    {
        I2C1->SR1 &= ~I2C_SR1_AF; // Clear the Acknowledge Failure Flag
        I2C1_Stop();
        return 0;
    }

    /* Clear the ADDR bit sequence by reading SR1 and SR2 */
    temp = I2C1->SR1;
    temp = I2C1->SR2;
    (void)temp;

    return 1;
}

void I2C1_WriteData(uint8_t data)
{
    uint32_t timeout = 100000;
    while(!(I2C1->SR1 & I2C_SR1_TXE) && --timeout);
    I2C1->DR = data;

    timeout = 100000;
    while(!(I2C1->SR1 & I2C_SR1_BTF) && --timeout);
}

void I2C1_Stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
}

void delay(void)
{
    for(volatile uint32_t i = 0; i < 1600000; i++);
}
