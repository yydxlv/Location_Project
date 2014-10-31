#pragma once
#include"JWD.h"
using namespace System;
namespace Location{
ref class CJWDHelper
{
	public:
		CJWDHelper(void);
		static double PI=3.1415926;
		//! 计算点A 和 点B的经纬度，求他们的距离和点B相对于点A的方位
		/*! 
		  * \param A A点经纬度
		  * \param B B点经纬度
		  * \param angle B相对于A的方位, 不需要返回该值，则将其设为空
		  * \return A点B点的距离
		  */
		static double distance(JWD^ A, JWD^ B, double angle);
		//! 计算点A 和 点B的经纬度，求他们的距离和点B相对于点A的方位
		/*! 
		  * \param longitude1 A点经度
		  * \param latitude1 A点纬度
		  * \param longitude2 B点经度
		  * \param latitude2 B点纬度
		  * \param angle B相对于A的方位, 不需要返回该值，则将其设为空
		  * \return A点B点的距离
		  */
		static double distance(
			double longitude1, double latitude1,double longitude2, double latitude2, double angle);
		//! 已知点A经纬度，根据B点据A点的距离，和方位，求B点的经纬度
		/*!
		  * \param A 已知点A
		  * \param distance B点到A点的距离 
		  * \param angle B点相对于A点的方位
		  * \return B点的经纬度坐标
		  */
		static JWD^ GetJWDB(JWD^ A, double distance, double angle);
		//! 已知点A经纬度，根据B点据A点的距离，和方位，求B点的经纬度
		/*!
		  * \param longitude 已知点A经度
		  * \param latitude 已知点A纬度
		  * \param distance B点到A点的距离 
		  * \param angle B点相对于A点的方位
		  * \return B点的经纬度坐标
		  */
		static JWD^ GetJWDB(double longitude, double latitude, double distance, double angle);
		static double angle(double longitude1, double latitude1, double longitude2, double latitude2) ;
		static double angle(Location::JWD^ A, Location::JWD^ B);
		static double distance1(double longitude1, double latitude1, double longitude2, double latitude2, double& angle) ;
		static double distance1(Location::JWD^ A, Location::JWD^ B, double& angle);


};
}