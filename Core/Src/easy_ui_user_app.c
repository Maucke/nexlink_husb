/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#include "easy_ui_user_app.h"

// Pages
EasyUIPage_t pageWelcome, pageMain, pagePreset, pageFlyWheelPID, pageDirPID, pageBackMotorPID, pageThreshold, pageCam, pagePoints, pageNormalPoints, pagePathGenerate,pageBasePoints,pageConePoints,pagePilePoints,pageSetting, pageAbout, pageVoltage, pageGenerateCone,pageGeneratePile;

// Items
EasyUIItem_t titleMain, itemRun, itemPreset, itemSpdPID, itemDirPID, itemBackMotor, itemNorDynaGain, itemTurnDynaGain,itemSlowVel, itemFastVel, itemTurnVel, itemRampVel, itemSlowServo, itemFastServo, itemTurnServo, itemEncode1, itemEncode2, itemThreshold, itemCam, itemGPS,itemSetKgain,itemSetYawBias,itemSetStaticAngle, itemSetServoCalibration, itemSetServoDitherFactor, itemSetting,itemGenCone,itemGenPile;
EasyUIItem_t titleGPS, itemBasePoints,itemNormalPoints,itemConePoints,itemPilePoints,itemPathGenerate, itemSavePoints, itemReadPoints,itemCNX,itemCNY,itemSSD,itemCYF,itemEGFN,itemSCY,itemSMC,itemSetIndex,itemSRY,itemSetConeCounts,itemSetConeTotalDis,itemSetConeHorizonDis,itemSetConeDir,itemSetPileRadius,itemSetPileDir;
EasyUIItem_t titleSpdPID, itemSpdKp, itemSpdKi, itemSpdKd, itemAngKp, itemAngKi, itemAngKd, itemAngSpdKp, itemAngSpdKi, itemAngSpdKd, KpitemSpdTarget, itemSpdInMax, itemSpdErrMax, itemSpdErrMin;
EasyUIItem_t titleDirPID, itemDirKp, itemDirKi, itemDirKd, itemDirInMax, itemDirErrMax, itemDirErrMin;
EasyUIItem_t titleBackMotorPID, itemBackMotorKp, itemBackMotorKi, itemBackMotorKd, itemBackMotorInMax, itemBackMotorErrMax, itemBackMotorErrMin;
EasyUIItem_t itemExp, itemTh;
EasyUIItem_t titleEle, itemLoop, itemCross, itemLeftR, itemRightR, itemBreak, itemObstacle, itemGarage;
EasyUIItem_t titleSetting, itemColor, itemListLoop, itemBuzzer, itemSave, itemReset, itemAbout, itemVoltage;

