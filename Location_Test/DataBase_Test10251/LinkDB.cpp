#include "StdAfx.h"
#include "LinkDB.h"
using namespace Location;
LinkDB::LinkDB(void)
{
}
//��ʼ�����ݿ����ӣ���DataBaseConfig.txt�ĵ��а�����¼���û���������
bool LinkDB::InitialDataBase()
{
	System::IO ::StreamReader^ fin = nullptr;
	//OpenFileDialog^ openDlg = gcnew OpenFileDialog();
	try
	{

		fin = File::OpenText (System::AppDomain::CurrentDomain->BaseDirectory +"DataBaseConfig.txt");
		System::String ^line;
		while ((line = fin->ReadLine()))
		{
			//System::Collections::ArrayList^ param = gcnew System::Collections::ArrayList();// line->Split('=');
			//System::Array <String^>^dd= line->Split('=');
			cli::array <System::String^>^param ; 
			param=line->Split('=');	
			if(param[0] == "server")
				server = param[1];
			if(param[0] == "user")
				user = param[1];
			if(param[0] == "password")
				pwd = param[1];
		}
		fin->Close();
		return true;
	}
	catch(Exception^ ex)
	{
		if(ex->Message != "Ӧ�ó��򲻿��á�")
		{
			Console::WriteLine(ex);
			//MessageBox.Show("���������ݿ����ӡ�");
		}
		else
		{
			MessageBox::Show(ex->Message); 
		}

		if(fin )
			fin->Close();
		return false;
	}
}
System::Data::SqlClient::SqlConnection^ LinkDB::GetConnection(System::String^ dbname)
{
	if (server->Empty && user->Empty && pwd->Empty)
	{
			server="localhost";
			user="null";
			pwd="null";
	}
	try
	{
		//string dbname = "gsmCDI_" + area;

		if (server == "localhost" && user == "null" && pwd == "null")
			source = "data source=(local);integrated security=SSPI;initial catalog="+dbname+";Max Pool Size = 1024";

		if (server != "localhost" && user == "null" && pwd == "null")
			source = "data source=" + server + ";integrated security=SSPI;initial catalog=" + dbname + ";Max Pool Size = 1024";

		if (server == "localhost" && user != "null" && pwd != "null")
			source = "data source=(local);user id=" + user + ";password =" + pwd + ";initial catalog=" + dbname + ";Max Pool Size = 1024";

		if (server != "localhost" && user != "null" && pwd != "null")
			source = "data source=" + server + ";user id=" + user + ";password =" + pwd + ";initial catalog=" + dbname + ";Max Pool Size = 1024";

		conn = gcnew SqlConnection(source);  
		conn->Open();
		return conn;
	}
	catch (Exception^ ex)
	{
		throw ex;
		return nullptr;
	}
}
System::Data ::SqlClient :: SqlConnection^ LinkDB::GetConnectionProj()
{
	//���ӵ�����������ݿ�LocateSystem
	try
	{
		if (server == "localhost" && user == "null" && pwd == "null")
			source = "data source=(local);integrated security=SSPI;initial catalog=LocateSystem;Max Pool Size = 1024";

		if (server != "localhost" && user == "null" && pwd == "null")
			source = "data source=" + server + ";integrated security=SSPI;initial catalog=LocateSystem;Max Pool Size = 1024";

		if (server == "localhost" && user != "null" && pwd != "null")
			source = "data source=(local);user id=" + user + ";password =" + pwd + ";initial catalog=LocateSystem;Max Pool Size = 1024";

		if (server != "localhost" && user != "null" && pwd != "null")
			source = "data source=" + server + ";user id=" + user + ";password =" + pwd + ";initial catalog=LocateSystem;Max Pool Size = 1024";

		conn = gcnew SqlConnection(source);  

		conn->Open();
		return conn;
	}
	catch (Exception^ ex)
	{
		throw ex;
		return nullptr;
	}
}
//�������ݿ������������ݿ�
System::String^ LinkDB::GetConnectionString(System::String ^ dbname)
{
	try
	{
		if (server == "localhost" && user == "null" && pwd == "null")
			source = "data source=(local);integrated security=SSPI;initial catalog="+dbname+";Max Pool Size = 1024";

		if (server != "localhost" && user == "null" && pwd == "null")
			source = "data source=" + server + ";integrated security=SSPI;initial catalog=" + dbname + ";Max Pool Size = 1024";

		if (server == "localhost" && user != "null" && pwd != "null")
			source = "data source=(local);user id=" + user + ";password =" + pwd + ";initial catalog=" + dbname + ";Max Pool Size = 1024";

		if (server != "localhost" && user != "null" && pwd != "null")
			source = "data source=" + server + ";user id=" + user + ";password =" + pwd + ";initial catalog=" + dbname + ";Max Pool Size = 1024";

		//conn = new SqlConnection(source);
		//conn.Open();
		return source;
	}
	catch (Exception^ ex)
	{
		throw ex;
		return nullptr;
	}
}
//����sql�������Ӧ���ݿ�����е������
System::Data ::SqlClient ::SqlDataReader^   LinkDB::Select(System::String ^ strSelect,System::String ^ dbname)
{
	try
	{
		//SqlConnection conn = GetConnection();
		conn = GetConnection(dbname);
		//conn.Open();
		SqlCommand^ cmd = gcnew SqlCommand();
		cmd->CommandText = strSelect;
		cmd->CommandTimeout = 50000;
		cmd->Connection = conn;
		SqlDataReader^ reader = cmd->ExecuteReader(CommandBehavior::CloseConnection);
		return reader;//��Ӧ�ó����йر����ӺͶ�дreader
	}
	catch (SqlException^ ex)
	{
		throw ex;
		return nullptr;
	}
}
System::Data ::SqlClient ::SqlDataReader^ LinkDB::SelectProj(System::String ^ strSelect)
{
	try
	{
		//SqlConnection conn = GetConnection();
		conn = GetConnectionProj();
		//conn.Open();
		SqlCommand^ cmd = gcnew SqlCommand();
		cmd->CommandText = strSelect;
		cmd->Connection = conn;
		SqlDataReader^ reader = cmd->ExecuteReader(CommandBehavior::CloseConnection);
		return reader;//��Ӧ�ó����йر����ӺͶ�дreader
	}
	catch (SqlException^ ex)
	{
		throw ex;
		return nullptr;
	}
}
void  LinkDB::Close()
{
	try
	{
		this->conn->Close();
		//conn->Dispose();
		//conn->Dispose ();

	}
	catch (Exception^ ex)
	{
		throw ex;
	}

}
int LinkDB::Modify(System::String ^ strSql,System::String ^ dbname)
{
	try
	{
		conn = GetConnection(dbname);
		//conn.Open();
		SqlCommand^ cmd = gcnew SqlCommand();
		cmd->CommandTimeout = 60000;
		cmd->CommandText = strSql;
		cmd->Connection = conn;
		int rowsAffected = cmd->ExecuteNonQuery();
		return rowsAffected;
	}
	catch (SqlException^ ex)
	{
		throw ex;
		return -1;
	}
	finally
	{
		conn->Close();//�ر�����
	}
}
int LinkDB::Count(System::String ^ strSql,System::String ^ dbname)
{
	try
	{
		conn = GetConnection(dbname);
		//conn.Open();
		SqlCommand^ cmd = gcnew SqlCommand();
		cmd->CommandTimeout = 50000;
		cmd->CommandText = strSql;
		cmd->Connection = conn;
		int rowsAffected = System::Convert ::ToInt32  (cmd->ExecuteScalar ());
		return rowsAffected;
	}
	catch (SqlException^ ex)
	{
		throw ex;
		return -1;
	}
	finally
	{
		conn->Close();//�ر�����
	}
}

