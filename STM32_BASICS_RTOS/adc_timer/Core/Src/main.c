#include"main.h"
#include"stdio.h"
volatile uint16_t adc_value;
char msg[50];
void gpio_init()
{
	RCC->AHB1ENR|=(1<<0);
	GPIOA->MODER&=~(3<<0);
	GPIOA->MODER |=(3<<0);
	GPIOA->PUPDR &=~(3<<0);

}

void adc_init()
{
	RCC->APB2ENR |=(1<<8);
	ADC1->SMPR2 |=(7<<0);
	ADC1->SQR1=0;
	ADC1->SQR3=0;
	ADC1->CR2|=(1<<0);
}
uint16_t adc_read(void)
{
 	ADC1->CR2|=(1<<30);
	while(!(ADC1->SR &ADC_SR_EOC));
 	return ADC1->DR;
}
void tim2_init()
{
	RCC->APB1ENR|=(1<<0);
	TIM2->PSC =83999;
	TIM2->ARR=2999;
	TIM2->EGR|=(1<<0);
	TIM2->CR1|=(1<<0);
}
void USART3_GPIO_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    GPIOD->MODER &= ~(0xFU << 16);
    GPIOD->MODER |=  (0xAU << 16);


    GPIOD->AFR[1] &= ~(0xFFU << 0);
    GPIOD->AFR[1] |=  (0x77U << 0);
}
void USART3_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    USART3->BRR = 139;

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
    {
        USART3_WriteChar(*str++);
    }
}
int main()
{
gpio_init();
adc_init();
tim2_init();
USART3_GPIO_Init();
USART3_Init();
while(1)
{
    if(TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;

        adc_value = adc_read();

        sprintf(msg,"ADC = %u\r\n",adc_value);

        USART3_WriteString(msg);
    }
}


}





