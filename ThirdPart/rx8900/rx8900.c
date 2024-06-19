#include "main.h"
#include "rx8900.h"
#include "stdio.h"
#include "myiic.h"

#define RX8900_ADDR  (0x32)

static uint8_t B2D(uint8_t bcd);
static uint8_t D2B(uint8_t decimal);
_RTC rtc = {
    .Year = 19, .Month = 12, .Date = 12,
    .DaysOfWeek = SUNDAY,
    .Hour = 1, .Min = 2, .Sec = 3
};
void RX8900_Init()
{
	IIC_Init();
  IIC_Write_Byte(RX8900_ADDR, RX8900_EXT_REG, 8);
  IIC_Write_Byte(RX8900_ADDR, RX8900_REG_STATUS, 0);
  IIC_Write_Byte(RX8900_ADDR, RX8900_REG_CONTROL, 64);
	RX8900_SetTime(&rtc);
}

char weektab[][4]={"SUN","MON","TUE","WED","THU","FRI","SAT"};
int RX8900_Get_Week_Day( u8 reg_week_day )
{
	int i, tm_wday = -1;
	
	for ( i=0; i < 7; i++ )
	{
		if ( reg_week_day & 1 )
		{
			tm_wday = i;
			break;
		}
		reg_week_day >>= 1;
	}
	
	return 	tm_wday;
}
bool RX8900_GetTime(_RTC *rtc)
{
  uint8_t buffer[7] = {0,};
	if(IIC_Read_Len(RX8900_ADDR, RX8900_REG_TIME, 7, buffer))
		return false;
		
  rtc->Sec = B2D(buffer[0] & 0x7F);
  rtc->Min = B2D(buffer[1] & 0x7F);
  rtc->Hour = B2D(buffer[2] & 0x3F);
  rtc->DaysOfWeek = RX8900_Get_Week_Day(buffer[3] & 0x7f );
  rtc->Date = B2D(buffer[4] & 0x3F);
  rtc->Month = B2D(buffer[5] & 0x1F);
  rtc->Year = B2D(buffer[6]);
			
  return true;
}

void RX8900_Test(int interval)
{
  static _RTC rtc;
  static long last_update_time;
  long now_tick = HAL_GetTick();
  if(now_tick - last_update_time > interval)
  {
    RX8900_GetTime(&rtc);
    dbmsg("Time: %02d:%02d:%02d", rtc.Hour,rtc.Min,rtc.Sec);
    last_update_time = HAL_GetTick();
  }
}

bool RX8900_SetTime(_RTC *rtc)
{
  uint8_t buffer[8] = {D2B(rtc->Sec), D2B(rtc->Min), D2B(rtc->Hour), (unsigned char)(1 << rtc->DaysOfWeek), D2B(rtc->Date), D2B(rtc->Month), D2B(rtc->Year)};
  
	if(IIC_Write_Len(RX8900_ADDR, RX8900_REG_TIME, 7, buffer))
		return false;
  return true;
}

bool RX8900_ReadTemperature(float *rtctemp)
{
  uint8_t startAddr = RX8900_REG_TEMP;
  uint8_t temp = IIC_Read_Byte(RX8900_ADDR, startAddr);
	
	if(!temp)
		return false;

  *rtctemp = (temp * 2 - 187.19) / 3.218;
	return true;
}

//bool RX8900_SetAlarm1(uint8_t mode, uint8_t date, uint8_t hour, uint8_t min, uint8_t sec)
//{
//  uint8_t alarmSecond = D2B(sec);
//  uint8_t alarmMinute = D2B(min);
//  uint8_t alarmHour = D2B(hour);
//  uint8_t alarmDate = D2B(date);

//  switch(mode)
//  {
//  case ALARM_MODE_ALL_MATCHED:
//    break;
//  case ALARM_MODE_HOUR_MIN_SEC_MATCHED:
//    alarmDate |= 0x80;
//    break;
//  case ALARM_MODE_MIN_SEC_MATCHED:
//    alarmDate |= 0x80;
//    alarmHour |= 0x80;
//    break;
//  case ALARM_MODE_SEC_MATCHED:
//    alarmDate |= 0x80;
//    alarmHour |= 0x80;
//    alarmMinute |= 0x80;
//    break;
//  case ALARM_MODE_ONCE_PER_SECOND:
//    alarmDate |= 0x80;
//    alarmHour |= 0x80;
//    alarmMinute |= 0x80;
//    alarmSecond |= 0x80;
//    break;
//  default:
//    break;
//  }

//  /* Write Alarm Registers */
//  uint8_t startAddr = RX8900_REG_ALARM1;
//  uint8_t buffer[5] = {startAddr, alarmSecond, alarmMinute, alarmHour, alarmDate};
//  if(HAL_I2C_Master_Transmit(i2c, RX8900_ADDR, buffer, sizeof(buffer), 50) != HAL_OK) return false;

//  /* Enable Alarm1 at Control Register */
//  uint8_t ctrlReg = 0x00;
//  IIC_Read_Byte(RX8900_ADDR, RX8900_REG_CONTROL, &ctrlReg);
//  ctrlReg |= RX8900_CON_A1IE;
//  ctrlReg |= RX8900_CON_INTCN;
//  IIC_Write_Byte(RX8900_ADDR, RX8900_REG_CONTROL, ctrlReg);

//  return true;
//}

bool RX8900_ClearAlarm1()
{
  uint8_t ctrlReg;
  uint8_t statusReg;

  /* Clear Control Register */
  ctrlReg = IIC_Read_Byte(RX8900_ADDR, RX8900_REG_CONTROL);
  ctrlReg &= ~RX8900_CON_A1IE;
  IIC_Write_Byte(RX8900_ADDR, RX8900_REG_CONTROL, ctrlReg);

  /* Clear Status Register */
  statusReg = IIC_Read_Byte(RX8900_ADDR, RX8900_REG_STATUS);
  statusReg &= ~RX8900_STA_A1F;
  IIC_Write_Byte(RX8900_ADDR, RX8900_REG_STATUS, statusReg);

  return true;
}

static uint8_t B2D(uint8_t bcd)
{
  return (bcd >> 4) * 10 + (bcd & 0x0F);
}

static uint8_t D2B(uint8_t decimal)
{
  return (((decimal / 10) << 4) | (decimal % 10));
}
