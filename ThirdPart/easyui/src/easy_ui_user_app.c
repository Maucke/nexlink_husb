/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#include "easy_ui_user_app.h"
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
EasyUIPage_t pageMain, pageUSBForm, pageFFT, pageSensor, pageAnimation, pageSetting, pageAbout;

// Items
EasyUIItem_t itemUSBForm;
EasyUIItem_t itemFFT;
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
void EasyUIKeyActionMonitor()
{
		extern bool mpu_left, mpu_right, mpu_ok, mpu_quit;
    if(keyUp.isPressed)
		{
			lv_anim_start(&anim_beep, 1000, 200);
			dbusbmsg("keyUp:isPressed");
			if(des.brides.brightness == 0)
			{
				lv_anim_start(&anim_backlight, des.brides.brightness, 2000);
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

void PageSensor(EasyUIItem_t* page)
{
	static int levelrun = 0;
	char tempstr[128];
	int screen_delta = 10;
	static float battery;
	static struct tm time_user;
  static float pressure, temperature, humidity, asl;
  static long last_update_time = 0;
  long now_tick = HAL_GetTick();
  if(now_tick - last_update_time > 500)
  {
		if(HAL_GPIO_ReadPin(PW_CHARGE_GPIO_Port,PW_CHARGE_Pin)==GPIO_PIN_RESET)
			levelrun = (levelrun+1)%4;
		else if(batteryVoltageToLevel(battery)!=-1)
			levelrun = batteryVoltageToLevel(battery);
		else
			levelrun = levelrun?0:1;
		battery = Get_ADC_Value(&hadc1) * 3.28f * 2.0f / 4096.0f;
    RX8900_GetTime(&time_user);
    BMP280_GetData(&pressure, &temperature, &humidity, &asl);
		last_update_time = HAL_GetTick();
  }
	
	
	snprintf(tempstr, sizeof tempstr, "BAT: %.1f V", battery);
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
	snprintf(tempstr, sizeof tempstr, "Alt: %.1f M", BMP280_PressureToAltitude(&pressure));
	EasyUIDisplayStr(10, screen_delta, tempstr);
	screen_delta += ITEM_HEIGHT;
}
void PageFFT(EasyUIItem_t* page)
{
	extern int32_t fft_input_buffer[];
	extern uint32_t adc_buffer[];
	for(int i=0;i<LCD_W;i++)
		EasyUIDrawDot(i,LCD_H/2+adc_buffer[i],0xFF00);
}

void MenuInit()
{
  setting_brightness = (des.brides.brightness + 1) / 10;
	enFirework = true;
	enStarwar = true;
  EasyUIAddPage(&pageMain, PAGE_LIST);
  EasyUIAddPage(&pageSetting, PAGE_LIST);
  EasyUIAddPage(&pageUSBForm, PAGE_CUSTOM, PageUSBForm);
  EasyUIAddPage(&pageFFT, PAGE_CUSTOM, PageFFT);
  EasyUIAddPage(&pageSensor, PAGE_CUSTOM, PageSensor);
  EasyUIAddPage(&pageAnimation, PAGE_LIST);
  EasyUIAddPage(&pageAbout, PAGE_CUSTOM, PageAbout);

  EasyUIAddItem(&pageMain, &itemUSBForm, "USBForm", ITEM_JUMP_PAGE, pageUSBForm.id);
  EasyUIAddItem(&pageMain, &itemFFT, "FFT", ITEM_JUMP_PAGE, pageFFT.id);
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

//		EasyUIItemOperationResponse(&pageAnimation, &itemAnimation, &itemAnimation.id);
  Motion_Init();
	// Key init
	EasyKeyInit(&keyUp, GPIOB, GPIO_PIN_6);
	EasyKeyInit(&keyDown, GPIOB, GPIO_PIN_7);
}
