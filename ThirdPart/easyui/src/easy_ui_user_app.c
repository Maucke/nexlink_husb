/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#include "easy_ui_user_app.h"
#include "usbd_nex_link.h"
#include "lv_anim_light.h"
#include "usbd_nex_link.h"
#include "zf_common_font.h"
#include "animation.h"
#include "adc.h"
#include "rx8900.h"
#include "beep.h"
#include "bmp280.h"
#include <time.h>
// Pages
EasyUIPage_t pageMain, pageUSBForm, pageDialog, pageSensor, pageAnimation, pageSetting, pageAbout;

// Items
EasyUIItem_t itemUSBForm;
EasyUIItem_t itemDebug;
EasyUIItem_t itemSensor;
EasyUIItem_t itemAnimation;
EasyUIItem_t itemSetting, itemColor, itemReset, itemBrightness, titleSetting;
EasyUIItem_t itemAbout;
EasyUIItem_t itemMind, itemCircle, itemSnowflake, itemMeteo, itemPlanet, itemTriangle, itemStarwar, itemBlast, itemGCircle, itemFirework, titleAnimation;

bool enMind, enCircle, enSnowflake, enMeteo, enPlanet, enTriangle, enStarwar, enGCircle, enFirework;
extern lv_anim_t anim_backlight;
extern nex_usb_des des;
float setting_brightness;
__IO bool usbinhibit = true;
__IO bool jump2winform = false;

EasyKey_t keyUp, keyDown;
extern __IO bool menuisvisible;
extern lv_anim_t anim_beep;
/*!
 * @brief   Sync the operation bool value
 *
 * @param   void
 * @return  void
 */
void EasyUIKeyActionMonitor() //Interrupt trigger, No HAL_Delay(xx)
{
		extern bool mpu_left, mpu_right, mpu_ok, mpu_quit;
    if(keyUp.isPressed)
		{
			lv_anim_start(&anim_beep, 1000, 200);
			dbusbmsg("keyUp:isPressed");
			if(des.brides.brightness == 0)
			{
				lv_anim_start(&anim_backlight, des.brides.brightness?des.brides.brightness:300, 2000);
				return;
			}
			
			menuisvisible = !menuisvisible;
			if(menuisvisible)
				mpu_left=mpu_right= mpu_ok=mpu_quit=0;
		}
    else if(keyUp.isHold)
		{
			lv_anim_start(&anim_beep, 2000, 500);
			dbusbmsg("keyUp:holdTime:%d", keyUp.holdTime);
			if(menuisvisible)
				jump2winform = true;
		}
		if(keyDown.isHold)
		{
			lv_anim_start(&anim_beep, 2000, 500);
			dbusbmsg("keyDown:holdTime:%d", keyDown.holdTime);
			lv_anim_start(&anim_backlight, 0, 1000);
		}
		
		if(!menuisvisible)
        return;
    if (opnForward || opnBackward || opnEnter || opnExit || opnUp || opnDown)
        return;
		
		opnEnter = mpu_ok;
		opnExit = mpu_quit;
		opnUp = mpu_right;
		opnDown = mpu_left;
		opnForward = mpu_left;
		opnBackward = mpu_right;
		mpu_left=mpu_right= mpu_ok=mpu_quit=0;
		if(opnEnter!=0)
		dbusbmsg("opnEnter:%d",opnEnter);
		if(opnExit!=0)
		dbusbmsg("opnExit:%d",opnExit);
		if(opnUp!=0)
		dbusbmsg("opnUp:%d",opnUp);
		if(opnDown!=0)
		dbusbmsg("opnDown:%d",opnDown);

#if ROTARY == 1
#endif
}

#define DIALOGITEMSIZE 40

EasyUIItem_t itemDialog[DIALOGITEMSIZE];
char* itemDialogStr[DIALOGITEMSIZE];
int itemDialogIndex = 0;
int itemDialogCount = 0;

extern uint8_t debug_buf[DEBUG_BUF_SIZE];
int dbug_printf(const char* pcFormat, ...)
{
  va_list args;
  int len = 0;
  memset(debug_buf, 0, sizeof debug_buf);
  va_start(args, pcFormat);

  len = vsnprintf((char*)debug_buf, sizeof(debug_buf), pcFormat, args);
	
	free(itemDialogStr[itemDialogIndex]);
	itemDialogStr[itemDialogIndex] = (char*)malloc(len+1);
	memcpy(itemDialogStr[itemDialogIndex], debug_buf, len);
	itemDialogStr[itemDialogIndex][len] = 0;
	if(itemDialogCount<DIALOGITEMSIZE)
		itemDialog[itemDialogIndex].title = itemDialogStr[itemDialogIndex];
	else
	{
		for(int i= 0;i<DIALOGITEMSIZE-1;i++)
			itemDialog[i].title = itemDialog[i+1].title;
		itemDialog[DIALOGITEMSIZE-1].title = itemDialogStr[itemDialogIndex];
	}
	
	itemDialogIndex = (itemDialogIndex+1)%DIALOGITEMSIZE;
	itemDialogCount++;
  va_end(args);

  return len;
}

