using namespace System;
#pragma once
namespace Location
{
	ref class JWD
	{
	public:
		JWD(void);
		double m_LoDeg, m_LoMin, m_LoSec;  // longtitude ����
		double m_LaDeg, m_LaMin, m_LaSec;
		double m_Longitude, m_Latitude;
		double m_RadLo, m_RadLa;
		double Ec;
		double Ed;
		static double Rc = 6378137;  // ����뾶
		static double Rj = 6356725;  // ���뾶
		static double PI=3.1415926;
		JWD(double loDeg, double loMin, double loSec, double laDeg, double laMin, double laSec);
		JWD(double longitude, double latitude);
	};
}
