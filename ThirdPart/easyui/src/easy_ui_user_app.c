/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#include "easy_ui_user_app.h"
#include "lv_anim_light.h"
#include "usbd_nex_link.h"
//// Pages
//EasyUIPage_t pageMain, pageCheckbox, pageRadButton, pageSwitch, pageChgVal;

//// Items
//EasyUIItem_t itemSave, itemReset;
//EasyUIItem_t titleMain, itemChkBox, itemRadButton, itemSw, itemVal;
//EasyUIItem_t titleCheckbox, itemCh1, itemCh2, itemCh3, itemCh4;
//EasyUIItem_t titleRadButton, itemButton1, itemButton2, itemButton3, itemButton4;
//EasyUIItem_t itemButton5, itemButton6, itemButton7, itemButton8, itemButton9;
//EasyUIItem_t titleSw, itemColor, itemSw2, itemSw3;
//EasyUIItem_t titleVal, itemFloat, itemInt, itemUint;

//bool ch1 = true, ch2 = true, ch3 = false, ch4 = true;
//bool rb1 = false, rb2 = true, rb3 = false, rb4 = false, rb5 = false;
//bool rb6 = false, rb7 = false, rb8 = false, rb9 = false;
//bool sw2 = false, sw3 = true;
//double testFloat = 0.68, testInt = 5, testUint = 34;

//void MenuInitTest()
//{
//    EasyUIAddPage(&pageMain, PAGE_LIST);
//    EasyUIAddPage(&pageCheckbox, PAGE_LIST);
//    EasyUIAddPage(&pageRadButton, PAGE_LIST);
//    EasyUIAddPage(&pageSwitch, PAGE_LIST);
//    EasyUIAddPage(&pageChgVal, PAGE_LIST);

//    EasyUIAddItem(&pageMain, &titleMain, "[Main]", ITEM_PAGE_DESCRIPTION);
//    EasyUIAddItem(&pageMain, &itemChkBox, "Test checkbox", ITEM_JUMP_PAGE, pageCheckbox.id);
//    EasyUIAddItem(&pageMain, &itemRadButton, "Test radio button", ITEM_JUMP_PAGE, pageRadButton.id);
//    EasyUIAddItem(&pageMain, &itemSw, "Test switches", ITEM_JUMP_PAGE, pageSwitch.id);
//    EasyUIAddItem(&pageMain, &itemVal, "Test change value", ITEM_JUMP_PAGE, pageChgVal.id);
//    EasyUIAddItem(&pageMain, &itemSave, "Save settings", ITEM_MESSAGE, "Saving...", EasyUIEventSaveSettings);
//    EasyUIAddItem(&pageMain, &itemReset, "Reset settings", ITEM_MESSAGE, "Resetting...", EasyUIEventResetSettings);

//    EasyUIAddItem(&pageCheckbox, &titleCheckbox, "[Multiple selection]", ITEM_PAGE_DESCRIPTION);
//    EasyUIAddItem(&pageCheckbox, &itemCh1, "Checkbox 1", ITEM_CHECKBOX, &ch1);
//    EasyUIAddItem(&pageCheckbox, &itemCh2, "Checkbox 2", ITEM_CHECKBOX, &ch2);
//    EasyUIAddItem(&pageCheckbox, &itemCh3, "Checkbox 3", ITEM_CHECKBOX, &ch3);
//    EasyUIAddItem(&pageCheckbox, &itemCh4, "Checkbox 4", ITEM_CHECKBOX, &ch4);

//    EasyUIAddItem(&pageRadButton, &titleRadButton, "[Single selection]", ITEM_PAGE_DESCRIPTION);
//    EasyUIAddItem(&pageRadButton, &itemButton1, "Radio button 1", ITEM_RADIO_BUTTON, &rb1);
//    EasyUIAddItem(&pageRadButton, &itemButton2, "Radio button 2", ITEM_RADIO_BUTTON, &rb2);
//    EasyUIAddItem(&pageRadButton, &itemButton3, "Radio button 3", ITEM_RADIO_BUTTON, &rb3);
//    EasyUIAddItem(&pageRadButton, &itemButton4, "Radio button 4", ITEM_RADIO_BUTTON, &rb4);
//    EasyUIAddItem(&pageRadButton, &itemButton5, "Radio button 5", ITEM_RADIO_BUTTON, &rb5);
//    EasyUIAddItem(&pageRadButton, &itemButton6, "Radio button 6", ITEM_RADIO_BUTTON, &rb6);
//    EasyUIAddItem(&pageRadButton, &itemButton7, "Radio button 7", ITEM_RADIO_BUTTON, &rb7);
//    EasyUIAddItem(&pageRadButton, &itemButton8, "Radio button 8", ITEM_RADIO_BUTTON, &rb8);
//    EasyUIAddItem(&pageRadButton, &itemButton9, "Radio button 9", ITEM_RADIO_BUTTON, &rb9);

