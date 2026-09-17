#include "stm32f10x.h"
#include <stdint.h>

#define MA_LOP       "D23CQDT01"
#define MA_NHOM      "Nhom03"
#define BUFFER_SIZE  100

char buffer[BUFFER_SIZE];
uint8_t buffer_index = 0;
void USART1_SendChar(char c)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

    USART_SendData(USART1, (uint16_t)c);
}
void USART1_SendString(const char *str)
{
    while (*str != '\0')
    {
        USART1_SendChar(*str);
        str++;
    }
}
void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1,
        ENABLE
    );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode =
        USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}
void Buffer_Clear(void)
{
    uint8_t i;

    for (i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = '\0';
    }

    buffer_index = 0;
}
void Send_Result(void)
{
    USART1_SendString("\r\n");
    USART1_SendString(MA_LOP);
    USART1_SendString("_");
    USART1_SendString(MA_NHOM);
    USART1_SendString(": ");
    USART1_SendString(buffer);
    USART1_SendString("\r\n");

    Buffer_Clear();
}
int main(void)
{
    uint16_t received_char;

    USART1_Init();
    Buffer_Clear();

    USART1_SendString("\r\n");
    USART1_SendString("UART READY\r\n");
    USART1_SendString("Nhap tin nhan, ket thuc bang !\r\n");

    while (1)
    {
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
        {
            received_char = USART_ReceiveData(USART1);
            if (received_char == '!')
            {
                Send_Result();
            }
            else if (received_char == '\b')
            {
                if (buffer_index > 0)
                {
                    buffer_index--;
                    buffer[buffer_index] = '\0';

                    USART1_SendString("\b \b");
                }
            }
            else if (received_char == '\r' ||
                     received_char == '\n')
            {
            }
            else
            {
                if (buffer_index < BUFFER_SIZE - 1)
                {
                    buffer[buffer_index] = (char)received_char;
                    buffer_index++;

                    buffer[buffer_index] = '\0';
                    USART1_SendChar((char)received_char);
                }
            }
        }
    }
}
