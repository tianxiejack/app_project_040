#include <stdio.h>
#include "uart_api.hpp"

//extern "C"{
#include"Arachne_Ipc_Message.h";
//}

void recvmsg_040(IPC_msg data);
void app_ctrl_setSensor(CORE_CMD * pInCmd);
void app_ctrl_setEnhance(CORE_CMD * pInCmd);
void app_ctrl_setTrkStat(CORE_CMD * pInCmd);
void app_ctrl_setAxisPos(int dir, int step);
void app_ctrl_SaveAxisPos();

int app_ctrl_setMmtSelect(CORE_CMD * pInCmd);
void app_ctrl_setMMT(CORE_CMD * pInCmd);
void app_ctrl_setMMTTrk(CORE_CMD * pInCmd);
void app_ctrl_setForceCoast(CORE_CMD * pInCmd);
void app_ctrl_setTrkBomen(CORE_CMD * pInCmd);
void app_ctrl_setAimPos(int dir, int step);
void app_ctrl_setZoom(CORE_CMD * pInCmd);
void app_ctrl_setAimSize(CORE_CMD * pInCmd);
void app_ctrl_setFovCtrl(CORE_CMD * pInCmd);
void app_ctrl_setSearchTrk(CORE_CMD * pInCmd );
void app_ctrl_setSysmode(CORE_CMD * pInCmd);
void app_ctrl_setDispGrade(CORE_CMD * pInCmd);
void app_ctrl_setDispColor(CORE_CMD * pInCmd );
void app_ctrl_setPicp(CORE_CMD * pInCmd);
void app_ctrl_poweron(CORE_CMD * pInCmd );
void app_ctrl_set422Rate(CORE_CMD * pInCmd );
void app_ctrl_set422TransMode(CORE_CMD * pInCmd );

void app_getCoredata();
