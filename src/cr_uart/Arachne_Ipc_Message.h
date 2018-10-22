/*
 * Arachne_Ipc_Message.h
 *
 *  Created on: Dec 8, 2017
 *      Author: xz
 */
#ifndef ARACHNE_IPC_MESSAGE_H
#define ARACHNE_IPC_MESSAGE_H

typedef enum {
	Message_Id_start = 0x0, Message_Id_TV = 0x65, //电视
	Message_Id_Thermal_Image = 0x66, //热像
	Message_Id_Reserved,
} Message_Id; //信息ID字节

typedef enum {
	Video_Switch_start = 0x0, Video_Switch_TV, //电视
	Video_Switch_Infrared, //红外
	Video_Switch_Reserved,
} Video_Switch; //视频切换

typedef enum {
	State_Off = 0x0, //关闭
	State_On, //打开/上电/开启
	State_Reserved,
} State; //状态

typedef enum {
	character_Color_white = 0x0, //白字符（缺省）
	character_Color_black, //黑字符
	character_Color_Reserved,
} Character_Color; //字符颜色

typedef enum {
	Character_State_Show = 0x0, //正常显示（缺省）
	Character_State_Hide, //字符消隐
	Character_State_All_Hide, //
	Character_State_Reserved, //全消隐
} Character_State; //字符显示

typedef enum {
	Laser_State_Prepare = 0x0, //激光准备
	Laser_State_Standby, //激光待机
	Laser_State_Once_Range, //激光单次测距
	Laser_State_Continuous_Range_1Hz, //激光连续测距1HZ
	Laser_State_Continuous_Range_5Hz, //激光连续测距5HZ
	Laser_State_Once_Irradiate, //激光单发照射
	Laser_State_Continuous_Irradiate, //激光快发照射
	Laser_State_Reserved,
} Laser_State; //激光器信息

typedef enum Laser_Debug_Message {
	Laser_Mes_D_Hide = 0x0, //消隐（缺省）
	Laser_Mes_D_Show, //显示
	Laser_Mes_D_Reserved,
} Laser_Mes_D; //调试信息显示

typedef enum {
	Laser_Code_Start = 0x0, Laser_Code_1, //0001时代表编码1
	Laser_Code_8, //1000代表编码8
} Laser_Code; //当前激光编码值

typedef enum {
	Time_Flag_Start = 0x0, Time_Flag_Hour, //时
	Time_Flag_Minute, //分
	Time_Flag_Second, //秒
} Time_Flag; //时间标志

typedef enum {
	Current_Mode_Start = 0x0, Current_Mode_Lock, //锁定
	Current_Mode_Collect, //收藏
	Current_Mode_Manual, //手动
	Current_Mode_Scan, //扫描
	Current_Mode_Tracking, //跟踪
	Current_Mode_Track_Search, //跟踪搜索
	Current_Mode_Geografia_Search, //地理跟踪
	Current_Mode_Search, //搜索
	Current_Mode_Serve, //服务
	Current_Mode_Tracker_Sintering, //跟踪器程序烧结
	Current_Mode_System_SelfInspection, //系统自检
	Current_Mode_Image_Roam, //图像漫游
	Current_Mode_Image_Roam_CharacterShow, //图像漫游(字符显示)
	Current_Mode_Reserved,
} Current_Mode; //当前模式

typedef enum {
	Move_Track_Hor_Off = 0x0, //不动
	Move_Track_Hor_Left, //左
	Move_Track_Hor_Right, //右
	Move_Track_Hor_Reserved, //N/A
} Move_Track_Hor; //跟踪微调指令（水平X方向）

typedef enum {
	Move_Track_Ver_Off = 0x0, //不动
	Move_Track_Ver_Up, //上
	Move_Track_Ver_Down, //下
	Move_Track_Ver_Reserved, //N/A
} Move_Track_Ver; //跟踪微调指令（垂直Y方向）

typedef enum {
	Move_CrossCurve_Hor_Left = 0x01, //左
	Move_CrossCurve_Hor_Right, //右
	Move_CrossCurve_Hor_Off, //不动
	Move_CrossCurve_Hor_Reserved, //N/A
} Move_CrossCurve_Hor; //十字线移动指令

typedef enum {
	Move_CrossCurve_Ver_Up = 0x01, //上
	Move_CrossCurve_Ver_Down, //下
	Move_CrossCurve_Ver_Off, //不动
	Move_CrossCurve_Ver_Reserved, //N/A
} Move_CrossCurve_Ver; //十字线移动指令

