#include "StdAfx.h"
#include "LinkDB.h"
using namespace Location;
LinkDB::LinkDB(void)
{
}
//初始化数据库链接，在DataBaseConfig.txt文档中包含登录的用户名和密码
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
		if(ex->Message != "应用程序不可用。")
		{
			Console::WriteLine(ex);
			//MessageBox.Show("请设置数据库连接。");
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
	//连接地区管理的数据库LocateSystem
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
//根据数据库名称链接数据库
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
//根据sql语句在相应数据库里进行调用语句
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
		return reader;//在应用程序中关闭连接和读写reader
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
		return reader;//在应用程序中关闭连接和读写reader
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
		conn->Close();//关闭连接
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
		conn->Close();//关闭连接
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
		conn->Close();//关闭连接
	}
}
//执行sql操作语句将select的结果放在dataset中
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
//执行sql操作语句将select的结果放在datatable中
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
//执行sql操作语句将select的结果放在datatset中，默认数据库名是locatesystem
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
//按照locate.bak的模版创建数据库，以dbname命名
void  LinkDB::Createdb(System::String ^ dbname)
{
	System::String ^ dir = AppDomain::CurrentDomain ->BaseDirectory ;//CurrentDomain.BaseDirectory;
	System::String ^ dir1=nullptr;
	System::String ^dir2=nullptr;
	SqlConnection^ sqlConnection1 = gcnew SqlConnection("Data Source=(local);Integrated Security=SSPI;" + "Initial Catalog=master");
	sqlConnection1->Open();
	//                MessageBox.Show("连接已打开");
	SqlCommand^ cmd = gcnew SqlCommand();
	cmd->Connection = sqlConnection1;
	cmd->CommandTimeout = 50000000;
	SqlCommand^ command = gcnew SqlCommand("select count(*) from sysdatabases where name = '" + dbname + "'", sqlConnection1);

	SqlDataReader^ reader = command->ExecuteReader();

	//                MessageBox.Show("查找gsmCDI语句执行成功");

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
		//                    MessageBox.Show("数据库已建立");
		//查询数据库的文件名
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
		//MessageBox.Show("准备还原数据库");   
		if (dir1 != nullptr && dir2 != nullptr)
		{
			//MessageBox::Show (dir+"----"+dir1+"-----"+dir2);
			cmd->CommandText = "RESTORE DATABASE [" + dbname + "] FROM  DISK = N'" + dir + "Locate.bak' WITH  FILE = 1,MOVE N'Locate' TO N'" + dir1 + "',  MOVE N'Locate_log' TO N'" + dir2 + "',  NOUNLOAD,  REPLACE,  STATS = 10";
			cmd->ExecuteNonQuery();
		}
		else
		{
			MessageBox::Show("数据库创建错误！", "提示", MessageBoxButtons::OK, MessageBoxIcon::Information);
		}
		sqlConnection1->Close();
	}
	else
	{
		System::Windows ::Forms ::DialogResult^ r1 = MessageBox::Show("SQL Server数据库中已有本系统所需数据库" + dbname + "，是否保留该数据库？\n\n点击“是”将保留您以前的数据库，点击“否”您以前的数据库将被清空。", "数据库部署", MessageBoxButtons::YesNo);
		int s1 = System::Convert ::ToInt32 (r1);//(int)r1;
		if (s1 == 7)
		{
			//查询数据库的文件名
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
			//MessageBox.Show("准备还原数据库");   
			if (dir1 != nullptr && dir2 != nullptr)
			{
				cmd->CommandText = "RESTORE DATABASE [" + dbname + "] FROM  DISK = N'" + dir + "Locate.bak' WITH  FILE = 1,MOVE N'Locate' TO N'" + dir1 + "',  MOVE N'Locate_log' TO N'" + dir2 + "',  NOUNLOAD,  REPLACE,  STATS = 10";
				cmd->ExecuteNonQuery();
			}
			else
			{
				MessageBox::Show("数据库创建错误！", "提示", MessageBoxButtons::OK, MessageBoxIcon::Information);
			}
			sqlConnection1->Close();
		}
	}
	//sqlConnection1->Dispose();
}
//删除数据库
void  LinkDB::deletedb(System::String ^ dbname)
{
	SqlConnection^ sqlConnection1 = gcnew SqlConnection("Data Source=(local);Integrated Security=SSPI;" + "Initial Catalog=master");
	sqlConnection1->Open();
	//                MessageBox.Show("连接已打开");
	SqlCommand^ cmd = gcnew SqlCommand();
	cmd->Connection = sqlConnection1;
	cmd->CommandTimeout = 50000000;


	cmd->CommandText ="ALTER DATABASE ["+dbname+"] SET  SINGLE_USER WITH ROLLBACK IMMEDIATE";
	cmd->ExecuteNonQuery ();
	cmd->CommandText ="DROP DATABASE ["+dbname+"]";
	cmd->ExecuteNonQuery ();
	sqlConnection1->Close();;//关闭连接
}