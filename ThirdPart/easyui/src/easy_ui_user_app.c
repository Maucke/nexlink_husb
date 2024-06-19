/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#include "easy_ui_user_app.h"
#include "lv_anim_light.h"
#include "usbd_nex_link.h"
#include "animation.h"
// Pages
EasyUIPage_t pageMain, pageUSBForm, pageAnimation, pageSetting, pageAbout;

// Items
EasyUIItem_t itemUSBForm;
EasyUIItem_t itemAnimation;
EasyUIItem_t itemSetting, itemColor, itemReset, itemBrightness, titleSetting;
EasyUIItem_t itemAbout;
EasyUIItem_t itemMind, itemCircle, itemSnowflake, itemMeteo, itemPlanet, itemTriangle, itemStarwar, itemBlast, itemGCircle, titleAnimation;
bool enMind, enCircle, enSnowflake, enMeteo, enPlanet, enTriangle, enStarwar, enGCircle;
extern lv_anim_t anim_backlight;
extern nex_usb_des des;
float setting_brightness;
__IO bool usbinhibit = true;

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

}

void EventChangeBrightness(EasyUIItem_t* item)
{
  if(opnUp)
  {
    if(*item->param + 10 <= 100)
      *item->param += 10;
    else
      *item->param = 100;
    lv_anim_start(&anim_backlight, *item->param == 100 ? 990 : (*item->param) * 10, 100);
    opnUp = opnForward = false;
  }
  if(opnDown)
  {
    if(*item->param - 10 >= 10)
      *item->param -= 10;
    else
      *item->param = 10;
    lv_anim_start(&anim_backlight, *item->param == 100 ? 990 : (*item->param) * 10, 100);
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
    lv_anim_start(&anim_backlight, *item->param == 100 ? 990 : (*item->param) * 10, 100);
    EasyUIBackgroundBlur();
    functionIsRunning = false;
    opnExit = false;
  }
  // Clear the states of key to monitor next key action
  opnForward = opnBackward = opnEnter = opnUp = opnDown = false;
}

void PageAbout(EasyUIItem_t* page)
{
  static uint8_t time = 0;
  static float x = SCREEN_WIDTH;
  static float step = (float)(SCREEN_WIDTH - 115) / 5;

  // Display about info
  EasyUIClearBuffer();
  EasyUIDisplayStr(3, 4, "SCEP");
  EasyUISetDrawColor(XOR);
  EasyUIDrawRBox(1, 1, 4 * FONT_WIDTH + 5, ITEM_HEIGHT, NV3030B_penColor, 1);
  EasyUISetDrawColor(NORMAL);
  EasyUIDrawBox(2, 16, 2, ITEM_HEIGHT * 5, NV3030B_penColor);
  EasyUIDisplayStr(36, 4, "v1.2");
  EasyUIDisplayStr(8, 18, "MCU    : CH32V3");
  EasyUIDisplayStr(8, 30, "EasyUI : ");
  EasyUIDisplayStr(8 + 9 * FONT_WIDTH, 30, EasyUIVersion);
  EasyUIDisplayStr(8, 42, "Flash  : 256KB");
  EasyUIDisplayStr(8, 54, "UID    : ");
  EasyUIDisplayStr(8, 66, ">> Powered by: ErBW_s");

  // Get uid
  static uint32_t* addrBase = (uint32_t*) 0x1FFFF7E8;
  uint64_t uid;
  memcpy(&uid, addrBase, 8);
  char str[13];
  uint64_t uidBackup = uid;
  const char hex_index[16] =
  {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'
  };
  int8_t data_temp[16];
  uint8_t bit = 0, i = 0;
  while(bit < 16)
  {
    data_temp[bit++] = (uidBackup & 0xF);
    uidBackup >>= 4;
  }
  for(bit = 12; bit > 0; bit--)
  {
    str[i++] = hex_index[data_temp[bit - 1]];
  }
  str[i] = '\0';
  EasyUIDisplayStr(8 + 9 * FONT_WIDTH, 54, str);

  // Display profile photo
  if(time < 5)
  {
    x -= step;
    time++;
  }
  else
    x = 115;
  EasyUIDisplayBMP((int16_t) x, (SCREEN_HEIGHT - 56) / 2, 29, 28, ErBW_s_2928);
  if(opnExit)
  {
    time = 0;
    x = SCREEN_WIDTH;
  }
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

void PageAnimation(EasyUIItem_t* page)
{
  EasyUIClearBuffer();

  if(opnExit)
  {
  }
}

void MenuInit()
{
  setting_brightness = (des.brides.brightness + 1) / 10;
  EasyUIAddPage(&pageMain, PAGE_LIST);
  EasyUIAddPage(&pageSetting, PAGE_LIST);
  EasyUIAddPage(&pageUSBForm, PAGE_CUSTOM, PageUSBForm);
  EasyUIAddPage(&pageAnimation, PAGE_LIST);
  EasyUIAddPage(&pageAbout, PAGE_CUSTOM, PageAbout);

  EasyUIAddItem(&pageMain, &itemUSBForm, "USBForm", ITEM_JUMP_PAGE, pageUSBForm.id);
  EasyUIAddItem(&pageMain, &itemAnimation, "Animation", ITEM_JUMP_PAGE, pageAnimation.id);
  EasyUIAddItem(&pageMain, &itemSetting, "Setting", ITEM_JUMP_PAGE, pageSetting.id);
  EasyUIAddItem(&pageMain, &itemAbout, "<About>", ITEM_JUMP_PAGE, pageAbout.id);

  EasyUIAddItem(&pageSetting, &titleSetting, "[Setting]", ITEM_PAGE_DESCRIPTION);
  EasyUIAddItem(&pageSetting, &itemColor, "Reversed", ITEM_SWITCH, &reversedColor);
  EasyUIAddItem(&pageSetting, &itemBrightness, "Brightness", ITEM_PROGRESS_BAR, &setting_brightness, EventChangeBrightness);

//  EasyUIItem_t itemMind, itemCircle, itemSnowflake, itemMeteo, itemPlanet, itemTriangle, itemStarwar, itemBlast, itemGCircle, titleAnimation;
  EasyUIAddItem(&pageAnimation, &titleAnimation, "[Animation]", ITEM_PAGE_DESCRIPTION);
  EasyUIAddItem(&pageAnimation, &itemMind, "Mind", ITEM_RADIO_BUTTON, &enMind);
  EasyUIAddItem(&pageAnimation, &itemCircle, "Circle", ITEM_RADIO_BUTTON, &enCircle);
  EasyUIAddItem(&pageAnimation, &itemSnowflake, "Snowflake", ITEM_RADIO_BUTTON, &enSnowflake);
  EasyUIAddItem(&pageAnimation, &itemMeteo, "Meteo", ITEM_RADIO_BUTTON, &enMeteo);
  EasyUIAddItem(&pageAnimation, &itemPlanet, "Planet", ITEM_RADIO_BUTTON, &enPlanet);
  EasyUIAddItem(&pageAnimation, &itemTriangle, "Triangle", ITEM_RADIO_BUTTON, &enTriangle);
  EasyUIAddItem(&pageAnimation, &itemStarwar, "Starwar", ITEM_RADIO_BUTTON, &enStarwar);
  EasyUIAddItem(&pageAnimation, &itemGCircle, "GCircle", ITEM_RADIO_BUTTON, &enGCircle);
  dbmsg("setting_brightness: %f", setting_brightness);
//
//		EasyUIItemOperationResponse(&pageAnimation, &itemAnimation, &itemAnimation.id);
  Motion_Init();
}
