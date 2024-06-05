#include "lcd.h"
#include "spi.h"
#include "stdbool.h"

#define delay HAL_Delay
volatile long remainsize = 0;

// LCD串行数据写入
static void LCD_Writ_Bus(uint8_t dat)
{
    HAL_SPI_Transmit(&hspi1, &dat, 1, 100);
}

// LCD写入8位数据
void LCD_WR_DATA8(uint8_t dat)
{
    LCD_CS_OUT(0);

    LCD_Writ_Bus(dat);
	
    LCD_CS_OUT(1);
}

// LCD写入16位数据
void LCD_WR_DATA(uint16_t dat)
{
    LCD_CS_OUT(0);
	
    LCD_Writ_Bus(dat >> 8);
    LCD_Writ_Bus(dat);
	
    LCD_CS_OUT(1);
}

// LCD写入命令
void LCD_WR_REG(uint8_t dat)
{
    LCD_CS_OUT(0);

    LCD_DC_OUT(0); // 写命令
    LCD_Writ_Bus(dat);
    LCD_DC_OUT(1); // 写数据

    LCD_CS_OUT(1);
}

// 启用SPI DMA连续发送单个16bit数据
void LCD_DMA_Transfer16Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state)
{
	// 清除 DMA 控制寄存器的相关设置
    LCD_SPI_TX_DMA->CR &= ~DMA_SxCR_MINC; 

//    // 设置 DMA 存储器和外设数据长度为半字(16bit)
//    LCD_SPI_TX_DMA->CR |= DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0; 

    // 根据传入的状态设置是否使能存储器地址增量
    if (state == DMA_MEMINC_ENABLE)
        LCD_SPI_TX_DMA->CR |= DMA_SxCR_MINC; 

    HAL_SPI_Transmit_DMA(&hspi1, pData, size); // 启用DMA传输
}

/*
 *功能: 设置起始和结束地址
 *参数1: @x1,x2 - 设置列的起始和结束地址
 *参数1: @y1,y2 - 设置行的起始和结束地址
 */
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_WR_REG(0x2a); // 列地址设置
    LCD_WR_DATA(x1);
    LCD_WR_DATA(x2);
    LCD_WR_REG(0x2b); // 行地址设置
    LCD_WR_DATA(y1);
    LCD_WR_DATA(y2);
    LCD_WR_REG(0x2c); // 储存器写
}

// LCD初始化
void LCD_Init(void)
{
    LCD_WR_REG(0x01);
    delay(150);
    LCD_WR_REG(0x11);
    delay(120);

    //------------------------------display and color format setting--------------------------------//
    LCD_WR_REG(0X36); // Memory Access Control
    LCD_WR_DATA8(0x00);

    LCD_WR_REG(0X3A);
    LCD_WR_DATA8(0X55);
    delay(10);

    LCD_WR_REG(0x21);

    //--------------------------------ST7789S Frame rate setting-------------------------
    LCD_WR_REG(0xb2);
    LCD_WR_DATA8(0x0c);
    LCD_WR_DATA8(0x0c);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xb3);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x0f);
    LCD_WR_DATA8(0x0f);

    LCD_WR_REG(0xb7);
    LCD_WR_DATA8(0x35);

    //---------------------------------ST7789S Power setting-----------------------------
    LCD_WR_REG(0xbb);
    LCD_WR_DATA8(0x35);
    LCD_WR_REG(0xc0);
    LCD_WR_DATA8(0x2c);
    LCD_WR_REG(0xc2);
    LCD_WR_DATA8(0x01);
    LCD_WR_REG(0xc3);
    LCD_WR_DATA8(0x13);
    LCD_WR_REG(0xc4);
    LCD_WR_DATA8(0x20);
    LCD_WR_REG(0xc6);
    LCD_WR_DATA8(0x1e);
    LCD_WR_REG(0xca);
    LCD_WR_DATA8(0x0f);
    LCD_WR_REG(0xc8);
    LCD_WR_DATA8(0x08);
    LCD_WR_REG(0x55);
    LCD_WR_DATA8(0x90);
    LCD_WR_REG(0xd0);
    LCD_WR_DATA8(0xa4);
    LCD_WR_DATA8(0xa1);
    //--------------------------------ST7789S gamma setting------------------------------
    LCD_WR_REG(0xe0);
    LCD_WR_DATA8(0xd0);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x06);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x0b);
    LCD_WR_DATA8(0x2a);
    LCD_WR_DATA8(0x3c);
    LCD_WR_DATA8(0x55);
    LCD_WR_DATA8(0x4b);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x16);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x19);
    LCD_WR_DATA8(0x20);
    LCD_WR_REG(0xe1);
    LCD_WR_DATA8(0xd0);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x06);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x0b);
    LCD_WR_DATA8(0x29);
    LCD_WR_DATA8(0x36);
    LCD_WR_DATA8(0x54);
    LCD_WR_DATA8(0x4b);
    LCD_WR_DATA8(0x0d);
    LCD_WR_DATA8(0x16);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x21);
    LCD_WR_DATA8(0x20);

    LCD_WR_REG(0x29);
}

