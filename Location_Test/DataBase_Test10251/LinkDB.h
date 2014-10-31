#pragma once
using namespace System;
using namespace System::Data::SqlClient;
using namespace System::IO;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Reflection;
using namespace System::Diagnostics;
using namespace Microsoft::Win32;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


namespace Location {
	ref class LinkDB
	{
	public:
		LinkDB(void);
		static System::String^ source ;
		static System::String^ server;
		static System::String^ user;
		static System::String^ pwd;
		static bool ENABLE = true;
		static bool InitialDataBase();
		System::Data::SqlClient:: SqlConnection^ GetConnection(System::String^ dbname);
		System::Data::SqlClient:: SqlConnection^ GetConnectionProj();
		System::String^ GetConnectionString(System::String^ dbname);
		System::Data::SqlClient::SqlDataReader^   Select(System::String^ strSelect,System::String ^ dbname);
		System::Data::SqlClient::SqlDataReader^ SelectProj(System::String^ strSelect);
		System::Data::SqlClient:: SqlDataAdapter^ FillDataTableProj(System::String^ strSql, System::Data::DataTable^ dataTable,String^ dbname);
		int Modify(System::String^ strSql,System::String^ dbname);
		int ModifyProj(System::String ^ strSql);
		System::Data::SqlClient::SqlDataAdapter^ FillDataSetProj(System::String^ strSql, DataSet^ dataSet,String^ dbname);
		System::Data::SqlClient::SqlDataAdapter^ FillDataSetProj(System::String^ strSql, DataSet^ dataSet);
		void  Createdb(System::String^ dbname);
		void  deletedb(System::String^ dbname);
		void  Close();
		int Count(System::String ^ strSql,System::String ^ dbname);
	private:
		System::Data ::SqlClient :: SqlConnection^ conn;
	};
}