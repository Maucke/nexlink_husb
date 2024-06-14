#ifndef STM32_RX8900_H_
#define STM32_RX8900_H_

#include <stdlib.h>
#include <stdbool.h>
#include "main.h"

#define RX8900_REG_TIME         0x00
#define RX8900_REG_ALARM1       0x07
#define RX8900_REG_ALARM2       0x0B
#define RX8900_EXT_REG          0x0D
#define RX8900_REG_CONTROL      0x0F
#define RX8900_REG_STATUS       0x0E
#define RX8900_REG_TEMP         0x17

#define RX8900_CON_EOSC         0x80
#define RX8900_CON_BBSQW        0x40
#define RX8900_CON_CONV         0x20
#define RX8900_CON_RS2          0x10
#define RX8900_CON_RS1          0x08
#define RX8900_CON_INTCN        0x04
#define RX8900_CON_A2IE         0x02
#define RX8900_CON_A1IE         0x01

#define RX8900_STA_OSF          0x80
#define RX8900_STA_32KHZ        0x08
#define RX8900_STA_BSY          0x04
#define RX8900_STA_A2F          0x02
#define RX8900_STA_A1F          0x01

typedef enum
{
  ALARM_MODE_ALL_MATCHED = 0,
  ALARM_MODE_HOUR_MIN_SEC_MATCHED,
  ALARM_MODE_MIN_SEC_MATCHED,
  ALARM_MODE_SEC_MATCHED,
  ALARM_MODE_ONCE_PER_SECOND
} AlarmMode;

typedef enum
{
  SUNDAY = 1,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY
} DaysOfWeek;

typedef struct
{
  uint8_t Year;
  uint8_t Month;
  uint8_t Date;
  uint8_t DaysOfWeek;
  uint8_t Hour;
  uint8_t Min;
  uint8_t Sec;
} _RTC;

typedef struct
{
	char Time[10];
	char Timens[10];
	char Hour[10];
	char Min[10];
	char Sec[10];
	char Date[20];
	char Week[10];
	char Datemd[10];
} _RTCStr;

void RX8900_Init(void);
bool RX8900_GetTime(_RTC *rtc);
bool RX8900_SetTime(_RTC *rtc);
bool RX8900_ReadTemperature(float *rtctemp);
bool RX8900_SetAlarm1(AlarmMode mode, uint8_t date, uint8_t hour, uint8_t min, uint8_t sec);
bool RX8900_ClearAlarm1(void);

#endif /* STM32_RX8900_H_ */