void EventJump()
{
		if(jump2winform)
		{
			jump2winform = false;
			EasyUIItemOperationResponse(&pageUSBForm, &itemUSBForm, &itemUSBForm.id); 
		}
}

void EventMotion()
{
  if(enMind)
    Motion_Mind();
  if(enCircle)
    Motion_Circle();
  if(enSnowflake)
    Motion_Snowflake();
  if(enMeteo)
    Motion_Movmeteor();
  if(enPlanet)
    Motion_Planet();
  if(enTriangle)
    Motion_Triangle();
  if(enStarwar)
    Motion_StarWar();
  if(enGCircle)
    Motion_GCFireworks();
  if(enFirework)
    Motion_Firework();
}

void EventChangeBrightness(EasyUIItem_t* item)
{
  if(opnUp)
  {
    if(*item->param + 10 <= 100)
      *item->param += 10;
    else
      *item->param = 100;
		des.brides.brightness =  *item->param == 100 ? 999 : (*item->param) * 10;
    lv_anim_start(&anim_backlight,des.brides.brightness, 100);
    opnUp = opnForward = false;
  }
  if(opnDown)
  {
    if(*item->param - 10 >= 10)
      *item->param -= 10;
    else
      *item->param = 10;
		des.brides.brightness =  *item->param == 100 ? 999 : (*item->param) * 10;
    lv_anim_start(&anim_backlight,des.brides.brightness, 100);
    opnDown = opnBackward = false;
  }

  if(opnEnter)
  {
    item->paramBackup = *item->param;
    EasyUIBackgroundBlur();
    functionIsRunning = false;
    opnEnter = false;
  }
  if(opnExit)
  {
    *item->param = item->paramBackup;
		des.brides.brightness =  *item->param == 100 ? 999 : (*item->param) * 10;
    lv_anim_start(&anim_backlight,des.brides.brightness, 100);
    EasyUIBackgroundBlur();
    functionIsRunning = false;
    opnExit = false;
  }
  // Clear the states of key to monitor next key action
  opnForward = opnBackward = opnEnter = opnUp = opnDown = false;
}

void PageAbout(EasyUIItem_t* page)
{
	char tempstr[128];
	int screen_delta = 10;

  EasyUIDisplayStr(10, screen_delta, "MCU: STM32F405");
	screen_delta += ITEM_HEIGHT;
  EasyUIDisplayStr(10, screen_delta, "MPU: MPU6050");
	screen_delta += ITEM_HEIGHT;
  EasyUIDisplayStr(10, screen_delta, "CLOCK: RX8900");
	screen_delta += ITEM_HEIGHT;
  EasyUIDisplayStr(10, screen_delta, "I2S: INMP441");
	screen_delta += ITEM_HEIGHT;
  EasyUIDisplayStr(10, screen_delta, "Author: DPJ");
	screen_delta += ITEM_HEIGHT;
  EasyUIDisplayStr(10, screen_delta, EasyUIVersion);
	screen_delta += ITEM_HEIGHT;
	snprintf(tempstr, sizeof tempstr, "Rel. %s", __DATE__);
  EasyUIDisplayStr(10, screen_delta, tempstr);
	screen_delta += ITEM_HEIGHT;
}

void PageUSBForm(EasyUIItem_t* page)
{
	if(usbinhibit)
	{
		EasyUITransitionAnim();
    EasyUIClearBuffer();
    EasyUISendBuffer();
		usbinhibit = false;
	}
  if(opnExit)
  {
    usbinhibit = true;
  }
}

void lowBatteryAction()
{
	float battery = Get_ADC_Value(&hadc1) * 3.28f * 2.0f / 4096.0f;
	if(battery<3.2f)
    lv_anim_start(&anim_backlight,0 , 100);
}

int batteryVoltageToPercentage(float voltage) {
    float minVoltage = 3.0f;
    float maxVoltage = 4.2f;
    
    // 计算电压在范围内的百分比
    if (voltage < minVoltage) {
        return 0; // 如果电压低于最小值，返回0%
    } else if (voltage > maxVoltage) {
        return 100; // 如果电压高于最大值，返回100%
    } else {
        // 在最小值和最大值之间进行线性插值计算
        float percentage = (voltage - minVoltage) / (maxVoltage - minVoltage) * 100.0f;
        return (int)percentage;
    }
}