typedef enum {
	Bomen_Size_Off = 0x00, //NA
	Bomen_Size_Big, //大波门
	Bomen_Size_Middle, //中波门
	Bomen_Size_Small, //小波门
	Bomen_Size_Reserved,
} Bomen_Size; //波门大小

typedef enum {
	Track_Grade_Default = 0x00, //不变（缺省）
	Track_Grade_1, //1
	Track_Grade_2, //2
	Track_Grade_3, //3
	Track_Grade_Reserved,
} Track_Grade; //跟踪模板更新系数等级

typedef enum {
	Interface_Rate_NA = 0x00, //NA
	Interface_Rate_1, //1.92Mbit/s速率
	Interface_Rate_3, //3.84Mbit/s 速率
	Interface_Rate_7, //7.96Mbit/s 速率
	Interface_Rate_Reserved,
} Interface_Rate; //同步接口速率切换
typedef enum {
	Video_Transfer_NA = 0x00, //NA
	Video_Transfer_Tv_Single, //电视单路传输
	Video_Transfer_Thermal_Sigle, //热像单路传输
	Video_Transfer_Together, //两路当前视频同传
	Video_Transfer_Reserved,
} Video_Transfer; //视频传输方式
typedef enum {
	Tv_Frame_NA = 0x00, //NA
	Tv_Frame_30, //电视帧频控制为（30帧）
	Tv_Frame_25, //电视帧频控制为（25帧）
	Tv_Frame_15, //电视帧频控制为 (15帧)
	Tv_Frame_Reserved,
} Tv_Frame;
typedef enum {
	Thermal_Image_Frame_NA = 0x00, //NA
	Thermal_Image_Frame_30, //红外帧频控制为（30帧）
	Thermal_Image_Frame_25, //红外帧频控制为（25帧）
	Thermal_Image_Frame_15, //红外帧频控制为 (15帧)
	Thermal_Image_Frame_Reserved,
} Thermal_Image_Frame; //热像帧频
/******************************************************************************/

typedef enum {
	Current_Video_TV = 0x00, //电视
	Current_Video_Thermal, //红外
	Current_Video_Reserved,
} Current_Video; //当前视频类型

typedef enum {
	Self_State_Normal = 0x00, //正常
	Self_State_Abnomal, //自检结果出错
	Self_State_Reserved,
} Self_State; //自检信息/跟踪状态
typedef enum {
	Video_Input_Normal = 0x00, //正常
	Video_Input_TV_Off, //电视视频无输入
	Video_Input_Thermal_Off, //热像视频无输入
	Video_Input_Together_Off, //电视、热像视频都无输入
	Video_Input_Reserved,
} Video_Input; //视频输入状态
typedef enum {
	Roam_Boot_off = 0x00, //未到位
	Roam_Boot_Normal, //到位
	Roam_Boot_Reserved,
} Roam_Boot; //漫游导引是否到位
typedef enum {
	Multi_Target_AVT_NA = 0x00, //N/A
	Multi_Target_AVT_YES, //是（选择对）
	Multi_Target_AVT_NO, //否（选择不对）
	Multi_Target_AVT_Reserved,
} Multi_Target_AVT; //多目标探测时，操作员选择的目标是否为AVT标识出的目标
typedef enum {
	Track_State_NA = 0x00, //N/A
	Track_State_Lose, //目标丢失
	Track_State_Tracking, //正在跟踪
	Track_State_Memory, //记忆跟踪
	Track_State_Reserved,
} Track_State; //“跟踪状态”位

/******************************************************************************/

typedef enum {
	IPC_MSG_TYPE_INIT,
	IPC_MSG_TYPE_STE_TO_AVT = 0x01,
	IPC_MSG_TYPE_AVT_TO_SET,
	IPC_MSG_COUNT,
} IPC_MSG_TYPE;