double X0,Y0;
void EventMainLoop(EasyUIItem_t *item)
{
#if USE_GPS == 1
    uint8_t status=0;
    if(Bike_Start ==0||Bike_Start==3)
    {
        cone_handler_index=0;
        cone_handler_flag = false;
        dirPid.Kp = fast_servo_kp;
        dynamic_gain = normal_dynamic_gain;
        motoDutySet(SERVO_PIN,SERVO_MID);
        servo_input_duty = SERVO_MID;
        if(!GlobalGraph.is_init ||!GlobalGraph.B_constructor->is_interpolated)
        {
            functionIsRunning = false;
            EasyUIDrawMsgBox("Not generate!");
            EasyUIBackgroundBlur();
            return;
        }
        stanleyControllerInit(&Global_stanleyController,(float)Global_k_gain,(float)0.05,&Global_yaw,&Global_v_now,&Global_current_node);
        status|=stanleyBuffLink(&Global_stanleyController,Global_pd_array,NULL,GlobalGraph.total);
        status|=stanley_GraphRegister(&GlobalGraph,&Global_stanleyController);
        status|=GraphNode_Diff(&GlobalGraph);
        INS_init();
        X0 = GlobalGraph.nodeBuff[0].X;
        Y0 = GlobalGraph.nodeBuff[0].Y;
        if(status)
        {
            functionIsRunning = false;
            EasyUIDrawMsgBox("Err check uart msg!");
            EasyUIBackgroundBlur();
            return;
        }
        GlobalGraph.is_finish = 0;
        Bike_Start = 2;
        uint16 temp=2000;
        while(!opnEnter){
            if (--temp==0)
            {
                NV3030B_ClearBuffer();
                NV3030B_ShowStr(0, 2, "offsetX:");
                NV3030B_ShowStr(0, 14, "offsetY:");
                NV3030B_ShowFloat(60, 2, moveArray.offsetX,3,3);
                NV3030B_ShowFloat(60, 14, moveArray.offsetY,3,3);
                NV3030B_SendBuffer();
//                BlueToothPrintf("%f,%f\n",moveArray.offsetX,moveArray.offsetY);
                temp = 2000;
            }
        }
        opnEnter = false;
        Bike_Start = 1;
    }
    pidClear(&backSpdPid);
    backSpdPid.target[NOW]=fast_velocity;
    anti_dither_flag = true;
    while(1)
    {
        static uint16 temp=4000;
        if(--temp==0)
        {
//            BlueToothPrintf("%f,%f\n",Global_current_node.X,Global_current_node.Y);
            temp = 4000;
        }
        if(!stagger_flag)
        {
            status |= Stanley_Control(&GlobalGraph);
            gpsConeHandler();
            if(status)
            {
                functionIsRunning = false;
                EasyUIDrawMsgBox("Err check uart msg!");
                EasyUIBackgroundBlur();
                return;
            }
            gps_use.delta = RAD_TO_ANGLE(GlobalGraph.Stanley_controller->theta);
            if(GlobalGraph.is_finish)
            {
                motoDutySet(SERVO_PIN,SERVO_MID);
                myTimeStamp = 0;
                functionIsRunning = false;
                beepTime = 1500;
                Bike_Start = 0;
                anti_dither_flag = false;
                break;
            }
        }
        if (opnExit)
        {
            motoDutySet(SERVO_PIN,SERVO_MID);
            anti_dither_flag = false;
            Bike_Start = 0;
            opnExit = false;
            functionIsRunning = false;
            EasyUIBackgroundBlur();
            break;
        }
    }
#elif USE_GPS == 2
        Bike_Start = 1;
//        gpsTest();
#endif

}

void EventChangeBuzzerVolume(EasyUIItem_t *item)
{
    if (opnUp)
    {
        if (*item->param + 10 <= 100)
            *item->param += 10;
        else
            *item->param = 100;
        opnUp = opnForward = false;
    }
    if (opnDown)
    {
        if (*item->param - 10 >= 0)
            *item->param -= 10;
        else
            *item->param = 0;
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
        EasyUIBackgroundBlur();
        functionIsRunning = false;
        opnExit = false;
    }
}


void PageWelcome(EasyUIPage_t *page)
{
    static uint8_t count = 50;
    static float voltage = 0.0f;
    if (count++ >= 50)
    {
        voltage = 4.2f;
        count = 0;
    }
    NV3030B_ShowStr(0, 2, "Battery Voltage:");
    NV3030B_ShowFloat(60, 40, voltage, 2, 2);
    NV3030B_ShowStr(95, 40, "V");

//    NV3030B_ShowStr(7, 9, page->itemHead->title);
//    uint8_t len = strlen(page->itemHead->title);
//    NV3030B_SetDrawColor(XOR);
//    NV3030B_DrawRBox(5, 5, len * FONT_WIDTH + 5, ITEM_HEIGHT, NV3030B_penColor, 1);
//    NV3030B_SetDrawColor(NORMAL);
//    NV3030B_ShowStr(7, 25, "*1.Press <Center> to run");
//    NV3030B_ShowStr(7, 41, " 2.Hold <Center> to enter settings");

//    if (opnEnter)
//    {
//        functionIsRunning = true;
//        EasyUIDrawMsgBox(page->itemHead->msg);
//    }
}


/*!
 * @brief   Custom page of Image
 *
 * @param   page    Useless param
 * @return  void
 */