int batteryVoltageToLevel(float voltage) {
    float minVoltage = 3.0f;
    float maxVoltage = 4.2f;
    
    // 计算电压在范围内的百分比
    float percentage = (voltage - minVoltage) / (maxVoltage - minVoltage) * 100.0f;
    
    // 将百分比映射到-1到4的级别
    if (percentage < 20.0f) {
        return -1;
    } else if (percentage < 40.0f) {
        return 0;
    } else if (percentage < 60.0f) {
        return 1;
    } else if (percentage < 75.0f) {
        return 2;
    } else if (percentage <= 90.0f) { // 考虑到小数精度可能性，这里增加一个等于的情况
        return 3;
    } else {
        return 4; 
    }
}

typedef struct {
    float current_value;  // 当前值
    float target_value;   // 目标值
} Filter;

// 更新滤波器，根据变化的幅度选择步进大小
void update_filter(Filter *filter) {
    float diff = fabs(filter->target_value - filter->current_value);
    
    if (diff > 20.0f) {
        filter->current_value = filter->target_value;  // 立即变化到新值
    } else if (diff > 10.0f) {
        if (filter->target_value > filter->current_value)
            filter->current_value += 2.0f;
        else
            filter->current_value -= 2.0f;
    } else if (diff > 5.0f) {
        if (filter->target_value > filter->current_value)
            filter->current_value += 1.0f;
        else
            filter->current_value -= 1.0f;
    } else if (diff > 2.0f) {
        if (filter->target_value > filter->current_value)
            filter->current_value += 0.5f;
        else
            filter->current_value -= 0.5f;
    } else if (diff > 0.5f) {
        if (filter->target_value > filter->current_value)
            filter->current_value += 0.1f;
        else
            filter->current_value -= 0.1f;
    } else {
        filter->current_value = filter->target_value;  // 立即变化到新值
    }
}

Filter fltaltitude;
Filter fltvoltagex10;

void PageSensor(EasyUIItem_t* page)
{
	static int levelrun = 0;
	char tempstr[128];
	int screen_delta = 10;
	static struct tm time_user;
  static float pressure, temperature, humidity, asl;
  static long last_update_time = 0;
  long now_tick = HAL_GetTick();
  if(now_tick - last_update_time > 500)
  {
		fltvoltagex10.target_value = Get_ADC_Value(&hadc1) * 3.28f * 2.0f / 40.960f;
		update_filter(&fltvoltagex10);
		
		if(HAL_GPIO_ReadPin(PW_CHARGE_GPIO_Port,PW_CHARGE_Pin)==GPIO_PIN_RESET)
			levelrun = (levelrun+1)%4;
		else if(batteryVoltageToLevel(fltvoltagex10.current_value/100.0f)!=-1)
			levelrun = batteryVoltageToLevel(fltvoltagex10.current_value/100.0f);
		else
			levelrun = levelrun?0:1;
    RX8900_GetTime(&time_user);
    BMP280_GetData(&pressure, &temperature, &humidity, &asl);
		fltaltitude.target_value = BMP280_PressureToAltitude(&pressure);
		update_filter(&fltaltitude);
		
		last_update_time = HAL_GetTick();
  }
	
	
	snprintf(tempstr, sizeof tempstr, "BAT: %.1f V", fltvoltagex10.current_value/100.0f);
	EasyUIDisplayStr(10, screen_delta, tempstr);
	screen_delta += ITEM_HEIGHT;
	if(HAL_GPIO_ReadPin(PW_CHARGE_GPIO_Port,PW_CHARGE_Pin)==GPIO_PIN_RESET)
		snprintf(tempstr, sizeof tempstr,"CHARGING");
	else
		snprintf(tempstr, sizeof tempstr,"DISCHARGE");
	EasyUIDisplayStr(10, screen_delta, tempstr);
	screen_delta += ITEM_HEIGHT;
	NV3030B_DrawBMP565(165, screen_delta, 36, 36, gImage_Battery[levelrun]);

	snprintf(tempstr, sizeof tempstr, "%04d-%02d-%02d, %s", time_user.tm_year+1900,time_user.tm_mon+1,time_user.tm_mday,weekdays[time_user.tm_wday%7]);
	EasyUIDisplayStr(10, screen_delta, tempstr);
	screen_delta += ITEM_HEIGHT;
	snprintf(tempstr, sizeof tempstr, "%02d:%02d:%02d", time_user.tm_hour%100, time_user.tm_min%100, time_user.tm_sec%100);
	EasyUIDisplayStr(10, screen_delta, tempstr);
	screen_delta += ITEM_HEIGHT;
	
	snprintf(tempstr, sizeof tempstr, "P: %.1f Pa", pressure);
	EasyUIDisplayStr(10, screen_delta, tempstr);
	screen_delta += ITEM_HEIGHT;
	snprintf(tempstr, sizeof tempstr, "T: %.1f C", temperature);
	EasyUIDisplayStr(10, screen_delta, tempstr);
	screen_delta += ITEM_HEIGHT;
	snprintf(tempstr, sizeof tempstr, "Alt: %.1f M", fltaltitude.current_value);
	EasyUIDisplayStr(10, screen_delta, tempstr);
	screen_delta += ITEM_HEIGHT;
}
extern USBD_HandleTypeDef hUSB;extern bool usbavaliable;