uint16_t LCD_ReadScanLine(void)
{
    while (hspi1.State != HAL_SPI_STATE_READY)
        ; // 等待SPI空闲

    __HAL_SPI_DISABLE(&hspi1);
    hspi1.Instance->CR1 &= ~(SPI_CR1_RXONLY | SPI_CR1_BIDIMODE);
    hspi1.Instance->CR1 |= SPI_CR1_BIDIMODE;
	
    hspi1.Instance->CR1 &= ~SPI_CR1_BR;
    hspi1.Instance->CR1 |= SPI_BAUDRATEPRESCALER_16;
    __HAL_SPI_ENABLE(&hspi1);
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;

    LCD_CS_OUT(0);
    LCD_DC_OUT(0);

    uint8_t cmd = 0x45;
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);

    LCD_DC_OUT(1);

    __HAL_SPI_DISABLE(&hspi1);
    SPI_1LINE_RX(&hspi1);
    __HAL_SPI_ENABLE(&hspi1);

    uint8_t data[3] = {0};
    int i = 0;
    while(i < 2)
    {
        if (hspi1.Instance->SR & SPI_FLAG_RXNE)
            data[i++] = *(__IO uint8_t *)&hspi1.Instance->DR;
    }
    
    __DSB();
    __HAL_SPI_DISABLE(&hspi1);

    while ((hspi1.Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE);
    /* read the received data */
    data[2] = *(__IO uint8_t *)&hspi1.Instance->DR;
    while ((hspi1.Instance->SR & SPI_FLAG_BSY) == SPI_FLAG_BSY);

    LCD_CS_OUT(1);

    __HAL_SPI_DISABLE(&hspi1);
    hspi1.Instance->CR1 &= ~SPI_CR1_BR;
    hspi1.Instance->CR1 |= SPI_BAUDRATEPRESCALER_2;
    SPI_1LINE_TX(&hspi1);
    hspi1.Instance->CR1 &= ~(SPI_CR1_RXONLY | SPI_CR1_BIDIMODE);
    __HAL_SPI_ENABLE(&hspi1);
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;

    return data[1] << 1 | !!data[2];
}

void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    while (hspi1.State != HAL_SPI_STATE_READY)
        ; // 等待SPI空闲

    static uint16_t color1[1];
    uint16_t num;
    color1[0] = color;
    num = (xend - xsta) * (yend - ysta);
    LCD_Address_Set(xsta, ysta, xend - 1, yend - 1); // 设置显示范围
    LCD_CS_OUT(0);

    LCD_DMA_Transfer16Bit((uint8_t *)color1, num*2, DMA_MEMINC_DISABLE); // 启用DMA发送

    // 其余部分见HAL_SPI_TxCpltCallback()函数
}


// 把指定区域的显示缓冲区写入屏幕
void LCD_Color_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *buf)
{
    while (hspi1.State != HAL_SPI_STATE_READY)
        ; // 等待SPI空闲
		
    long num;
    num = (x1 - x0) * (y1 - y0)*2;
    LCD_Address_Set(x0, y0, x1 - 1, y1 - 1);
    LCD_CS_OUT(0);

	if(num > 65535)
	{
		remainsize=num-65535;
		num = 65535;
	}
	else
		remainsize=0;
    LCD_DMA_Transfer16Bit((uint8_t *)buf, num, DMA_MEMINC_ENABLE); // 启用DMA发送

    // 其余部分见HAL_SPI_TxCpltCallback()函数
}
extern uint16_t grambuff[];
// SPI传输完成回调函数
// 此函数会在DMA SPITX传输完成后被调用
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(remainsize>0)
	{
		LCD_DMA_Transfer16Bit((uint8_t *)(grambuff)+65535, remainsize, DMA_MEMINC_ENABLE); // 启用DMA发送
		remainsize=0;
		dbmsg("next:%d",HAL_GetTick());
	}
	else
	{
		dbmsg("done:%d",HAL_GetTick());
		LCD_CS_OUT(1);
	}
}