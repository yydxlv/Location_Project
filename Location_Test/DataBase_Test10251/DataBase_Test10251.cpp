// DataBase_Test10251.cpp: ����Ŀ�ļ���

#include "stdafx.h"
#include "MyForm.h"
#pragma once
using namespace System;
using namespace System::Data::SqlClient;
using namespace System::IO;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace Location;
int main(array<System::String ^> ^args)
{
	System::Windows::Forms::Application::EnableVisualStyles();
	System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false);

	// ���������ڲ�������
	Location::MyForm^ f = gcnew Location::MyForm();
	if (f->okk == true)
	 System::Windows::Forms::Application::Run(f);
	return 0;
}