int LinkDB::ModifyProj(System::String ^ strSql)
{
	try
	{

		conn = GetConnectionProj();
		//conn.Open();
		SqlCommand^ cmd = gcnew SqlCommand();
		cmd->CommandTimeout = 50000;
		cmd->CommandText = strSql;
		cmd->Connection = conn;
		int rowsAffected = cmd->ExecuteNonQuery();

		return rowsAffected;
	}
	catch (SqlException^ ex)
	{
		throw ex;
		return -1;
	}
	finally
	{
		conn->Close();//�ر�����
	}
}
//ִ��sql������佫select�Ľ������dataset��
System::Data ::SqlClient :: SqlDataAdapter^ LinkDB::FillDataSetProj(System::String ^ strSql, DataSet^ dataSet,String^dbname)
{

	try
	{
		conn = GetConnection(dbname);
		SqlDataAdapter^ adapter = gcnew SqlDataAdapter(strSql, conn);
		adapter->Fill(dataSet);
		return adapter;
	}
	catch (SqlException^ ex)
	{
		throw ex;
		return nullptr;
	}
}
//ִ��sql������佫select�Ľ������datatable��
System::Data ::SqlClient :: SqlDataAdapter^ LinkDB::FillDataTableProj(System::String ^ strSql, System::Data::DataTable^ dataTable,String^dbname)
{

	try
	{
		conn = GetConnection(dbname);
		SqlDataAdapter^ adapter = gcnew SqlDataAdapter(strSql, conn);
		adapter->Fill(dataTable);
		return adapter;
	}
	catch (SqlException^ ex)
	{
		throw ex;
		return nullptr;
	}
}
//ִ��sql������佫select�Ľ������datatset�У�Ĭ�����ݿ�����locatesystem
System::Data ::SqlClient :: SqlDataAdapter^ LinkDB::FillDataSetProj(System::String ^ strSql, DataSet^ dataSet)
{

	try
	{
		conn = GetConnectionProj();
		SqlDataAdapter^ adapter = gcnew SqlDataAdapter(strSql, conn);
		adapter->Fill(dataSet);
		return adapter;
	}
	catch (SqlException^ ex)
	{
		throw ex;
		return nullptr;
	}
}
//����locate.bak��ģ�洴�����ݿ⣬��dbname����
void  LinkDB::Createdb(System::String ^ dbname)
{
	System::String ^ dir = AppDomain::CurrentDomain ->BaseDirectory ;//CurrentDomain.BaseDirectory;
	System::String ^ dir1=nullptr;
	System::String ^dir2=nullptr;
	SqlConnection^ sqlConnection1 = gcnew SqlConnection("Data Source=(local);Integrated Security=SSPI;" + "Initial Catalog=master");
	sqlConnection1->Open();
	//                MessageBox.Show("�����Ѵ�");
	SqlCommand^ cmd = gcnew SqlCommand();
	cmd->Connection = sqlConnection1;
	cmd->CommandTimeout = 50000000;
	SqlCommand^ command = gcnew SqlCommand("select count(*) from sysdatabases where name = '" + dbname + "'", sqlConnection1);

	SqlDataReader^ reader = command->ExecuteReader();

	//                MessageBox.Show("����gsmCDI���ִ�гɹ�");

	// Call Read before accessing dat
	int count = 99;
	while (reader->Read())
	{
		count = Convert::ToInt32(reader[0]);
	}

	reader->Close();
	if (count == 0)
	{
		cmd->CommandText = "create database " + dbname;
		cmd->ExecuteNonQuery();
		//                    MessageBox.Show("���ݿ��ѽ���");
		//��ѯ���ݿ���ļ���
		cmd->CommandText = "select filename from [" + dbname + "]..sysfiles";//where charindex('LDF',filename)>0";
		SqlDataReader^ readername2 = cmd->ExecuteReader();
		if (readername2->HasRows)
		{
			while (readername2->Read())
			{
				if (readername2[0]->ToString()->EndsWith(".mdf"))
					dir1 = readername2[0]->ToString();
				else if (readername2[0]->ToString()->EndsWith(".LDF"))
					dir2 = readername2[0]->ToString();
			}
		}
		readername2->Close();
		//MessageBox.Show("׼����ԭ���ݿ�");   
		if (dir1 != nullptr && dir2 != nullptr)
		{
			//MessageBox::Show (dir+"----"+dir1+"-----"+dir2);
			cmd->CommandText = "RESTORE DATABASE [" + dbname + "] FROM  DISK = N'" + dir + "Locate.bak' WITH  FILE = 1,MOVE N'Locate' TO N'" + dir1 + "',  MOVE N'Locate_log' TO N'" + dir2 + "',  NOUNLOAD,  REPLACE,  STATS = 10";
			cmd->ExecuteNonQuery();
		}
		else
		{
			MessageBox::Show("���ݿⴴ������", "��ʾ", MessageBoxButtons::OK, MessageBoxIcon::Information);
		}
		sqlConnection1->Close();
	}
	else
	{
		System::Windows ::Forms ::DialogResult^ r1 = MessageBox::Show("SQL Server���ݿ������б�ϵͳ�������ݿ�" + dbname + "���Ƿ��������ݿ⣿\n\n������ǡ�����������ǰ�����ݿ⣬�����������ǰ�����ݿ⽫����ա�", "���ݿⲿ��", MessageBoxButtons::YesNo);
		int s1 = System::Convert ::ToInt32 (r1);//(int)r1;
		if (s1 == 7)
		{
			//��ѯ���ݿ���ļ���
			cmd->CommandText = "select filename from [" + dbname + "]..sysfiles";//where charindex('LDF',filename)>0";
			SqlDataReader^ readername2 = cmd->ExecuteReader();
			if (readername2->HasRows)
			{
				while (readername2->Read())
				{
					if (readername2[0]->ToString()->EndsWith(".mdf"))
						dir1 = readername2[0]->ToString();
					else if (readername2[0]->ToString()->EndsWith(".LDF"))
						dir2 = readername2[0]->ToString();
				}
			}
			readername2->Close();
			//MessageBox.Show("׼����ԭ���ݿ�");   
			if (dir1 != nullptr && dir2 != nullptr)
			{
				cmd->CommandText = "RESTORE DATABASE [" + dbname + "] FROM  DISK = N'" + dir + "Locate.bak' WITH  FILE = 1,MOVE N'Locate' TO N'" + dir1 + "',  MOVE N'Locate_log' TO N'" + dir2 + "',  NOUNLOAD,  REPLACE,  STATS = 10";
				cmd->ExecuteNonQuery();
			}
			else
			{
				MessageBox::Show("���ݿⴴ������", "��ʾ", MessageBoxButtons::OK, MessageBoxIcon::Information);
			}
			sqlConnection1->Close();
		}
	}
	//sqlConnection1->Dispose();
}
//ɾ�����ݿ�
void  LinkDB::deletedb(System::String ^ dbname)
{
	SqlConnection^ sqlConnection1 = gcnew SqlConnection("Data Source=(local);Integrated Security=SSPI;" + "Initial Catalog=master");
	sqlConnection1->Open();
	//                MessageBox.Show("�����Ѵ�");
	SqlCommand^ cmd = gcnew SqlCommand();
	cmd->Connection = sqlConnection1;
	cmd->CommandTimeout = 50000000;


	cmd->CommandText ="ALTER DATABASE ["+dbname+"] SET  SINGLE_USER WITH ROLLBACK IMMEDIATE";
	cmd->ExecuteNonQuery ();
	cmd->CommandText ="DROP DATABASE ["+dbname+"]";
	cmd->ExecuteNonQuery ();
	sqlConnection1->Close();;//�ر�����
}