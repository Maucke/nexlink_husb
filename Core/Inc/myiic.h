#ifndef __MYIIC_H
#define __MYIIC_H
#include "main.h"

// IO方向设置
#define SDA_IN()                         \
    {                                    \
        GPIOA->MODER &= ~(3 << (1 * 2)); \
        GPIOA->MODER |= 0 << 1 * 2;      \
    } // PB1输入模式
#define SDA_OUT()                        \
    {                                    \
        GPIOA->MODER &= ~(3 << (1 * 2)); \
        GPIOA->MODER |= 1 << 1 * 2;      \
    } // PB1输出模式
// IO操作函数
#define IIC_SCL PAout(0) // SCL
#define IIC_SDA PAout(1) // SDA
#define READ_SDA PAin(1) // 输入SDA

// IIC所有操作函数
void IIC_Init(void);                 // 初始化IIC的IO口
void IIC_Start(void);                // 发送IIC开始信号
void IIC_Stop(void);                 // 发送IIC停止信号
void IIC_Send_Byte(u8 txd);          // IIC发送一个字节
u8 IIC_Recv_Byte(unsigned char ack); // IIC读取一个字节
u8 IIC_Wait_Ack(void);               // IIC等待ACK信号
void IIC_Ack(void);                  // IIC发送ACK信号
void IIC_NAck(void);                 // IIC不发送ACK信号

u8 IIC_Write_Byte(u8 addr, u8 reg, u8 data);
u8 IIC_Read_Byte(u8 addr, u8 reg);
u8 IIC_Write_Len(u8 addr, u8 reg, u8 len, u8 *buf);
u8 IIC_Read_Len(u8 addr, u8 reg, u8 len, u8 *buf);
		
#endif
