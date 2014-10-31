#include "stdafx.h"
#include "StringConvector.h"
#include <stdlib.h>
#include <vcclr.h>
#include <math.h>
#include <fstream>
#include <iomanip>
using namespace std;
//string m_i1, m_i2;
//int mr1, mr2;
//StringConvector->To_string(mrid1, m_i1);
//StringConvector->To_string(mrid2, m_i2);
//mr1 = atoi(m_i1.c_str());//atoi把字符串转换成整型数
//mr2 = atoi(m_i2.c_str());
void MarshalString ( String^ s, std::string& os ) 
{
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}
bool To_CharStar(String^ source, char*& target)
{
	pin_ptr<const wchar_t> wch = PtrToStringChars(source);
	int len = ((source->Length + 1) * 2);
	target = new char[len];
	return wcstombs(target, wch, len) != -1;
}
bool To_string(String^ source, std::string &target)
{
	pin_ptr<const wchar_t> wch = PtrToStringChars(source);
	int len = ((source->Length + 1) * 2);
	char *ch = new char[len];
	bool result = wcstombs(ch, wch, len) != -1;
	target = ch;
	delete[]ch;
	return result;
}
bool JudgeNumber(string & Str)
{
	string IStr = Str;
	char min = '0';
	char max = '9';
	int count = IStr.length();
	for (int i = 0; i<count; i++)
	{
		if (IStr[i] <min || IStr[i]> max || IStr[0] == '0')
		{
			return false;
		}
	}
	return true;
}
int  Timediffer(String^ line1, String^ line2)
{
	String^ delimStr = " -.:";
	array<wchar_t>^ delimiter = delimStr->ToCharArray();
	array<String^>^ words1;
	array<String^>^ words2;
	words1 = line1->Split(delimiter);
	words2 = line2->Split(delimiter);
	int t1[4],t2[4];
	for (int i = 3; i < words1->Length; i++)
	{
		string sline1, sline2;
		To_string(words1[i], sline1);
		To_string(words2[i], sline2);
		t1[i - 3] = atoi(sline1.c_str());
		t2[i - 3] = atoi(sline2.c_str());
	}
	int TimeDiffer = ((t2[0] - t1[0]) * 3600 + (t2[1] - t1[1]) * 60 + (t2[2] - t1[2])) * 1000 + (t2[3] - t1[3]);
	return TimeDiffer;
}