void PageImage(EasyUIPage_t *page)
{
    if (opnUp)
    {
        if (*page->itemHead->param + 10 <= 500)
            *page->itemHead->param += 10;
        else
            *page->itemHead->param = 500;
    }
    if (opnDown)
    {
        if (*page->itemHead->param - 10 >= 100)
            *page->itemHead->param -= 10;
        else
            *page->itemHead->param = 0;
    }
}

/*!
 * @brief   Custom page of {About}
 *
 * @param   page    Useless param
 * @return  void
 */
void PageAbout(EasyUIItem_t *page)
{
    static uint8_t time = 0;
    static float x = SCREEN_WIDTH;
    static float step = (float) (SCREEN_WIDTH - 115) / 5;

    // Display about info
    NV3030B_ClearBuffer();
    NV3030B_ShowStr(3, 4, "SCEP");
    NV3030B_SetDrawColor(XOR);
    NV3030B_DrawRBox(1, 1, 4 * FONT_WIDTH + 5, ITEM_HEIGHT, NV3030B_penColor, 1);
    NV3030B_SetDrawColor(NORMAL);
    NV3030B_DrawBox(2, 16, 2, ITEM_HEIGHT * 5, NV3030B_penColor);
    NV3030B_ShowStr(36, 4, "v1.2");
    NV3030B_ShowStr(8, 18, "MCU    : CH32V3");
    NV3030B_ShowStr(8, 30, "EasyUI : ");
    NV3030B_ShowStr(8 + 9 * FONT_WIDTH, 30, EasyUIVersion);
    NV3030B_ShowStr(8, 42, "Flash  : 256KB");
    NV3030B_ShowStr(8, 54, "UID    : ");
    NV3030B_ShowStr(8, 66, ">> Powered by: ErBW_s");

    // Get uid
    static uint32_t *addrBase = (uint32_t *) 0x1FFFF7E8;
    uint64_t uid;
    memcpy(&uid, addrBase, 8);
    char str[13];
    uint64_t uidBackup = uid;
    const char hex_index[16] = {
            '0', '1', '2', '3',
            '4', '5', '6', '7',
            '8', '9', 'A', 'B',
            'C', 'D', 'E', 'F'};
    int8_t data_temp[16];
    uint8_t bit = 0, i = 0;
    while (bit < 16)
    {
        data_temp[bit++] = (uidBackup & 0xF);
        uidBackup >>= 4;
    }
    for (bit = 12; bit > 0; bit--)
    {
        str[i++] = hex_index[data_temp[bit - 1]];
    }
    str[i] = '\0';
    NV3030B_ShowStr(8 + 9 * FONT_WIDTH, 54, str);

    // Display profile photo
    if (time < 5)
    {
        x -= step;
        time++;
    } else
        x = 115;
//    EasyUIDisplayBMP((int16_t) x, (SCREEN_HEIGHT - 56) / 2, 29, 28, ErBW_s_2928);
    if (opnExit)
    {
        time = 0;
        x = SCREEN_WIDTH;
    }
}