void PageDialog(EasyUIItem_t* page)
{
	
}

void MenuInit()
{
  setting_brightness = (des.brides.brightness + 1) / 10;
	enFirework = true;
	enStarwar = true;
  EasyUIAddPage(&pageMain, PAGE_LIST);
  EasyUIAddPage(&pageSetting, PAGE_LIST);
  EasyUIAddPage(&pageUSBForm, PAGE_CUSTOM, PageUSBForm);
  EasyUIAddPage(&pageDialog, PAGE_LIST);
  EasyUIAddPage(&pageSensor, PAGE_CUSTOM, PageSensor);
  EasyUIAddPage(&pageAnimation, PAGE_LIST);
  EasyUIAddPage(&pageAbout, PAGE_CUSTOM, PageAbout);

  EasyUIAddItem(&pageMain, &itemUSBForm, "USBForm", ITEM_JUMP_PAGE, pageUSBForm.id);
  EasyUIAddItem(&pageMain, &itemDebug, "Debug", ITEM_JUMP_PAGE, pageDialog.id);
  EasyUIAddItem(&pageMain, &itemSensor, "Sensor", ITEM_JUMP_PAGE, pageSensor.id);
  EasyUIAddItem(&pageMain, &itemAnimation, "Animation", ITEM_JUMP_PAGE, pageAnimation.id);
  EasyUIAddItem(&pageMain, &itemSetting, "Setting", ITEM_JUMP_PAGE, pageSetting.id);
  EasyUIAddItem(&pageMain, &itemAbout, "<About>", ITEM_JUMP_PAGE, pageAbout.id);

  EasyUIAddItem(&pageSetting, &titleSetting, "[Setting]", ITEM_PAGE_DESCRIPTION);
  EasyUIAddItem(&pageSetting, &itemColor, "Reversed", ITEM_SWITCH, &reversedColor);
  EasyUIAddItem(&pageSetting, &itemBrightness, "Brightness", ITEM_PROGRESS_BAR, &setting_brightness, EventChangeBrightness);

  EasyUIAddItem(&pageAnimation, &titleAnimation, "[Animation]", ITEM_PAGE_DESCRIPTION);
  EasyUIAddItem(&pageAnimation, &itemMind, "Mind", ITEM_CHECKBOX, &enMind);
  EasyUIAddItem(&pageAnimation, &itemCircle, "Circle", ITEM_CHECKBOX, &enCircle);
  EasyUIAddItem(&pageAnimation, &itemSnowflake, "Snowflake", ITEM_CHECKBOX, &enSnowflake);
  EasyUIAddItem(&pageAnimation, &itemMeteo, "Meteo", ITEM_CHECKBOX, &enMeteo);
  EasyUIAddItem(&pageAnimation, &itemPlanet, "Planet", ITEM_CHECKBOX, &enPlanet);
  EasyUIAddItem(&pageAnimation, &itemTriangle, "Triangle", ITEM_CHECKBOX, &enTriangle);
  EasyUIAddItem(&pageAnimation, &itemStarwar, "Starwar", ITEM_CHECKBOX, &enStarwar);
  EasyUIAddItem(&pageAnimation, &itemGCircle, "GCircle", ITEM_CHECKBOX, &enGCircle);
  EasyUIAddItem(&pageAnimation, &itemFirework, "Firework", ITEM_CHECKBOX, &enFirework);
  dbusbmsg("setting_brightness: %f", setting_brightness);


	for(int itemDialogIndex=0;itemDialogIndex<DIALOGITEMSIZE;itemDialogIndex++)
	{
		itemDialogStr[itemDialogIndex] = (char*)malloc(1);
		itemDialogStr[itemDialogIndex][0] = 0;
		EasyUIAddItem(&pageDialog, &itemDialog[itemDialogIndex], "", ITEM_DETAIL);
		itemDialog[itemDialogIndex].title = itemDialogStr[itemDialogIndex];
	}
//		EasyUIItemOperationResponse(&pageAnimation, &itemAnimation, &itemAnimation.id);
  Motion_Init();
	// Key init
	EasyKeyInit(&keyUp, GPIOB, GPIO_PIN_6);
	EasyKeyInit(&keyDown, GPIOB, GPIO_PIN_7);
}
