#include "stm32f10x.h" 


volatile uint32_t count_0_1Hz = 0;
volatile uint32_t count_1Hz = 0;
volatile uint32_t count_10Hz = 0;

void GPIO_Configuration(void);

int main(void)
{

    GPIO_Configuration();

    if (SysTick_Config(SystemCoreClock / 1000))
    { 

        while (1);
    }

    while (1)
    {

    }
}


void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    

    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
}

void SysTick_Handler(void)
{

    count_0_1Hz++;
    count_1Hz++;
    count_10Hz++;
    if (count_0_1Hz >= 5000)
    {
        GPIOA->ODR ^= GPIO_Pin_0; 
        count_0_1Hz = 0;
    }


    if (count_1Hz >= 500)
    {
        GPIOA->ODR ^= GPIO_Pin_1;
        count_1Hz = 0;
    }


    if (count_10Hz >= 50)
    {
        GPIOA->ODR ^= GPIO_Pin_2;
        count_10Hz = 0;
    }
}
