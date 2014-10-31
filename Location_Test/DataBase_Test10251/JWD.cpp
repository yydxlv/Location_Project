#include "StdAfx.h"
#include "JWD.h"
using namespace Location;
JWD::JWD(void)
{
}
//构造函数
JWD::JWD(double loDeg, double loMin, double loSec, double laDeg, double laMin, double laSec) 
{
	m_LoDeg=loDeg; 
	m_LoMin=loMin; 
	m_LoSec=loSec; 
	m_LaDeg=laDeg; 
	m_LaMin=laMin;
	m_LaSec=laSec;
	m_Longitude = m_LoDeg + m_LoMin / 60 + m_LoSec / 3600;
	m_Latitude = m_LaDeg + m_LaMin / 60 + m_LaSec / 3600;
	m_RadLo  = m_Longitude * PI / 180;
	m_RadLa  = m_Latitude * PI / 180;
	Ec = Rj + (Rc - Rj) * (90- m_Latitude) / 90;
	Ed = Ec * Math::Cos(m_RadLa);
}
//用经纬度构造
JWD::JWD(double longitude, double latitude) 
{
	m_LoDeg =System::Convert::ToInt16(longitude);
	m_LoMin = System::Convert::ToInt16((longitude - m_LoDeg)*60);
	m_LoSec = (longitude - m_LoDeg - m_LoMin/60)*3600;

	m_LaDeg = System::Convert::ToInt16(latitude);
	m_LaMin = System::Convert::ToInt16((latitude - m_LaDeg)*60);
	m_LaSec = (latitude - m_LaDeg - m_LaMin/60)*3600;

	m_Longitude = longitude;
	m_Latitude = latitude;
	m_RadLo = longitude * PI/180;
	m_RadLa = latitude * PI/180;
	Ec = Rj + (Rc - Rj) * (90-m_Latitude) / 90;
	Ed = Ec * Math::Cos(m_RadLa);

}