void MenuInit()
{
    EasyUIAddPage(&pageMain, PAGE_LIST);
    EasyUIAddPage(&pagePoints, PAGE_LIST);
    EasyUIAddPage(&pageGenerateCone, PAGE_LIST);
    EasyUIAddPage(&pageGeneratePile, PAGE_LIST);
    EasyUIAddPage(&pageFlyWheelPID, PAGE_LIST);
    EasyUIAddPage(&pageDirPID, PAGE_LIST);
    EasyUIAddPage(&pageBackMotorPID, PAGE_LIST);
    EasyUIAddPage(&pageSetting, PAGE_LIST);
    EasyUIAddPage(&pageAbout, PAGE_CUSTOM, PageAbout);
    EasyUIAddPage(&pageVoltage, PAGE_CUSTOM, PageWelcome);

    // Page Main
    EasyUIAddItem(&pageMain, &titleMain, "[Main]", ITEM_PAGE_DESCRIPTION);
    EasyUIAddItem(&pageMain, &itemRun, "Run", ITEM_MESSAGE, "Running...", EventMainLoop);
    EasyUIAddItem(&pageMain, &itemGPS, "GPS Points", ITEM_JUMP_PAGE, pagePoints.id);
    EasyUIAddItem(&pageMain, &itemSetting, "Settings", ITEM_JUMP_PAGE, pageSetting.id);

    // Page GPS points
    EasyUIAddItem(&pagePoints, &titleGPS, "[GPS Points]", ITEM_PAGE_DESCRIPTION);
    EasyUIAddItem(&pagePoints, &itemBasePoints, "Base Points", ITEM_JUMP_PAGE, pageBasePoints.id);
    EasyUIAddItem(&pagePoints, &itemNormalPoints, "Normal Points", ITEM_JUMP_PAGE, pageNormalPoints.id);

    EasyUIAddItem(&pagePoints, &itemConePoints, "Cone Points", ITEM_JUMP_PAGE, pageConePoints.id);
    EasyUIAddItem(&pagePoints, &itemPilePoints, "Pile Points", ITEM_JUMP_PAGE, pagePilePoints.id);
//    EasyUIAddItem(&pagePoints, &itemSSD, "Set Step Distance", ITEM_CHANGE_VALUE, &distance_step, EasyUIEventChangeFloat);
//    EasyUIAddItem(&pagePoints, &itemCNX, "Det X", ITEM_CHANGE_VALUE, &normalXArray[0], EasyUIEventChangeFloat);
//    EasyUIAddItem(&pagePoints, &itemCNY, "Det Y", ITEM_CHANGE_VALUE, &normalYArray[0], EasyUIEventChangeFloat);
//
//    EasyUIAddItem(&pagePoints, &itemSMC, "Set MulCounts", ITEM_CHANGE_VALUE, &multiple_counts, EasyUIEventChangeUint);
//    EasyUIAddItem(&pagePoints, &itemCYF, "Enable Const Angle", ITEM_SWITCH, &constant_angle_flag);
//    EasyUIAddItem(&pagePoints, &itemEGFN, "Enable GPS Normal", ITEM_SWITCH, &normal_gps_enable);
//    EasyUIAddItem(&pagePoints, &itemSCY, "Set Constant Angle", ITEM_CHANGE_VALUE, &constant_angle, EasyUIEventChangeFloatForYaw);
//    EasyUIAddItem(&pagePoints, &itemSRY, "Set Ref Angle", ITEM_CHANGE_VALUE, &ref_angle, EasyUIEventChangeFloatForYaw);
//
//    EasyUIAddItem(&pagePoints, &itemGenCone, "ConeGenerate Setting", ITEM_JUMP_PAGE, pageGenerateCone.id);
//    EasyUIAddItem(&pagePoints, &itemGenPile, "PileGenerate Setting", ITEM_JUMP_PAGE, pageGeneratePile.id);


    

    // Page setting
    EasyUIAddItem(&pageSetting, &titleSetting, "[Settings]", ITEM_PAGE_DESCRIPTION);
    EasyUIAddItem(&pageSetting, &itemVoltage, "Show Voltage", ITEM_JUMP_PAGE, pageVoltage.id);
    EasyUIAddItem(&pageSetting, &itemColor, "Reversed Color", ITEM_SWITCH, &reversedColor);
    EasyUIAddItem(&pageSetting, &itemListLoop, "List Loop", ITEM_SWITCH, &listLoop);
    
//    EasyUIAddItem(&pageSetting, &itemBuzzer, "Buzzer Volume", ITEM_PROGRESS_BAR, &buzzerVolume, EventChangeBuzzerVolume);
    EasyUIAddItem(&pageSetting, &itemSave, "Save Settings", ITEM_MESSAGE, "Saving...", EasyUIEventSaveSettings);
    EasyUIAddItem(&pageSetting, &itemReset, "Reset Settings", ITEM_MESSAGE, "Resetting...", EasyUIEventResetSettings);
    EasyUIAddItem(&pageSetting, &itemAbout, "<About>", ITEM_JUMP_PAGE, pageAbout.id);

}
