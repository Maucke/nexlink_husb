/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#ifndef _EASY_UI_H_
#define _EASY_UI_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "easy_key.h"
#include "nv3030b.h"
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "profile_photo_erbws.h"

// Operation response
extern uint8_t opnForward, opnBackward;
extern uint8_t opnEnter, opnExit, opnUp, opnDown;

#define MPUCRL          1
#define KEY_NUM         3
#define ROTARY          0

#define BATTERY_ADC_PIN         ADC2_IN2_A2
#define LOWEST_BATTERY_VOLTAGE  0.0f

#define FONT_WIDTH              12
#define FONT_HEIGHT             16
#define ITEM_HEIGHT             20
#define CHECK_BOX_OFFSET        2
#define RADIO_BUTTON_OFFSET        3
#define SCROLL_BAR_WIDTH        4
#define ITEM_LINES              ((uint8_t)(SCREEN_HEIGHT / ITEM_HEIGHT))
#define MAX_LAYER               10
#define ICON_SIZE               50

// Represent the time it takes to play the animation, smaller the quicker. Unit: ms
#define INDICATOR_MOVE_TIME     70
#define ITEM_MOVE_TIME          70
#define TRANSITION_TIME         60

#define OFFSET_X 10
#define OFFSET_Y 40
#define SCREEN_WIDTH            (LCD_W - OFFSET_X*2)
#define SCREEN_HEIGHT           (LCD_H - OFFSET_Y*2)
#define EasyUIScreenInit(gram)                                  (NV3030B_Init(gram))
#define EasyUIDisplayStr(x, y, str)                             (NV3030B_ShowStr((x) + OFFSET_X, (y) + OFFSET_Y, str))
#define EasyUIDisplayFloat(x, y, dat, num, pointNum)            (NV3030B_ShowFloat((x) + OFFSET_X, (y) + OFFSET_Y, dat, num, pointNum))
#define EasyUIDrawDot(x, y, color)                              (NV3030B_DrawPoint((x) + OFFSET_X, (y) + OFFSET_Y, color))
#define EasyUIDrawBox(x, y, width, height, color)               (NV3030B_DrawBox((x) + OFFSET_X, (y) + OFFSET_Y, width, height, color))
#define EasyUIDrawFrame(x, y, width, height, color)             (NV3030B_DrawFrame((x) + OFFSET_X, (y) + OFFSET_Y, width, height, color))
#define EasyUIDrawRFrame(x, y, width, height, color, r)         (NV3030B_DrawRFrame((x) + OFFSET_X, (y) + OFFSET_Y, width, height, color, r))
#define EasyUIDrawRBox(x, y, width, height, color, r)           (NV3030B_DrawRBox((x) + OFFSET_X, (y) + OFFSET_Y, width, height, color, r))
#define EasyUIClearBuffer(void)                                     (NV3030B_ClearBuffer())
#define EasyUISendBuffer(void)                                      (NV3030B_SendBuffer())
#define EasyUISetDrawColor(mode)                                (NV3030B_SetDrawColor(mode))
#define EasyUIDisplayBMP(x, y, width, height, pic)              (NV3030B_ShowBMP((x) + OFFSET_X, (y) + OFFSET_Y, width, height, pic))
#define EasyUIModifyColor(void)                                     (NV3030B_ModifyColor())
#define EasyUIDrawCircle(x, y, r, color, section)                 (NV3030B_DrawCircle((x) + OFFSET_X, (y) + OFFSET_Y, r, color, section))
#define EasyUIDrawDisc(x, y, r, color, section)                 (NV3030B_DrawDisc((x) + OFFSET_X, (y) + OFFSET_Y, r, color, section))

#define EasyUIGetBatVoltage(void)                                   (4.2f)
#define EasyUIDelay_ms(time)                                    (HAL_Delay(time))

typedef     float      paramType;

typedef enum
{
    ITEM_PAGE_DESCRIPTION,
    ITEM_JUMP_PAGE,
    ITEM_SWITCH,
    ITEM_CHANGE_VALUE,
    ITEM_PROGRESS_BAR,
    ITEM_RADIO_BUTTON,
    ITEM_CHECKBOX,
    ITEM_MESSAGE
} EasyUIItem_e;

typedef enum
{
    PAGE_LIST,
    PAGE_ICON,
    PAGE_CUSTOM
} EasyUIPage_e;

typedef struct EasyUI_item
{
    struct EasyUI_item *next;

    EasyUIItem_e funcType;
    uint8_t id;
    int16_t lineId;
    float posForCal;
    float step;
    int16_t position;
    char *title;

    char *msg;                                  // ITEM_MESSAGE
    bool *flag;                                 // ITEM_CHECKBOX and ITEM_RADIO_BUTTON and ITEM_SWITCH
    bool flagDefault;                           // Factory default setting
    paramType *param;                           // ITEM_CHANGE_VALUE and ITEM_PROGRESS_BAR
    paramType paramDefault;                     // Factory default setting
    paramType paramBackup;                      // ITEM_CHANGE_VALUE and ITEM_PROGRESS_BAR
    uint8_t pageId;                             // ITEM_JUMP_PAGE
    void (*Event)(struct EasyUI_item *item);    // ITEM_CHANGE_VALUE and ITEM_PROGRESS_BAR
} EasyUIItem_t;

typedef struct EasyUI_page
{
    struct EasyUI_page *next;

    EasyUIPage_e funcType;
    EasyUIItem_t *itemHead, *itemTail;
    uint8_t id;

    void (*Event)(struct EasyUI_page *page);
} EasyUIPage_t;

extern char *EasyUIVersion;
extern bool functionIsRunning, listLoop, errorOccurred, batteryMonitor;
extern EasyUIPage_t *pageHead, *pageTail;

void EasyUIAddItem(EasyUIPage_t *page, EasyUIItem_t *item, char *_title, EasyUIItem_e func, ...);
void EasyUIAddPage(EasyUIPage_t *page, EasyUIPage_e func, ...);
void EasyUITransitionAnim(void);
void EasyUIBackgroundBlur(void);

void EasyUIDrawMsgBox(char *msg);
float EasyUIGetBatteryVoltage(void);

void EasyUIEventChangeUint(EasyUIItem_t *item);
void EasyUIEventChangeInt(EasyUIItem_t *item);
void EasyUIEventChangeFloat(EasyUIItem_t *item);
void EasyUIEventSaveSettings(EasyUIItem_t *item);
void EasyUIEventResetSettings(EasyUIItem_t *item);
void EasyUIEventChangeFloatForYaw(EasyUIItem_t *item);
void EasyUIInit(uint8_t mode);
void EasyUIEvent(uint8_t timer);
void EasyUIItemOperationResponse(EasyUIPage_t *page, EasyUIItem_t *item, uint8_t *index);

#ifdef __cplusplus
}
#endif

#endif
