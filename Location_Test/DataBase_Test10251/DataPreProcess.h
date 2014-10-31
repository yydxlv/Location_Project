#pragma once
#include"LinkDB.h";
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Text;
namespace Location
{
	ref class DataProcess
	{

	public:
		DataProcess(String^ dbname,String^ tablename);
		System::String ^dbname;
		System::String ^tablename;
		bool MRRxlevSort();
		bool InsertValue2(ArrayList^ target,int index,int &mm,double average, double diff);
		bool SmoothFilter1( ArrayList^ value, int &mm);
		bool FillCallID(String^ selectTable,String^ selectTarget,ArrayList^value) ;
		bool FillMRJWD();
		bool FillMRTimeID();
		bool Measure_QAH_CD();
		bool Measure_QOH_CD();
		bool Measure_Group_Combina_CD();
		bool Measure_SameS_SameA_CD();
		bool HandOver();
		bool MREffectiveCheck();
		bool SmothMR();
		bool computespeed();
	};
}