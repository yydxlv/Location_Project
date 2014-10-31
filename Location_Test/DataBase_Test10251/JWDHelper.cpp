#include "StdAfx.h"
#include "JWDHelper.h"
using namespace Location;
CJWDHelper::CJWDHelper(void)
{
}
//计算两点间的球面距离，参数为两个经纬度结构和一个用于计算的方向角
double CJWDHelper::distance(Location::JWD^ A, Location::JWD^ B, double angle)
{
	double PI=3.1415926;
	double dx = (B->m_RadLo - A->m_RadLo) * A->Ed;
	double dy = (B->m_RadLa - A->m_RadLa) * A->Ec;
	double outresult = Math::Sqrt(dx * dx + dy * dy);

	if( angle != 390)
	{
		if(dx==0.0)
			angle=0;
		else if(dy==0.0)
			angle=90;
		else
			angle = Math::Atan(Math::Abs(dx/dy))*180/PI;//得到相对角度
		// 判断象限,第一象限不变，顺时针延续1-4象限
		double dLo = B->m_Longitude - A->m_Longitude;
		double dLa = B->m_Latitude - A->m_Latitude;

		if(dLo > 0 && dLa <= 0) 
		{
			angle = (90 - angle) + 90;
		}
		else if(dLo <= 0 && dLa < 0) 
		{
			angle = angle + 180;
		}
		else if(dLo < 0 && dLa >= 0) 
		{
			angle = (90 - angle) + 270;
		}
	}
	return outresult/1000;

}
//计算两点间的球面距离，参数为两点的经纬度信息和一个用于计算的方向角
double CJWDHelper::distance(double longitude1, double latitude1, double longitude2, double latitude2, double angle) 
{
	JWD^ A=gcnew JWD(longitude1,latitude1);
	JWD^ B=gcnew JWD(longitude2,latitude2);
	return distance(A, B,angle);
}
//通过给定的一个点的结构，距离，角度，求出另一个点的位置
JWD^ CJWDHelper::GetJWDB(Location::JWD ^A, double distance, double angle)
{
	double PI=3.1415926;
	double dx = distance*1000 * Math::Sin(angle * PI /180);
	double dy = distance*1000 * Math::Cos(angle * PI /180);

	//double dx = (B.m_RadLo - m_RadLo) * Ed;
	//double dy = (B.m_RadLa - m_RadLa) * Ec;
	double BJD = (dx/A->Ed + A->m_RadLo) * 180/PI;
	double BWD = (dy/A->Ec + A->m_RadLa) * 180/PI;
	JWD^ B=gcnew JWD(BJD, BWD);
	return B; 
}
//通过给定的一个点的经纬度，距离，角度，求出另一个点的位置
JWD^CJWDHelper::GetJWDB(double longitude, double latitude, double distance, double angle) 
{
	JWD^ A=gcnew JWD(longitude,latitude);
	return GetJWDB(A, distance, angle);
}
//给出两个点的经纬度，求出两点见连线与正北方向的夹角
double CJWDHelper::angle(double longitude1, double latitude1, double longitude2, double latitude2) 
{
	JWD^ A=gcnew JWD(longitude1,latitude1);
	JWD^ B=gcnew JWD(longitude2,latitude2);
	return angle(A, B);
}
//给出两个点的结构，求出两点见连线与正北方向的夹角
double CJWDHelper::angle(Location::JWD^ A, Location::JWD^ B)
{
	double angle=0;
	double PI=3.1415926;
	double dx = (B->m_RadLo - A->m_RadLo) * A->Ed;
	double dy = (B->m_RadLa - A->m_RadLa) * A->Ec;
	double outresult = Math::Sqrt(dx * dx + dy * dy);

	if( angle != 390)
	{
		if(dx==0.0)
			angle=0;
		else if(dy==0.0)
			angle=90;
		else
			angle = Math::Atan(Math::Abs(dx/dy))*180/PI;//得到相对角度
		// 判断象限,第一象限不变，顺时针延续1-4象限
		double dLo = B->m_Longitude - A->m_Longitude;
		double dLa = B->m_Latitude - A->m_Latitude;

		if(dLo > 0 && dLa <= 0) 
		{
			angle = (90 - angle) + 90;
		}
		else if(dLo <= 0 && dLa < 0) 
		{
			angle = angle + 180;
		}
		else if(dLo < 0 && dLa >= 0) 
		{
			angle = (90 - angle) + 270;
		}
	}
	return angle;

}
double CJWDHelper::distance1(double longitude1, double latitude1, double longitude2, double latitude2, double& angle) 
{
	JWD^ A=gcnew JWD(longitude1,latitude1);
	JWD^ B=gcnew JWD(longitude2,latitude2);
	return distance1(A, B,angle);
}
double CJWDHelper::distance1(Location::JWD^ A, Location::JWD^ B, double& angle)
{
	double PI=3.1415926;
	double dx = (B->m_RadLo - A->m_RadLo) * A->Ed;
	double dy = (B->m_RadLa - A->m_RadLa) * A->Ec;
	double outresult = Math::Sqrt(dx * dx + dy * dy);

	if( angle != 390)
	{
		if(dx==0.0)
			angle=0;
		else if(dy==0.0)
			angle=90;
		else
			angle = Math::Atan(Math::Abs(dx/dy))*180/PI;//得到相对角度
		// 判断象限,第一象限不变，顺时针延续1-4象限
		double dLo = B->m_Longitude - A->m_Longitude;
		double dLa = B->m_Latitude - A->m_Latitude;

		if(dLo > 0 && dLa <= 0) 
		{
			angle = (90 - angle) + 90;
		}
		else if(dLo <= 0 && dLa < 0) 
		{
			angle = angle + 180;
		}
		else if(dLo < 0 && dLa >= 0) 
		{
			angle = (90 - angle) + 270;
		}
	}
	return outresult/1000;

}

