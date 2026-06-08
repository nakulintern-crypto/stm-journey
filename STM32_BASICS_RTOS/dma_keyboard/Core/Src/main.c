#include "main.h"
volatile char data;
void gpio_uart_init(void)
{
    RCC->AHB1ENR |= (1 << 3);
    GPIOD->MODER &= ~((3 << 16) | (3 << 18));
    GPIOD->MODER |=  ((2 << 16) | (2 << 18));
    GPIOD->AFR[1] &= ~((15 << 0) | (15 << 4));
    GPIOD->AFR[1] |=  ((7 << 0) | (7 << 4));
}

void uart3_init(void)
{
    RCC->APB1ENR |= (1 << 18);
    USART3->BRR = 0x8B;
    USART3->CR1 |= (1 << 2);
    USART3->CR1 |= (1 << 3);
    USART3->CR3 |= (1 << 6);
    USART3->CR3 |= (1 << 7);
    USART3->CR1 |= (1 << 13);
}
void dma_rx_init(void)
{
    RCC->AHB1ENR |= (1 << 21);
    DMA1_Stream1->CR &= ~(1 << 0);
    while(DMA1_Stream1->CR & (1 << 0));
    DMA1->LIFCR |= (0x3D << 6);
    DMA1_Stream1->CR = 0;
    DMA1_Stream1->CR |= (4 << 25);
    DMA1_Stream1->CR &= ~(3 << 6);
    DMA1_Stream1->PAR = (uint32_t)&USART3->DR;
    DMA1_Stream1->M0AR = (uint32_t)&data;
    DMA1_Stream1->NDTR = 1;
    DMA1_Stream1->CR |= (1 << 0);
}
void dma_tx_init(void)
{
     DMA1_Stream3->CR &= ~(1 << 0);
    while(DMA1_Stream3->CR & (1 << 0));
    DMA1->LIFCR |= (0x3D << 22);
    DMA1_Stream3->CR = 0;
    DMA1_Stream3->CR |= (4 << 25);
    DMA1_Stream3->CR |= (1 << 6);
    DMA1_Stream3->PAR = (uint32_t)&USART3->DR;
    DMA1_Stream3->M0AR = (uint32_t)&data;
    DMA1_Stream3->NDTR = 1;
}
int main(void)
{
    HAL_Init();
    gpio_uart_init();
    uart3_init();
    dma_rx_init();
    dma_tx_init();
    while(1)
    {
        while(!(DMA1->LISR & (1 << 11)));
        DMA1_Stream1->CR &= ~(1 << 0);
        while(DMA1_Stream1->CR & (1 << 0));
        DMA1->LIFCR |= (0x3D << 6);
        DMA1_Stream3->NDTR = 1;
        DMA1_Stream3->CR |= (1 << 0);
        while(!(DMA1->LISR & (1 << 27)));
        DMA1_Stream3->CR &= ~(1 << 0);
        while(DMA1_Stream3->CR & (1 << 0));
        DMA1->LIFCR |= (0x3D << 22);
        DMA1_Stream1->NDTR = 1;
        DMA1_Stream1->CR |= (1 << 0);
    }
}
