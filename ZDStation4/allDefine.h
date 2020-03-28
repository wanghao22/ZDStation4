#pragma once

#include <iostream>
#include <opencv2\opencv.hpp>

#define MAX_DEVICE_NUM	1
#define IMG_HEIGHT		2048
#define IMG_WIDTH		3072
#define BUF_SIZE		(IMG_HEIGHT*IMG_WIDTH*1)
#define IMG_CHANNEL		CV_8UC1
#define CAMERA_SN1		"00D79917666"

#define _USE_GLOG_
//是否使用相机
//#define _DONT_CONNECT_CAM_

#define _WINDOW_NAME_	"station"

//检测参数
#define _DECT_THRE_					90
//中心圆最小面积
#define _CENTER_MIN_AREA_			40000
//中心圆最大面积
#define _CENTER_MAX_AREA_			52900
//大圆半径
#define _LARGE_CIRCLE_R_			640
//小圆半径
#define _SMALL_CIRCLE_R_			440
//螺丝孔最小面积
#define _SIX_CIRCLE_MIN_AREA_		9000
//螺丝孔最大面积
#define _SIX_CIRCLE_MAX_AREA_		14000
//最小长宽差
#define _SUBWH_MIN_					10
//像素转为实际值
#define _PIX_TO_MM_					(21.0/544.65)
//最小旋转角度
#define _MIN_ROTATE_ANGLE_			-4.0
//最大旋转角度
#define _MAX_ROTATE_ANGLE_			27.0
//超出角度范围
#define _OUT_OF_ANGLE_				-101.9
//两边像素差
const double g_r = (21.0 - 14.0) / _PIX_TO_MM_;
//给0
#define _FLP_GIVE_ZERO_	

/*definition for returned value*/
#define STATUS_OK               1
#define STATUS_ERROR            0

/*trigger mode*/
#define TRIGGER_OFF             0
#define TRIGGER_ON              1
#define TRIGGER_NOT_SET         -1

/*trigger source*/
#define SOFTWAREMODE            7
#define HAREWAREMODE            0