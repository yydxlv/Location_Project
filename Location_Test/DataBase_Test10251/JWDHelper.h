#pragma once
#include"JWD.h"
using namespace System;
namespace Location{
ref class CJWDHelper
{
	public:
		CJWDHelper(void);
		static double PI=3.1415926;
		//! �����A �� ��B�ľ�γ�ȣ������ǵľ���͵�B����ڵ�A�ķ�λ
		/*! 
		  * \param A A�㾭γ��
		  * \param B B�㾭γ��
		  * \param angle B�����A�ķ�λ, ����Ҫ���ظ�ֵ��������Ϊ��
		  * \return A��B��ľ���
		  */
		static double distance(JWD^ A, JWD^ B, double angle);
		//! �����A �� ��B�ľ�γ�ȣ������ǵľ���͵�B����ڵ�A�ķ�λ
		/*! 
		  * \param longitude1 A�㾭��
		  * \param latitude1 A��γ��
		  * \param longitude2 B�㾭��
		  * \param latitude2 B��γ��
		  * \param angle B�����A�ķ�λ, ����Ҫ���ظ�ֵ��������Ϊ��
		  * \return A��B��ľ���
		  */
		static double distance(
			double longitude1, double latitude1,double longitude2, double latitude2, double angle);
		//! ��֪��A��γ�ȣ�����B���A��ľ��룬�ͷ�λ����B��ľ�γ��
		/*!
		  * \param A ��֪��A
		  * \param distance B�㵽A��ľ��� 
		  * \param angle B�������A��ķ�λ
		  * \return B��ľ�γ������
		  */
		static JWD^ GetJWDB(JWD^ A, double distance, double angle);
		//! ��֪��A��γ�ȣ�����B���A��ľ��룬�ͷ�λ����B��ľ�γ��
		/*!
		  * \param longitude ��֪��A����
		  * \param latitude ��֪��Aγ��
		  * \param distance B�㵽A��ľ��� 
		  * \param angle B�������A��ķ�λ
		  * \return B��ľ�γ������
		  */
		static JWD^ GetJWDB(double longitude, double latitude, double distance, double angle);
		static double angle(double longitude1, double latitude1, double longitude2, double latitude2) ;
		static double angle(Location::JWD^ A, Location::JWD^ B);
		static double distance1(double longitude1, double latitude1, double longitude2, double latitude2, double& angle) ;
		static double distance1(Location::JWD^ A, Location::JWD^ B, double& angle);


};
}