//    EasyUIAddItem(&pageSwitch, &titleSw, "[Switch]", ITEM_PAGE_DESCRIPTION);
//    EasyUIAddItem(&pageSwitch, &itemColor, "Reversed color", ITEM_SWITCH, &reversedColor);
//    EasyUIAddItem(&pageSwitch, &itemSw2, "Switch 2", ITEM_SWITCH, &sw2);
//    EasyUIAddItem(&pageSwitch, &itemSw3, "Switch 3", ITEM_SWITCH, &sw3);

//    EasyUIAddItem(&pageChgVal, &titleVal, "[Change value]", ITEM_PAGE_DESCRIPTION);
//    EasyUIAddItem(&pageChgVal, &itemFloat, "Test float", ITEM_CHANGE_VALUE, &testFloat, EasyUIEventChangeFloat);
//    EasyUIAddItem(&pageChgVal, &itemInt, "Test int", ITEM_CHANGE_VALUE, &testInt, EasyUIEventChangeInt);
//    EasyUIAddItem(&pageChgVal, &itemUint, "Test uint", ITEM_CHANGE_VALUE, &testUint, EasyUIEventChangeUint);

//}

// Pages
EasyUIPage_t pageMain, pageUSBForm, pageSetting;

// Items
EasyUIItem_t itemUSBForm, itemColor, itemReset, titleUSBForm;
EasyUIItem_t itemSetting, itemColor, itemReset, itemBrightness, titleSetting;

extern lv_anim_t a;
extern nex_usb_des des;
void EventChangeBrightness(EasyUIItem_t *item)
{
		uint16_t last_brightness;
    if (opnUp)
    {
        if (*item->param + 10 <= 100)
            *item->param += 10;
        else
            *item->param = 100;
				last_brightness = des.brides.brightness;
				a = lv_anim_start(*item->param==100?990:(*item->param)*10,last_brightness,set_brightness_value,100);
				des.brides.brightness = *item->param==100?990:(*item->param)*10;
        opnUp = opnForward = false;
    }
    if (opnDown)
    {
        if (*item->param - 10 >= 10)
            *item->param -= 10;
        else
            *item->param = 10;
				last_brightness = des.brides.brightness;
				a = lv_anim_start(*item->param==100?990:(*item->param)*10,last_brightness,set_brightness_value,100);
				des.brides.brightness = *item->param==100?990:(*item->param)*10;
        opnDown = opnBackward = false;
    }

    if (opnEnter)
    {
        item->paramBackup = *item->param;
        EasyUIBackgroundBlur();
        functionIsRunning = false;
        opnEnter = false;
    }
    if (opnExit)
    {
        *item->param = item->paramBackup;
				last_brightness = des.brides.brightness;
				a = lv_anim_start(*item->param==100?990:(*item->param)*10,last_brightness,set_brightness_value,100);
				des.brides.brightness = *item->param==100?990:(*item->param)*10;
        EasyUIBackgroundBlur();
        functionIsRunning = false;
        opnExit = false;
    }
		// Clear the states of key to monitor next key action
		opnForward = opnBackward = opnEnter = opnUp = opnDown = false;
}

float setting_brightness;

void MenuInit()
{
		EasyUIAddPage(&pageMain, PAGE_LIST);
    EasyUIAddPage(&pageSetting, PAGE_LIST);
    EasyUIAddPage(&pageUSBForm, PAGE_LIST);

		EasyUIAddItem(&pageMain, &itemUSBForm, "USBForm", ITEM_JUMP_PAGE, pageUSBForm.id);
		EasyUIAddItem(&pageMain, &itemSetting, "Setting", ITEM_JUMP_PAGE, pageSetting.id);
	
    EasyUIAddItem(&pageUSBForm, &titleUSBForm, "[USBForm]", ITEM_PAGE_DESCRIPTION);

    EasyUIAddItem(&pageSetting, &titleSetting, "[Setting]", ITEM_PAGE_DESCRIPTION);
    EasyUIAddItem(&pageSetting, &itemColor, "Reversed color", ITEM_SWITCH, &reversedColor);
    EasyUIAddItem(&pageSetting, &itemBrightness, "LCD Brightness", ITEM_PROGRESS_BAR, &setting_brightness, EventChangeBrightness);

		setting_brightness = (des.brides.brightness+1)/10;
		
		dbmsg("setting_brightness: %f",setting_brightness);
}