typedef struct {
	long msg_type;
	struct Arachne_payload {
		Message_Id message_id; //信息ID字节
		struct sensor_message {
			Video_Switch video_switch; //视频切换
			State Infrared_state; //红外上电状态
			State Laser_state; //激光上电状态
			Character_Color character_color; //字符颜色
			State Image_Enhance; //图像增强(电视下的指令)
			Character_State character_state; //字符显示
		} sen_mes; //传感器信息
		unsigned int Sensor_Tv_Angle; //电视传感器视场角
		struct Laser_Message {
			Laser_State laser_state; //激光状态
			Laser_Mes_D laser_mes_d; //调试信息显示
			Laser_Code laser_code; //当前激光编码值
		} Laser_Mes; //激光器信息
		struct {
			Current_Mode cur_mode; //当前模式
			State Comp_SelfCheck; //强制自检命令
			State LMC; //LMC‘开/关’位
			State Shaft_Start; //进入校轴
			State Shart_Exit; //退出校轴
		} Work_Pattern; //工作模式信息
		struct Tracking_Search {
			unsigned char HighOrder_Hor; //水平方向高位信息
			unsigned char HighOrder_Ver; //垂直方向高位信息
			State MulTarget_Detect; //多目标探测
			unsigned char LowOrder_Hor; //水平方向像素低位信息
			unsigned char LowOrder_Ver; //垂直方向像素低位信息
		} Track_Search; //跟踪搜索功能
		struct Tracking_Trimming {
			Move_Track_Hor move_track_hor; //跟踪微调指令（水平X方向）
			Move_Track_Ver move_track_ver; //跟踪微调指令（垂直Y方向）
			Move_CrossCurve_Hor move_cross_hor; //十字线移动指令，X方向
			Move_CrossCurve_Ver move_cross_ver; //十字线移动指令，Y方向
		} Track_Trim; //跟踪微调/校轴信息
		unsigned int Target_Distance; //目标距离
		unsigned int TurretAngle_Hor; //转塔方位角度
		int TurretAngle_Ver; //转塔俯仰角度
		struct Time_Date {
			Time_Flag time_flag; //时标
			unsigned char time_date; //时间数据
		} Td; //时标-时/分/秒（复用）
		struct MulTarget_Choice_Number {
			unsigned char Mul_Number; //多目标选择编号（1～8）
			State TV_SizeChage; //电视电子变倍
			Bomen_Size bomen_size; //波门大小
			State White_Black; //黑白切换
		} MulTarget_Num; //多目标选择编号/黑白切换/波门大小选择
		struct Sensor_Image_Enhance {
			State Roam_Bomen_Switch; //漫游引导波门开关
			Track_Grade track_grade; //跟踪模板更新系数等级
			State En_AutoFocus; //自动聚焦(使能强制跟踪)
			State Infrared_Change; //红外电子变倍
			State Sensor_Thermal; //热像传感器图像增强状态
			State Sensor_Tv_Enhance; //电视传感器图像增强状态
		} Sensor_Image_EN; //传感器图像增强
		struct Video_Compress {
			Interface_Rate interface_rate; //同步接口速率切换
			Video_Transfer video_transfer; //视频传输方式
			Tv_Frame video_frame; //电视帧频
			Thermal_Image_Frame Thermal_frame; //热像帧频
		} Video_C; //视频压缩指令
		unsigned int Sensor_Thermal_Angle; //红外传感器视场角
	} payload;
} IPC_msg; //接收的数据包

typedef struct {
	unsigned int CycleTime; //周期发送时间，单位ms
	struct return_payload {
		Message_Id message_id; //信息ID字节
		int Track_Deviation_X; //跟踪偏差△X
		int Track_Deviation_Y; //跟踪偏差△Y
		Track_Grade Track_Thermal; //红外跟踪模板更新系数等级
		Track_Grade Track_Tv; //电视跟踪模板更新系数等级
		Current_Video current_video; //当前视频类型
		unsigned int Senser_Aim_CurX; //当前观瞄传感器轴位信息X
		unsigned int Senser_Aim_CurY; //当前观瞄传感器轴位信息Y
		struct Self_Tracking_State {
			Self_State self_state; //自检结果
			Video_Input video_input; //视频输入状态
			Roam_Boot roam_boot; //漫游导引是否到位
			Multi_Target_AVT mul_target_AVT; //多目标探测时，操作员选择的目标是否为AVT标识出的目标
			Track_State track_state; //“跟踪状态”位
		} Self_Tr_S; //自检信息/跟踪状态
		Video_Transfer Compress_Transfer; //表示压缩板传输状态
		State Power_On; //表示上电启动状态
		Interface_Rate r_interface_rate; //表示同步接口速率
		Thermal_Image_Frame r_thermal_frame; //热像下传帧频
		Tv_Frame r_tv_frame; //电视下传帧频
	} payload;
} Return_pack; //返回的数据包

void InitIPCModule(); //初始化消息队列并打开串口
void SetReturnPack(Return_pack* value); //设置返回包的数据，以及周期发送时间
IPC_msg GetMessage(); //返回消息结构体
void IPC_Destroy(); //删除消息队列并关闭串口
void sendMsg_Sem();

#endif /* Arachne_MESSAGE_H_ */
