#include "StdAfx.h"
#include "DataPreProcess.h"
#include "JWDHelper.h"
using namespace Location;
DataProcess::DataProcess(String^ dbname,String^ tablename)
{
	this->dbname =dbname;
	this->tablename =tablename;			
}
//将callid填充到数组中
bool DataProcess::FillCallID(String^ selectTable,String^ selectTarget,ArrayList^value) 
{
		LinkDB^ link=gcnew LinkDB();
		try
		{
				String^ commandtext = "";			
				commandtext = " select distinct "+ selectTarget+" from "+ selectTable;
				DataSet^ dataset = gcnew DataSet();
				SqlDataAdapter^ adapter = link->FillDataSetProj (commandtext,dataset,dbname);
				int num = dataset->Tables [0]->Rows ->Count;
				for(int i=0;i<num ;i++)
				{
					//index->Add (dataset->Tables [0]->Rows[i]["rowid"]);
					DataRow^ DataRowTest = dataset->Tables[0]->Rows[i];
					value->Add(DataRowTest[selectTarget]);
				}						
			return true;
		}
		catch(Exception^ ex)
		{
			throw ex;
		}

}
//////////////////////////////////新方案///////////////////////////////
//插入MRID的经纬度,添加available字段，标识该条MR属于那种类型（文档中的3类MR）包含TCH校正
bool DataProcess::FillMRJWD()
{
	try
	{
		LinkDB^ link = gcnew LinkDB();
		String ^str0 = "if exists(select 1 from sysobjects where name='temp1') drop table temp1" ;
		link->Modify(str0, this->dbname );
		String^str1="select  *  into   temp1   from "+tablename+"  order by MRID;"
					//+"alter table temp1 ADD Available int null;"
					+"update temp1 set Available = 1;"
					+"update temp1 set Available = 0  where"
					+"( LAC1!=-1 and LAC2!=-1 and LAC3!=-1 and LAC4!=-1 and LAC5!=-1 and LAC6!=-1 and LAC1!=0 and LAC2!=0 and LAC3!=0 and LAC4!=0 and LAC5!=0 and LAC6!=0);";
		link->Modify(str1,dbname);
		String^ fillLL="update temp1 set longitude=tbMRLocation.longitude,latitude=tbMRLocation.latitude from temp1, tbMRLocation where tbMRLocation.mrid=temp1.mrid";
		link->Modify (fillLL,dbname);

		String^ count = "select count(*) from temp1 ";
		int num,num2,num1,maxpage=65535;///应该类似于定制内存表的最大长度吧，然后将待处理的表分页。
		num2=num = link->Count(count, dbname);
		DataSet^ ds= gcnew DataSet();
		DataSet^ds2= gcnew DataSet();
		ArrayList^ CallID=gcnew ArrayList();
		this->FillCallID ("temp1","CallID",CallID);
		for(int i=0;i<=num2/maxpage;i++)
		{
			ds->Clear ();
			ds2->Clear ();
			String^ str2 = "select  * from temp1 where  MRID>"+System::Convert ::ToString (i*maxpage)+" and MRID<"+System::Convert ::ToString (i*maxpage+maxpage+1)+" order by MRID";
			//String^ str3 = "select  * from tbMRLocation where MRID>"+System::Convert ::ToString (i*maxpage)+" and MRID<"+System::Convert ::ToString (i*maxpage+maxpage+1)+" order by MRID";					
			SqlDataAdapter^ adpter = link->FillDataSetProj (str2,ds,dbname );
			for(int j=0;j<ds->Tables [0]->Rows ->Count ;j++ )
			{
				int counterror=0;
				DataRow^ DataRowTest = ds->Tables[0]->Rows[j];
				if (Convert::ToInt32(DataRowTest["LAC1"]) == -1 || Convert::ToInt32(DataRowTest["LAC1"]) == 0 || Convert::ToInt32(DataRowTest["CI1"]) == -1)
					counterror=counterror+1;
				if (Convert::ToInt32(DataRowTest["LAC2"]) == -1 || Convert::ToInt32(DataRowTest["LAC2"]) == 0 || Convert::ToInt32(DataRowTest["CI2"]) == -1)
					counterror=counterror+1;
				if (Convert::ToInt32(DataRowTest["LAC3"]) == -1 || Convert::ToInt32(DataRowTest["LAC3"]) == 0 || Convert::ToInt32(DataRowTest["CI3"]) == -1)
					counterror=counterror+1;
				if (Convert::ToInt32(DataRowTest["LAC4"]) == -1 || Convert::ToInt32(DataRowTest["LAC4"]) == 0 || Convert::ToInt32(DataRowTest["CI4"]) == -1)
					counterror=counterror+1;
				if (Convert::ToInt32(DataRowTest["LAC5"]) == -1 || Convert::ToInt32(DataRowTest["LAC5"]) == 0 || Convert::ToInt32(DataRowTest["CI5"]) == -1)
					counterror=counterror+1;
				if (Convert::ToInt32(DataRowTest["LAC6"]) == -1 || Convert::ToInt32(DataRowTest["LAC6"]) == 0 || Convert::ToInt32(DataRowTest["CI6"]) == -1)
					counterror=counterror+1;
				if(counterror>=3)
					DataRowTest["Available"] = 2;
				//插补时会用到经纬度插值，所以要都填入，不能有空
				if (DataRowTest["longitude"]->ToString() == "")
				{
					DataRow^ DataRowTest1 = ds->Tables[0]->Rows[j-1];
					DataRowTest["longitude"] = DataRowTest1["longitude"];
					DataRowTest["latitude"]  = DataRowTest1["latitude"];
				}
			}
			if(i==0)
			{
				String ^sqlcopy1 = "delete from original_mr_by_wukai_copy;" ;
				link->Modify(sqlcopy1, this->dbname  );
			}
			String^  getConnectString = link->GetConnectionString(dbname);
			Data::SqlClient::SqlBulkCopy^  bcp = gcnew Data::SqlClient::SqlBulkCopy(getConnectString);
			Data::DataTable^  dtTemp = gcnew  Data::DataTable();
			Data::SqlClient::SqlConnection::ClearAllPools();			
			bcp->DestinationTableName = "original_mr_by_wukai_copy ";
			dtTemp = ds->Tables [0];
			bcp->BatchSize = dtTemp->Rows->Count;
			for (int  k = 0; k < dtTemp->Columns->Count; k++)
			{
				String^  ColumnName = dtTemp->Columns[k]->ColumnName;
				Data::SqlClient ::SqlBulkCopyColumnMapping^ columnMap = gcnew Data::SqlClient ::SqlBulkCopyColumnMapping(ColumnName, ColumnName);
				bcp->ColumnMappings->Add(columnMap);
			}					
			bcp->WriteToServer(dtTemp);
			bcp->Close();
		}
		
		String ^sqldelete = "drop table temp1" ;
		link->Modify(sqldelete, this->dbname);
		return true;
	}
	catch(Exception^ex)
	{
		throw ex;
		return false;
	}
	finally
	{

	}
}
//插入timeid，取消按行号插入，对后续会有影响
bool DataProcess::FillMRTimeID()
{
		LinkDB^ link = gcnew LinkDB();
		DataSet^ ds= gcnew DataSet();
		ds->Clear ();
		String^ str2 = "select  * from original_mr_by_wukai_copy  order by Time";
		SqlDataAdapter^ adpter = link->FillDataSetProj (str2,ds,dbname );
		int timeid = 1;
		for(int j=0;j<ds->Tables [0]->Rows ->Count ;j++ )
		{
			DataRow^ DataRowTest = ds->Tables[0]->Rows[j];
			DataRowTest["TimeID"] = timeid;
			timeid++;
		}
		String ^sqlcopy1 = "delete from original_mr_by_wukai_copy;" ;
		link->Modify(sqlcopy1, this->dbname  );
			
		String^  getConnectString = link->GetConnectionString(dbname);
		Data::SqlClient::SqlBulkCopy^ bcp = gcnew Data::SqlClient::SqlBulkCopy(getConnectString);
		Data::DataTable^ dtTemp = gcnew  Data::DataTable();
		Data::SqlClient::SqlConnection::ClearAllPools();
		bcp->DestinationTableName = "original_mr_by_wukai_copy ";
		dtTemp = ds->Tables [0];
		bcp->BatchSize = dtTemp->Rows->Count;
		for (int  k = 0; k < dtTemp->Columns->Count; k++)
		{
			String^  ColumnName = dtTemp->Columns[k]->ColumnName;
			Data::SqlClient::SqlBulkCopyColumnMapping^ columnMap = gcnew Data::SqlClient::SqlBulkCopyColumnMapping(ColumnName, ColumnName);
			bcp->ColumnMappings->Add(columnMap);
		}	
		bcp->WriteToServer(dtTemp);
		bcp->Close();
		String^ss1="update original_mr_by_wukai_copy set original_mr_by_wukai_copy.siteid=tbcell.siteid from tbcell,original_mr_by_wukai_copy where tbcell.cellid=original_mr_by_wukai_copy.cellid ";
		link->Modify(ss1, this->dbname);
		/*String ^sqldelete = "drop table temp1" ;
		link->Modify(sqldelete, this->dbname );*/
		return true;
}
//插补规则1
bool DataProcess::Measure_QAH_CD()
{
	/*try
	{*/
			
			dbname = "Locate_成都";		
			LinkDB^ linkdb=gcnew LinkDB();
			Data::SqlClient::SqlConnection^ conn = linkdb->GetConnection(dbname);//连接到数据库

			int window = 3;			
			//导入的临时表中
			String ^str0 = "if exists(select 1 from sysobjects where name='temp1') drop table temp1" ;
			linkdb->Modify(str0, this->dbname);
//			String^str1="select  * into   temp1   from tbMR  order by Time;";//武凯修改
			String^str1="select  * into   temp1   from original_mr_by_wukai_copy  order by Time;";
			linkdb->Modify(str1,dbname);
			//提取所有不同的callid
			String^ select1="select distinct callid from temp1";
			System::Data::DataTable^ dt1= gcnew System::Data::DataTable();
			linkdb->FillDataTableProj(select1,dt1,this->dbname );
			//建立临时内存表
			System::Data::DataTable^ dtable1 = gcnew System::Data::DataTable();
			dtable1->Columns->Add("MRID");
			dtable1->Columns->Add("LAC");
			dtable1->Columns->Add("CI");
			dtable1->Columns->Add("Rxlev");
			dtable1->Columns->Add("Distance");
			dtable1->Columns->Add ("Frequence");
			System::Data::DataTable^ dtable2 = gcnew System::Data::DataTable();
			dtable2->Columns->Add("MRID");
			dtable2->Columns->Add("LAC");
			dtable2->Columns->Add("CI");
			dtable2->Columns->Add("Rxlev");
			dtable2->Columns->Add("Distance");
			dtable2->Columns->Add ("Frequence");
			System::Data::DataTable^ dtable3 = gcnew System::Data::DataTable();
			dtable3->Columns->Add("LAC");
			dtable3->Columns->Add("CI");
			dtable3->Columns->Add("Rxlev");
			dtable3->Columns->Add("Distance");
			dtable3->Columns->Add ("Frequence");
			System::Data::DataTable^ dtable4 = gcnew System::Data::DataTable();
			dtable4->Columns->Add("LAC");
			dtable4->Columns->Add("CI");
			dtable4->Columns->Add("Rxlev");
			dtable4->Columns->Add("Distance");
			dtable4->Columns->Add ("Frequence");
			for (int i=0;i<dt1->Rows ->Count ;i++)
			{
				DataRow^ DataRowTest1 = dt1->Rows[i];//callid 注意VS2005与VS2013的不兼容之处
				//找出其中一个callid的所有MR
				String^select2="select * from temp1 where callid = "+DataRowTest1["CallID"]+" order by time";//在temp1中找出所有等于dt1[i]callid的所有mr（行），按time递增排列
				System::Data ::DataTable ^dt2= gcnew System::Data::DataTable();
				linkdb->FillDataTableProj(select2,dt2,this->dbname );
				if(dt2->Rows->Count>1)//按time递增，对应一个唯一callid下的所有mr。个人感觉，这是程序强健性的体现。武凯注。
				{
					for(int j=0;j<dt2->Rows ->Count ;j++)
					{
						DataRow^ DataRowTest2 = dt2->Rows[j];//同一callid下的MR
						if(j==0)
						{
							ArrayList^ mrlacci=gcnew ArrayList();							
							ArrayList^ misslacci= gcnew ArrayList();
							for(int k=1;k<=6;k++)//检查dt2第j行的6个邻小区，是否有需要插补的
							{
								if (Convert::ToInt32(DataRowTest2["LAC" + k]) == -1 || Convert::ToInt32(DataRowTest2["LAC" + k]) == 0 || Convert::ToInt32(DataRowTest2["CI" + k]) == -1 || Convert::ToInt32(DataRowTest2["CI" + k]) == 0)
									misslacci->Add (k);
								else
								{
									String^ laccik = DataRowTest2["LAC" + k] + "_" + DataRowTest2["CI" + k];
									if(!mrlacci->Contains (laccik))
										mrlacci->Add (laccik);
								}
							}//此模块完成后，misslacci中存储的是缺失的小区“顺序号”，mrlacci存储的是不需插补的小区lac和ci
							//填充后4条数据		
							if(misslacci->Count >0)
							{
								dtable2->Clear();
								for(int h=j+1; h<=j+window&&h<dt2->Rows ->Count;h++)//j是mr的序列号
								{
									DataRow^ DataRowTest3 = dt2->Rows[h];
									if(h>=0)
									{
										for(int k=1;k<=6;k++)
										{
											if (Convert::ToInt32(DataRowTest3["LAC" + k]) != -1 && Convert::ToInt32(DataRowTest3["LAC" + k]) != 0)
											{
												String^ laccik = DataRowTest3["LAC" + k] + "_" + DataRowTest3["CI" + k];
												if(!mrlacci->Contains (laccik))//检验相邻的mr中的有效邻小区是否已经存在于本条mr的小区中。
												{//fromrlacci->Add (laccik);
													System::Data::DataRow^ thisrow = dtable2->NewRow();
													thisrow["MRID"] = DataRowTest3["MRID"];
													thisrow["LAC"] = DataRowTest3["LAC" + k];
													thisrow["CI"] = DataRowTest3["CI" + k];
													thisrow["Rxlev"] = DataRowTest3["RxlevNCell" + k];
													thisrow["Distance"] = Convert::ToInt32 (h-j);
													thisrow["Frequence"] = 0;
													dtable2->Rows->Add(thisrow);
												}//end if
											}//end if
										}//end for
									}//end if
								}//end for
								//dtable2存储的是窗口内的，所有本条mr没有的小区，包括mrid,lacX,ciX,relexX,距离,频率(0)。其中，可以用相同重复的小区lac,ci
								if(dtable2->Rows ->Count >0)
								{
									for(int h=0;h<dtable2->Rows ->Count ;h++)//先遍历一下，统计备选小区出现频次
									{
										DataRow^ DataRow2 = dtable2->Rows[h];
										int lac = Convert::ToInt32(DataRow2["LAC"]);
										int ci = Convert::ToInt32(DataRow2["CI"]);
										for(int k=0;k<dtable2->Rows ->Count ;k++)
										{
											DataRow^ DataRow2k = dtable2->Rows[k];
											int lac1 = Convert::ToInt32(DataRow2k["LAC"]);
											int ci1 = Convert::ToInt32(DataRow2k["CI"]);
											if(lac1==lac&&ci1==ci)
											{
												DataRow2["Frequence"] = Convert::ToInt32(Convert::ToInt32(DataRow2["Frequence"]) + 1);
											}
										}
						
									}
									//计算结果
									DataView^ dv = 	dtable2->DefaultView; 
									dv->Sort = "LAC Desc,CI Desc";//！！！按LACCI的顺序递减排列是很有必要的，因为下面都是统计出属于一个LACCI的平均接收电平和出现频率！！！！
									dtable2 = dv->ToTable();
									bool jisuan=false;
									DataRow^ DataRow0 = dtable2->Rows[0];
									int lac = Convert::ToInt32(DataRow0["LAC"]);
									int ci = Convert::ToInt32(DataRow0["CI"]);
									double rxlev=0;
									double num = Convert::ToInt32(DataRow0["Frequence"]);
									dtable4->Clear ();
									//下面的循环，dtable4存的是没有重复的lac,ci,平均Relex,出现频率
									for(int h=0;h<dtable2->Rows->Count  ;h++)
									{
										DataRow^ DataRow2h = dtable2->Rows[h];
										if (Convert::ToInt32(DataRow2h["LAC"]) == lac&&Convert::ToInt32(DataRow2h["CI"]) == ci)
											rxlev = rxlev + Convert::ToDouble(DataRow2h["Rxlev"]);
										else
										{
											DataRow^ DataRow2h_1 = dtable2->Rows[h-1];
											System::Data::DataRow^ thisrow = dtable4->NewRow();
											thisrow["LAC"] = lac;
											thisrow["CI"] = ci;
											thisrow["Rxlev"] =Convert::ToInt32 (rxlev/num);
											//thisrow["Distance"] =dtable3->Rows [k]["Distance"];
											thisrow["Frequence"] = DataRow2h_1["Frequence"];
											dtable4->Rows->Add(thisrow);

											lac = Convert::ToInt32(DataRow2h["LAC"]);
											ci = Convert::ToInt32(DataRow2h["CI"]);
											rxlev = Convert::ToInt32(DataRow2h["Rxlev"]);
											num = Convert::ToInt32(DataRow2h["Frequence"]);
										}
										if(h==dtable2->Rows->Count -1)
										{
											System::Data::DataRow^ thisrow = dtable4->NewRow();
											thisrow["LAC"] = lac;
											thisrow["CI"] = ci;
											thisrow["Rxlev"] =Convert::ToInt32 (rxlev/num);
											//thisrow["Distance"] =dtable3->Rows [k]["Distance"];
											thisrow["Frequence"] = DataRow2h["Frequence"];
											dtable4->Rows->Add(thisrow);
										}
									}
									DataView^ dv1 = dtable4->DefaultView; 
									dv1->Sort = "Frequence Desc, Rxlev Desc";//当然，此时就要按出现频率递减排列了；若出现频率相同，则按电平递减选择
									dtable4 = dv1->ToTable();
									//修改值该条MR的空缺字段
									int num1=dtable4->Rows ->Count;
									//,misslacci->Count );
									for(int h=0;h<num1 ;h++)
									{	
										DataRow^ DataRow2j = dt2->Rows[j];
										DataRow^ DataRow4 = dtable4->Rows[h];
										if (misslacci->Count != 0 && !mrlacci->Contains(DataRow4["LAC"] + "_" + DataRow4["CI"]))//多次、反复条件判断！！
										{
											DataRow2j["LAC" + Convert::ToString(misslacci[0])] = DataRow4["LAC"];
											DataRow2j["CI" + Convert::ToString(misslacci[0])] = DataRow4["CI"];
											DataRow2j["RxlevNCell" + Convert::ToString(misslacci[0])] = DataRow4["Rxlev"];
										
										misslacci->Remove (misslacci[0]);
										}
									}
								}
							}
						}//end j==0
						if(j==dt2->Rows->Count-1)
						{
							ArrayList^ mrlacci=gcnew ArrayList();
							ArrayList^ misslacci= gcnew ArrayList();
							for(int k=1;k<=6;k++)
							{
								DataRow^ DataRow2j = dt2->Rows[j];
								if (Convert::ToInt32(DataRow2j["LAC" + k]) == -1 || Convert::ToInt32(DataRow2j["LAC" + k]) == 0 || Convert::ToInt32(DataRow2j["CI" + k]) == -1 || Convert::ToInt32(DataRow2j["CI" + k]) == 0)
									misslacci->Add (k);
								else
								{
									String^ laccik = DataRow2j["LAC" + k] + "_" + DataRow2j["CI" + k];
									if(!mrlacci->Contains (laccik))
										mrlacci->Add (laccik);
								}
							}
							//填充前4条数据
							if(misslacci->Count >0)
							{
								dtable1->Clear ();
								for(int h=j-1; h>=j-window;h--)
								{
									if(h>=0)
									{
										for(int k=1;k<=6;k++)
										{
											DataRow^ DataRow2h = dt2->Rows[h];
											if (Convert::ToInt32(DataRow2h["LAC" + k]) != -1 && Convert::ToInt32(DataRow2h["LAC" + k]) != 0)
											{
												String^ laccik = DataRow2h["LAC" + k] + "_" + DataRow2h["CI" + k];
												if(!mrlacci->Contains (laccik))
												{//fromrlacci->Add (laccik);
													System::Data::DataRow^ thisrow = dtable1->NewRow();
													thisrow["MRID"] = DataRow2h["MRID"];
													thisrow["LAC"] = DataRow2h["LAC" + k];
													thisrow["CI"] = DataRow2h["CI" + k];
													thisrow["Rxlev"] = DataRow2h["RxlevNCell" + k];
													thisrow["Distance"] = Convert::ToInt32 (j-h);
													thisrow["Frequence"] = 0;
													dtable1->Rows->Add(thisrow);
												}//end if
											}//end if
										}//end for
									}//end if
								}//end for
								if(dtable1->Rows ->Count >0)
								{
									//统计每个小区出现的频率
									for(int h=0;h<dtable1->Rows ->Count ;h++)
									{
										DataRow^ DataRow1h = dtable1->Rows[h];
										int lac=Convert::ToInt32 (DataRow1h["LAC"]);
										int ci=Convert::ToInt32 (DataRow1h["CI"]);
										for(int k=0;k<dtable1->Rows ->Count ;k++)
										{
											DataRow^ DataRow1k = dtable1->Rows[k];
											int lac1 = Convert::ToInt32(DataRow1k["LAC"]);
											int ci1 = Convert::ToInt32(DataRow1k["CI"]);
											if(lac1==lac&&ci1==ci)
											{
												DataRow1h["Frequence"]=Convert::ToInt32 (Convert::ToInt32 (DataRow1h["Frequence"])+1);
											}
										}
									}

									//计算结果
									DataView^ dv = 	dtable1->DefaultView; 
									dv->Sort = "LAC Desc,CI Desc";
									dtable1 = dv->ToTable();
									bool jisuan=false;
									DataRow^ DataRow0 = dtable1->Rows[0];
									int lac = Convert::ToInt32(DataRow0["LAC"]);
									int ci = Convert::ToInt32(DataRow0["CI"]);
									double rxlev=0;
									double num = Convert::ToDouble(DataRow0["Frequence"]);

									dtable4->Clear ();
									for(int h=0;h<dtable1->Rows->Count  ;h++)
									{
										DataRow^ DataRow1h = dtable1->Rows[h];
										if (Convert::ToInt32(DataRow1h["LAC"]) == lac&&Convert::ToInt32(DataRow1h["CI"]) == ci)
											rxlev = rxlev + Convert::ToDouble(DataRow1h["Rxlev"]);
										else
										{
											DataRow^ DataRow1h_1 = dtable1->Rows[h-1];
											System::Data::DataRow^ thisrow = dtable4->NewRow();
											thisrow["LAC"] = lac;
											thisrow["CI"] = ci;
											thisrow["Rxlev"] =Convert::ToInt32 (rxlev/num);
											//thisrow["Distance"] =dtable3->Rows [k]["Distance"];
											thisrow["Frequence"] = DataRow1h_1["Frequence"];
											dtable4->Rows->Add(thisrow);


											lac = Convert::ToInt32(DataRow1h["LAC"]);
											ci = Convert::ToInt32(DataRow1h["CI"]);
											rxlev = Convert::ToDouble(DataRow1h["Rxlev"]);
											num = Convert::ToDouble(DataRow1h["Frequence"]);
										}
										if(h==dtable1->Rows->Count -1)
										{
											System::Data::DataRow^ thisrow = dtable4->NewRow();
											thisrow["LAC"] = lac;
											thisrow["CI"] = ci;
											thisrow["Rxlev"] =Convert::ToInt32 (rxlev/num);
											//thisrow["Distance"] =dtable3->Rows [k]["Distance"];
											thisrow["Frequence"] = DataRow1h["Frequence"];
											dtable4->Rows->Add(thisrow);

										}
									}
									//修改值该条MR的空缺字段//
									DataView^ dv1 = dtable4->DefaultView; 
									dv1->Sort = "Frequence Desc, Rxlev Desc";
									dtable4 = dv1->ToTable();
									int num1=dtable4->Rows ->Count;
									//,misslacci->Count );
									for(int h=0;h<num1 ;h++)
									{	
										DataRow^ DataRow4h = dtable4->Rows[h];
										if (misslacci->Count != 0 && !mrlacci->Contains(DataRow4h["LAC"] + "_" + DataRow4h["CI"]))
										{
											DataRow^ DataRow2j = dt2->Rows[j];
											DataRow2j["LAC" + Convert::ToString(misslacci[0])] = DataRow4h["LAC"];
											DataRow2j["CI" + Convert::ToString(misslacci[0])] = DataRow4h["CI"];
											DataRow2j["RxlevNCell" + Convert::ToString(misslacci[0])] = DataRow4h["Rxlev"];
										
										misslacci->Remove (misslacci[0]);
										}
									}
								}
							}
						}//end j==count-1
						if(j>0&&j<dt2->Rows ->Count -1)
						{
							//将该小区的lacci和空缺位置填进来
							ArrayList^ mrlacci=gcnew ArrayList();
							ArrayList^ misslacci= gcnew ArrayList();
							for(int k=1;k<=6;k++)
							{
								DataRow^ DataRow2j = dt2->Rows[j];
								String^ laccik = DataRow2j["LAC" + k] + "_" + DataRow2j["CI" + k];
								if (Convert::ToInt32(DataRow2j["LAC" + k]) == -1 || Convert::ToInt32(DataRow2j["LAC" + k]) == 0 || Convert::ToInt32(DataRow2j["CI" + k]) == -1 || Convert::ToInt32(DataRow2j["CI" + k]) == 0)
									misslacci->Add (k);
								else
								{
									if(!mrlacci->Contains (laccik))
									mrlacci->Add (laccik);
								}
							}
							if(misslacci->Count >0)
							{
								//填充前4条数据
								dtable1->Clear ();
								for(int h=j-1; h>=j-window;h--)
								{
									if(h>=0)
									{
										for(int k=1;k<=6;k++)
										{
											DataRow^ DataRow2h = dt2->Rows[h];
											if (Convert::ToInt32(DataRow2h["LAC" + k]) != -1 && Convert::ToInt32(DataRow2h["LAC" + k]) != 0)
											{
												String^ laccik = DataRow2h["LAC" + k] + "_" + DataRow2h["CI" + k];
												if(!mrlacci->Contains (laccik))
												{//fromrlacci->Add (laccik);
													System::Data::DataRow^ thisrow = dtable1->NewRow();
													thisrow["MRID"] = DataRow2h["MRID"];
													thisrow["LAC"] = DataRow2h["LAC" + k];
													thisrow["CI"] = DataRow2h["CI" + k];
													thisrow["Rxlev"] = DataRow2h["RxlevNCell" + k];
													thisrow["Distance"] = Convert::ToInt32 (j-h);
													thisrow["Frequence"] = 0;
													dtable1->Rows->Add(thisrow);
												}//end if
											}//end if
										}//end for
									}//end if
								}//end for
								//填充后4条数据
								dtable2->Clear ();
								for(int h=j+1; h<=j+window&&h<dt2->Rows ->Count;h++)
								{
									if(h>=0)
									{
										for(int k=1;k<=6;k++)
										{
											DataRow^ DataRow2h = dt2->Rows[h];
											if (Convert::ToInt32(DataRow2h["LAC" + k]) != -1 && Convert::ToInt32(DataRow2h["LAC" + k]) != 0)
											{
												String^ laccik = DataRow2h["LAC" + k] + "_" + DataRow2h["CI" + k];
												if(!mrlacci->Contains (laccik))
												{//fromrlacci->Add (laccik);
													System::Data::DataRow^ thisrow = dtable2->NewRow();
													thisrow["MRID"] = DataRow2h["MRID"];
													thisrow["LAC"] = DataRow2h["LAC" + k];
													thisrow["CI"] = DataRow2h["CI" + k];
													thisrow["Rxlev"] = DataRow2h["RxlevNCell" + k];
													thisrow["Distance"] = Convert::ToInt32 (h-j);
													thisrow["Frequence"] = 0;
													dtable2->Rows->Add(thisrow);
												}//end if
											}//end if
										}//end for
									}//end if
								}//end for
								//统计每个小区出现的频率
								if(dtable1->Rows ->Count >0&&dtable2->Rows ->Count >0)
								{
									for(int h=0;h<dtable1->Rows ->Count ;h++)
									{
										DataRow^ DataRow1h = dtable1->Rows[h];
										int lac = Convert::ToInt32(DataRow1h["LAC"]);
										int ci = Convert::ToInt32(DataRow1h["CI"]);
										for(int k=0;k<dtable1->Rows ->Count ;k++)
										{
											DataRow^ DataRow1k = dtable1->Rows[k];
											int lac1 = Convert::ToInt32(DataRow1k["LAC"]);
											int ci1 = Convert::ToInt32(DataRow1k["CI"]);
											if(lac1==lac&&ci1==ci)
											{
												DataRow1h["Frequence"] = Convert::ToInt32(Convert::ToInt32(DataRow1h["Frequence"]) + 1);
											}
										}
									}
									for(int h=0;h<dtable2->Rows ->Count ;h++)
									{
										DataRow^ DataRow2h = dtable2->Rows[h];
										int lac = Convert::ToInt32(DataRow2h["LAC"]);
										int ci = Convert::ToInt32(DataRow2h["CI"]);
										for(int k=0;k<dtable2->Rows ->Count ;k++)
										{
											DataRow^ DataRow2k = dtable2->Rows[k];
											int lac1 = Convert::ToInt32(DataRow2k["LAC"]);
											int ci1 = Convert::ToInt32(DataRow2k["CI"]);
											if(lac1==lac&&ci1==ci)
											{
												DataRow2h["Frequence"] = Convert::ToInt32(Convert::ToInt32(DataRow2h["Frequence"]) + 1);
											}
										}
									}
									//填充计算结果
									dtable3->Clear ();
									for(int h=0;h<dtable1->Rows ->Count ;h++)
									{
										DataRow^ DataRow1h = dtable1->Rows[h];
										String^ lac = Convert::ToString(DataRow1h["LAC"]);
										String^ ci = Convert::ToString(DataRow1h["CI"]);
										for(int k =0;k <dtable2->Rows ->Count ;k++)
										{
											DataRow^ DataRow1k = dtable1->Rows[k];
											String^ lac1 = Convert::ToString(DataRow1k["LAC"]);
											String^ ci1 = Convert::ToString(DataRow1k["CI"]);
											if(lac1==lac&&ci1==ci)
											{
												System::Data::DataRow^ thisrow = dtable3->NewRow();
												//thisrow["MRID"] = dt2->Rows [h]["MRID"];
												thisrow["LAC"] = lac;
												thisrow["CI"] = ci;
												double rxlev1 = Convert::ToDouble(DataRow1h["Distance"]) / (Convert::ToDouble(DataRow1h["Distance"]) + Convert::ToDouble(DataRow1k["Distance"]))*Convert::ToDouble(DataRow1k["Rxlev"]);
												double rxlev2 = Convert::ToDouble(DataRow1k["Distance"]) / (Convert::ToDouble(DataRow1h["Distance"]) + Convert::ToDouble(DataRow1k["Distance"]))*Convert::ToDouble(DataRow1h["Rxlev"]);
												double rxlev=rxlev1+rxlev2;
												thisrow["Rxlev"] =Convert::ToInt32 (rxlev);
												thisrow["Distance"] = Convert::ToInt32(DataRow1h["Distance"]) + Convert::ToInt32(DataRow1k["Distance"]);
												thisrow["Frequence"] = Convert::ToInt32(DataRow1h["Frequence"]) + Convert::ToInt32(DataRow1k["Frequence"]);
												dtable3->Rows->Add(thisrow);
											}
										}
									}//dtable3中当然会有重复的小区，但是后面，我们要找distance小的，因此，不论有多少重复的（一定会有不少），它们的出现频率是能保证一致，然后都从距离小的开始找
									//将频度高，距离小的找出来，放到结果中
									if(dtable3->Rows ->Count >0)
									{
										dtable4->Clear ();
										DataView^ dv = 	dtable3->DefaultView; 
										dv->Sort = "Frequence DESC,Distance ASC";
										dtable3 = dv->ToTable();
										ArrayList^ result=gcnew ArrayList();
										for(int k =0;k<dtable3->Rows ->Count ;k++)
										{
											DataRow^ DataRow3k = dtable3->Rows[k];
											String^ laccih = DataRow3k["LAC"] + "_" + DataRow3k["CI"];
											if(!result->Contains (laccih))
											{
												result->Add (laccih);
												System::Data::DataRow^ thisrow = dtable4->NewRow();
												thisrow["LAC"] = DataRow3k["LAC"];
												thisrow["CI"] = DataRow3k["CI"];
												thisrow["Rxlev"] = DataRow3k["Rxlev"];
												thisrow["Distance"] = DataRow3k["Distance"];
												thisrow["Frequence"] = DataRow3k["Frequence"];
												dtable4->Rows->Add(thisrow);
											}
										}
										//修改值该条MR的空缺字段
										int num1=dtable4->Rows ->Count;
										for(int h=0;h<num1 ;h++)
										{	
											DataRow^ DataRow4h = dtable4->Rows[h];
											DataRow^ DataRow2j = dt2->Rows[j];
											if (misslacci->Count != 0 && !mrlacci->Contains(DataRow4h["LAC"] + "_" + DataRow4h["CI"]))
											{
												DataRow2j["LAC" + Convert::ToString(misslacci[0])] = DataRow4h["LAC"];
												DataRow2j["CI" + Convert::ToString(misslacci[0])] = DataRow4h["CI"];
												DataRow2j["RxlevNCell" + Convert::ToString(misslacci[0])] = DataRow4h["Rxlev"];
											
											misslacci->Remove (misslacci[0]);
											}
										}
									}
								}//处理下一条MR
							}
						}
					}
				}
				//将该callid写入数据库中
				String^  getConnectString = linkdb->GetConnectionString(dbname);
				Data::SqlClient::SqlBulkCopy^  bcp = gcnew Data::SqlClient::SqlBulkCopy(getConnectString);
				Data::DataTable^  dtTemp = gcnew  Data::DataTable();
				Data::SqlClient::SqlConnection::ClearAllPools();
				dtTemp = dt2;
				bcp->BatchSize = dt2->Rows->Count;
				if(i==0)
					for (int  k = 0; k < dtTemp->Columns->Count; k++)
					{
						String^  ColumnName = dtTemp->Columns[k]->ColumnName;
						Data::SqlClient ::SqlBulkCopyColumnMapping^ columnMap = gcnew Data::SqlClient ::SqlBulkCopyColumnMapping(ColumnName, ColumnName);
						bcp->ColumnMappings->Add(columnMap);
					}	
				if(i==0)
				{
				//	String ^sqlcopy = "if exists(select 1 from sysobjects where name='MRPreprocessbefore') drop table MRPreprocessbefore" ;
					String ^sqlcopy = "if exists(select 1 from sysobjects where name='original_mr_by_wukai_copy') drop table original_mr_by_wukai_copy" ;
					linkdb->Modify(sqlcopy, this->dbname  );
					
				//	String ^sqlcopy1 = "select top 1 *  into MRPreprocessbefore from temp1;delete from MRPreprocessbefore;" ;
					String ^sqlcopy1 = "select top 1 *  into original_mr_by_wukai_copy from temp1;delete from original_mr_by_wukai_copy;" ;
					linkdb->Modify(sqlcopy1, this->dbname  );
				}
			//	bcp->DestinationTableName = "MRPreprocessbefore ";
				bcp->DestinationTableName = "original_mr_by_wukai_copy ";
				bcp->WriteToServer(dtTemp); 
				bcp->Close();
			}
			String ^sqldelete = "drop table temp1" ;
			linkdb->Modify(sqldelete, this->dbname    );
			return true;
			/*}
			catch(Exception^e)
			{
				throw e;
				return false;
			}*/
	}
//对MRPreprocessbefore中的小区按信号强度排序（这里没有用到）
bool DataProcess::MRRxlevSort() 
	{
		dbname = "Locate_成都";
		LinkDB^ linkdb = gcnew LinkDB();
		Data::SqlClient::SqlConnection^ conn = linkdb->GetConnection(dbname);

		System::Data::DataTable^ dtresult=gcnew System::Data::DataTable();
		dtresult->Columns ->Add ("RxlevNCell");
		dtresult->Columns ->Add ("BcchNCell");
		dtresult->Columns ->Add ("BSICNCell");
		dtresult->Columns ->Add ("LAC");
		dtresult->Columns ->Add ("CI");

		System::Data ::DataTable ^dtresult1=gcnew System::Data ::DataTable();
		dtresult1->Columns ->Add ("RxlevNCell");
		dtresult1->Columns ->Add ("BcchNCell");
		dtresult1->Columns ->Add ("BSICNCell");
		dtresult1->Columns ->Add ("LAC");
		dtresult1->Columns ->Add ("CI");

		System::Data ::DataTable ^dt1=gcnew System::Data::DataTable();
		String^ select1="select * from original_mr_by_wukai_copy";
		linkdb->FillDataTableProj(select1,dt1,this->dbname );

		for (int i=0;i<dt1->Rows ->Count ;i++)
		{			
			dtresult->Clear ();
			dtresult1->Clear ();
			for(int j=1;j<=6;j++)
			{
				DataRow^ DataRowTest = dt1->Rows[i];
				if (Convert::ToInt32(DataRowTest["LAC" + j]) == -1)
				{
					System::Data::DataRow^ thisrow = dtresult1->NewRow();
					thisrow["RxlevNCell"] = DataRowTest["RxlevNCell" + j];
					thisrow["BcchNCell"] = DataRowTest["BcchNCell" + j];
					thisrow["BSICNCell"] = DataRowTest["BSICNCell" + j];
					thisrow["LAC"] = DataRowTest["LAC" + j];
					thisrow["CI"] = DataRowTest["CI" + j];
					dtresult1->Rows ->Add (thisrow);
				}
				else
				{
					System::Data::DataRow^ thisrow = dtresult->NewRow();
					thisrow["RxlevNCell"] = DataRowTest["RxlevNCell" + j];
					thisrow["BcchNCell"] = DataRowTest["BcchNCell" + j];
					thisrow["BSICNCell"] = DataRowTest["BSICNCell" + j];
					thisrow["LAC"] = DataRowTest["LAC" + j];
					thisrow["CI"] = DataRowTest["CI" + j];
					dtresult->Rows ->Add (thisrow);
				}
			}
			DataView^ dv = 	dtresult->DefaultView; 
			dv->Sort = "RxlevNCell Asc";
			dtresult = dv->ToTable();
			int count=dtresult->Rows ->Count ;
			for(int j=1;j<=count;j++)
			{
				DataRow^ RowTest = dt1->Rows[j];
				DataRow^ dtRowTest = dt1->Rows[j-1];
				RowTest["RxlevNCell" + j] = dtRowTest["RxlevNCell"];
				RowTest["BcchNCell" + j] = dtRowTest["BcchNCell"];
				RowTest["BSICNCell" + j] = dtRowTest["BSICNCell"];
				RowTest["LAC" + j] = dtRowTest["LAC"];
				RowTest["CI" + j]  = dtRowTest["CI"];

			}
			int count1=dtresult1->Rows ->Count ;
			for(int j=count+1;j<=6;j++)
			{
				DataRow^ RowTest = dt1->Rows[j];
				DataRow^ dtRowTest = dt1->Rows[j -count - 1];
				RowTest["RxlevNCell" + j] = dtRowTest["RxlevNCell"];
				RowTest["BcchNCell" + j] = dtRowTest["BcchNCell"];
				RowTest["BSICNCell" + j] = dtRowTest["BSICNCell"];
				RowTest["LAC" + j] = dtRowTest["LAC"];
				RowTest["CI" + j] = dtRowTest["CI"];

			}
		}
				String^  getConnectString = linkdb->GetConnectionString(dbname);
				Data::SqlClient::SqlBulkCopy^  bcp = gcnew Data::SqlClient::SqlBulkCopy(getConnectString);
				Data::DataTable^  dtTemp = gcnew  Data::DataTable();
				Data::SqlClient::SqlConnection::ClearAllPools();
				dtTemp = dt1;
				bcp->BatchSize = dtTemp->Rows->Count;
				for (int  k = 0; k < dtTemp->Columns->Count; k++)
				{
					String^  ColumnName = dtTemp->Columns[k]->ColumnName;
					Data::SqlClient ::SqlBulkCopyColumnMapping^ columnMap = gcnew Data::SqlClient ::SqlBulkCopyColumnMapping(ColumnName, ColumnName);
					bcp->ColumnMappings->Add(columnMap);
				}	
			
				String ^sqlcopy = "delete from original_mr_by_wukai_copy" ;
				linkdb->Modify(sqlcopy, this->dbname);
					
				bcp->DestinationTableName = "original_mr_by_wukai_copy";
				bcp->WriteToServer(dtTemp); 
				bcp->Close ();
				return true;
	}
//根据点评信号判断该MR是否处于切换状态（这里没有用到）
bool DataProcess::HandOver ()
	{
		dbname = "Locate_成都";
		LinkDB^ linkdb = gcnew LinkDB();
		Data::SqlClient::SqlConnection^ conn = linkdb->GetConnection(dbname);

		String^select="select distinct callid from tbMR";
		System::Data ::DataTable ^dt1=gcnew System::Data ::DataTable();
		linkdb->FillDataTableProj (select,dt1,this->dbname );

		for(int i=0;i<dt1->Rows ->Count ;i++)
		{
			DataRow^ dtRowTest = dt1->Rows[i];
			System::Data ::DataTable ^dt2=gcnew System::Data ::DataTable();
			String^select2 = "select * from tbMR where CallID = " + dtRowTest["CallID"] + " order by mrid";
			linkdb->FillDataTableProj (select2,dt2,this->dbname );
			for(int j=1;j<dt2->Rows ->Count ;j++)
			{
				DataRow^ RowTest = dt2->Rows[j];
				DataRow^ RowTest1 = dt2->Rows[j-1];
				int cell1 = Convert::ToInt32(RowTest1["CellID"]);
				int cell2 = Convert::ToInt32(RowTest["CellID"]);
				if(j==1)
					RowTest1["LocateLongitude"] = 0;
					
				if(cell1==cell2)
					RowTest["LocateLongitude"] = 0;
				else
					RowTest["LocateLongitude"] = 1;

			}
			String^  getConnectString = linkdb->GetConnectionString(dbname);
			Data::SqlClient::SqlBulkCopy^  bcp = gcnew Data::SqlClient::SqlBulkCopy(getConnectString);
			Data::DataTable^  dtTemp = gcnew  Data::DataTable();
			Data::SqlClient::SqlConnection::ClearAllPools();
			dtTemp = dt2;
			bcp->BatchSize = dtTemp->Rows->Count;
			for (int  k = 0; k < dtTemp->Columns->Count; k++)
			{
				String^  ColumnName = dtTemp->Columns[k]->ColumnName;
				Data::SqlClient ::SqlBulkCopyColumnMapping^ columnMap = gcnew Data::SqlClient ::SqlBulkCopyColumnMapping(ColumnName, ColumnName);
				bcp->ColumnMappings->Add(columnMap);
			}	
			if(i==0)
			{
				String ^sqlcopy = "if exists(select 1 from sysobjects where name='temp') drop table temp" ;
				linkdb->Modify(sqlcopy, this->dbname  );
					
				String ^sqlcopy1 = "select top 1 *  into temp from tbMR;delete from temp;" ;
				linkdb->Modify(sqlcopy1, this->dbname  );
			}
			bcp->DestinationTableName = "temp";
			bcp->WriteToServer(dtTemp);
			bcp->Close();
		}
		return true;
	}
//计算同一callid下的后一条mr与前一条mr的距离（这里没有用到）
bool DataProcess::computespeed()
	{
		//callid,mrid,distance
		dbname = "Locate_成都";
		LinkDB^ linkdb = gcnew LinkDB();
		Data::SqlClient::SqlConnection^ conn = linkdb->GetConnection(dbname);

		String^select="select distinct callid from tbMR";
		System::Data ::DataTable ^dt1=gcnew System::Data ::DataTable();
		linkdb->FillDataTableProj (select,dt1,this->dbname );

		System::Data ::DataTable ^dtresult=gcnew System::Data ::DataTable();
		dtresult->Columns->Add("CallID");
		dtresult->Columns->Add("MRID");
		dtresult->Columns->Add("Time");
		dtresult->Columns->Add("MRDistance");
		dtresult->Columns->Add("MRSpeed");
		dtresult->Columns->Add("Longitude");
		dtresult->Columns->Add("Latitude");

		String^  getConnectString =linkdb->GetConnectionString(dbname);
		Data::SqlClient::SqlBulkCopy^  bcp = gcnew Data::SqlClient ::SqlBulkCopy(getConnectString);
		Data::DataTable^  dtTemp =gcnew  Data::DataTable();
		Data::SqlClient::SqlConnection ::ClearAllPools();
		bcp->BatchSize = 10000;

		String^  getConnectString1 =linkdb->GetConnectionString(dbname);
		Data::SqlClient::SqlBulkCopy^  bcp1 = gcnew Data::SqlClient ::SqlBulkCopy(getConnectString1);
		Data::DataTable^  dtTemp1 =gcnew  Data::DataTable();
		Data::SqlClient::SqlConnection ::ClearAllPools();
		bcp1->BatchSize = 10000;

		for(int i=0;i<dt1->Rows ->Count ;i++)
		{
			DataRow^ dtRowTest = dt1->Rows[i];
			System::Data ::DataTable ^dt2=gcnew System::Data ::DataTable();
			String^select2 = "select * from MRPreprocessbefore where CallID = " + dtRowTest["CallID"] + " order by timeid";
			
			linkdb->FillDataTableProj (select2,dt2,this->dbname );

			dtresult->Clear ();
			for(int j=0;j<dt2->Rows ->Count ;j++)
			{
				DataRow^ RowTest = dt2->Rows[j];
				DataRow^ RowTest1 = dt2->Rows[j - 1];
				DataRow^ RowTest2 = dt2->Rows[j - 2];
				Data::DataRow ^ thisrow=dtresult->NewRow ();
				thisrow["CallID"] = RowTest["CallID"];
				thisrow["MRID"] = RowTest["MRID"];
				thisrow["Longitude"] = RowTest["Longitude"];
				thisrow["Latitude"] = RowTest["Latitude"];
				
				if(this->dbname =="Locate_宁波")
				{					
					//MessageBox::Show (/*Convert::ToString (dt2->Rows [j]["MRID"])+"--"+*/Convert::ToString (dt2->Rows [j]["Time"]));
					thisrow["Time"]=System::Convert ::ToDateTime(RowTest["Time"]);
				}
				else
				{
					String^time = Convert::ToString(RowTest["Time"]);
					time=time->Insert (4,"-");
					time=time->Insert (7,"-");
					time=time->Insert (10," ");
					time=time->Insert (13,":");
					time=time->Insert (16,":");
					time=time->Insert (19,".");
					thisrow["Time"]=System::Convert ::ToDateTime(time);
				}
				if(j==0||j==1)
				{
					thisrow["MRDistance"]=0;
					thisrow["MRSpeed"]=0;
				}
				else
				{
					double MRlon1,MRlat1,MRlon2,MRlat2,MRlon3,MRlat3;
					MRlon1 = Convert::ToDouble(RowTest2["Longitude"]);
					MRlat1 = Convert::ToDouble(RowTest2["Latitude"]);
					MRlon2 = Convert::ToDouble(RowTest1["Longitude"]);
					MRlat2 = Convert::ToDouble(RowTest1["Latitude"]);
					MRlon3 = Convert::ToDouble(RowTest["Longitude"]);
					MRlat3 = Convert::ToDouble(RowTest["Latitude"]);
					double MRDistance1=CJWDHelper::distance(MRlon1,MRlat1,MRlon2,MRlat2,0)*1000;
					double MRDistance2=CJWDHelper::distance(MRlon2,MRlat2,MRlon3,MRlat3,0)*1000;
					double MRDistance=MRDistance1+MRDistance2;
					thisrow["MRDistance"]=Math::Round (MRDistance,6);
					String^time1 = Convert::ToString(RowTest2["Time"]);
					time1=time1->Insert (4,"-");
					time1=time1->Insert (7,"-");
					time1=time1->Insert (10," ");
					time1=time1->Insert (13,":");
					time1=time1->Insert (16,":");
					time1=time1->Insert (19,".");
					System::DateTime^datetime1=System::Convert ::ToDateTime(time1);
					String^time2 = Convert::ToString(RowTest["Time"]);
					time2=time2->Insert (4,"-");
					time2=time2->Insert (7,"-");
					time2=time2->Insert (10," ");
					time2=time2->Insert (13,":");
					time2=time2->Insert (16,":");
					time2=time2->Insert (19,".");
					System::DateTime^datetime2=System::Convert ::ToDateTime(time2);
						//System::DateTime ^ time = System::Convert ::ToDateTime (drs1[0]["Time"]);
						System::TimeSpan ^ timespan1;
						timespan1=(datetime2->Subtract (*datetime1));
						int timespan2=timespan1->Seconds;
						if(timespan2==0)
							thisrow["MRSpeed"]=0;
						else
							thisrow["MRSpeed"]=Math::Round (Convert::ToDouble (MRDistance/Convert::ToDouble(timespan2)),6);

						

					//暂时不需要计算定位结果的速度
					/*double Locatelon1,Locatelat1,Locatelon2,Locatelat2;
					Locatelon1=Convert::ToDouble (dt2->Rows [j-1]["LocateLongitude"]);
					Locatelat1=Convert::ToDouble (dt2->Rows [j-1]["LocateLatitude"]);
					Locatelon2=Convert::ToDouble (dt2->Rows [j]["LocateLongitude"]);
					Locatelat2=Convert::ToDouble (dt2->Rows [j]["LocateLatitude"]);
					double ResultDistance=CJWDHelper::distance(Locatelon1,Locatelat1,Locatelon2,Locatelat2,0)*1000;
					thisrow["ResultDistance"]=Math::Round (ResultDistance,6);*/
				}
				dtresult->Rows ->Add (thisrow);
			}



		dtTemp = dtresult;
			if(i==0)
				for (int  k = 0; k < dtTemp->Columns->Count; k++)
				{
					String^  ColumnName = dtTemp->Columns[k]->ColumnName;
					Data::SqlClient ::SqlBulkCopyColumnMapping^ columnMap = gcnew Data::SqlClient ::SqlBulkCopyColumnMapping(ColumnName, ColumnName);
					bcp->ColumnMappings->Add(columnMap);
				}	
			if(i==0)
			{
					String ^sqlcopy = "delete from tbMRSpeed;" ;
					linkdb->Modify(sqlcopy, this->dbname  );
			}
			bcp->DestinationTableName = "tbMRSpeed ";
			bcp->WriteToServer(dtTemp);
			
		}
		bcp->Close ();
		return true;

	}
//MR有效性检测，预处理第一步
bool DataProcess::MREffectiveCheck()
	{
		dbname = "Locate_成都";
		LinkDB^ linkdb = gcnew LinkDB();
		Data::SqlClient::SqlConnection^ conn = linkdb->GetConnection(dbname);

		//String^SetEffective="update tbmr1 set effective=1; update tbmr1 set lac0=lac,ci0=ci from tbLacCi_CellID as a where tbmr1.cellid=cellid ;";
		String^SetEffective="update original_mr_by_wukai_copy set effective=1; update original_mr_by_wukai_copy set lac0=tbLacCi_CellID.lac,ci0=tbLacCi_CellID.ci from tbLacCi_CellID, original_mr_by_wukai_copy where original_mr_by_wukai_copy.cellid=tbLacCi_CellID.cellid ;";
		linkdb->Modify (SetEffective,this->dbname );

		//String^selectmr="select * from tbmr1 ";
		String^selectmr="select * from original_mr_by_wukai_copy ";
		System::Data ::DataTable ^dtmr=gcnew System::Data ::DataTable ();
		linkdb->FillDataTableProj (selectmr,dtmr,this->dbname);

		String^selectcell="select * from tbcell";
		System::Data::DataTable^ dtcell=gcnew System::Data::DataTable ();
		linkdb->FillDataTableProj (selectcell,dtcell,this->dbname);

		for(int i=0;i<dtmr->Rows ->Count ;i++)
		{
			DataRow^ RowTest = dtmr->Rows[i];
			String^lac0 = Convert::ToString(RowTest["LAC0"]);
			String^ci0 = Convert::ToString(RowTest["CI0"]);
			if(lac0!=""||ci0!="")
			{
				cli::array <System::Data ::DataRow ^,1>^ drs1 = dtcell->Select ("LAC="+lac0+" and CI="+ci0+"");
				double lon0=Convert::ToDouble (drs1[0]["Longitude"]);
				double lat0=Convert::ToDouble (drs1[0]["Latitude"]);
				for(int j=1;j<=6;j++)
				{
					String^lac = Convert::ToString(RowTest["LAC" + j]);
					String^ci = Convert::ToString(RowTest["CI" + j]);
					if(lac!="-1"&&ci!="-1")
					{
						cli::array <System::Data ::DataRow ^,1>^ drs2 = dtcell->Select ("LAC="+lac+" and CI="+ci+"");
						if(drs2->Length >0)
						{
							double lon=Convert::ToDouble (drs2[0]["Longitude"]);
							double lat=Convert::ToDouble (drs2[0]["Latitude"]);
							double distance=CJWDHelper::distance (lon0,lat0,lon,lat,0);
							if(distance>=10)
							{
								RowTest["Effective"] = 0;
							}
						}
					}
				}
			}
			else
				RowTest["Effective"] = 0;
		}
		String^  getConnectString = linkdb->GetConnectionString("Locate_成都");
		Data::SqlClient::SqlBulkCopy^  bcp = gcnew Data::SqlClient::SqlBulkCopy(getConnectString);
		Data::DataTable^  dtTemp = gcnew  Data::DataTable();
		Data::SqlClient::SqlConnection::ClearAllPools();
		dtTemp = dtmr;
		bcp->BatchSize = dtmr->Rows->Count;
		for (int  k = 0; k < dtTemp->Columns->Count; k++)
		{
			String^  ColumnName = dtTemp->Columns[k]->ColumnName;
			Data::SqlClient ::SqlBulkCopyColumnMapping^ columnMap = gcnew Data::SqlClient ::SqlBulkCopyColumnMapping(ColumnName, ColumnName);
			bcp->ColumnMappings->Add(columnMap);
		}	
;
		String ^sqlcopy = "delete from original_mr_by_wukai_copy;" ;
		linkdb->Modify(sqlcopy, this->dbname  );

		bcp->DestinationTableName = "original_mr_by_wukai_copy ";
		bcp->WriteToServer(dtTemp);
		bcp->Close ();
		return true;

	}
//数据平滑
bool DataProcess::SmothMR() 
{
			dbname = "Locate_成都";
			LinkDB^ linkdb=gcnew LinkDB();
			Data::SqlClient::SqlConnection^ conn = linkdb->GetConnection(dbname);

			int window=5;//前面的窗口都是3，这次变成5了
			int index = 0;//检测平滑了几个mr
			int mm = 0;//武凯添加，为了统计平滑的小区数。

			System::Data::DataTable^ dtable1 = gcnew System::Data::DataTable();
			dtable1->Columns->Add("LAC");
			dtable1->Columns->Add("CI");
			dtable1->Columns->Add ("Frequence");
			//导入的临时表中
			String ^str0 = "if exists(select 1 from sysobjects where name='temp1') drop table temp1" ;
			linkdb->Modify(str0, this->dbname  );
			//String^str1="select  * into   temp1   from MRPreprocessbefore  order by Time;";
			String^str1="select  * into   temp1   from original_mr_by_wukai_copy order by Time;";
			linkdb->Modify(str1,dbname);
			//提取所有不同的callid
			String^ select1="select distinct callid from temp1";
			System::Data::DataTable^ dt1= gcnew System::Data::DataTable();
			linkdb->FillDataTableProj(select1,dt1,this->dbname );
			for(int i=0;i<dt1->Rows ->Count ;i++)
			{
				DataRow^ DataRowTest = dt1->Rows[i];
				String^select2 = "select * from temp1 where callid = " + DataRowTest["CallID"] + " order by time";
				System::Data::DataTable^ dt2= gcnew System::Data::DataTable();
				linkdb->FillDataTableProj(select2,dt2,this->dbname );
				int rownum=dt2->Rows ->Count;
				for(int j=0;j<=rownum/window;j++)//注意，平滑就不是逐条进行了，而是以窗口长度为单位
				{					
					System::Collections::Generic::Dictionary <String^, int>^ laccifre = gcnew Dictionary<String^, int>();
					laccifre->Clear ();
					ArrayList^lacci=gcnew ArrayList();
					for(int h=0;h<=window-1&&(j*window+h)<rownum;h++)
					{
						DataRow^ DataRowTest = dt2->Rows[j*window + h];
						for(int k=0;k<=6;k++)//包括了主小区
						{
							String^ lac = DataRowTest["LAC" + k]->ToString();
							String^ ci =  DataRowTest["CI" + k]->ToString();
							//cli::array <System::Data ::DataRow ^,1>^ drs1 = dtable1->Select ("LAC = '"+ lac+"' and CI = '"+ci+"'");
							if(laccifre->ContainsKey (lac+"-"+ci))//if和esle搭配，统计所有小区出现的次数
								laccifre[lac+"-"+ci]+=1;
							else
							{
								laccifre[lac+"-"+ci]=1;
								lacci->Add (lac+"-"+ci);
							}
							
						}
					}
					for(int a=0;a<lacci->Count ;a++)
					{
						if(Convert::ToInt32 (laccifre[lacci[a]->ToString ()])>=window)
						{
							ArrayList^rxlev=gcnew ArrayList();
							for(int h=0;h<=window-1&&(j*window+h)<rownum;h++)
							{
								DataRow^ DataRowTest = dt2->Rows[j*window + h];
								for(int k=0;k<=6;k++)//包括了主小区
								{
									String^ lac = DataRowTest["LAC" + k]->ToString();
									String^ ci = DataRowTest["CI" + k]->ToString();
									//cli::array <System::Data ::DataRow ^,1>^ drs1 = dtable1->Select ("LAC = '"+ lac+"' and CI = '"+ci+"'");
									if(lacci[a]->ToString()==Convert::ToString (lac+"-"+ci))
									{
										if(k==0)
											rxlev->Add(DataRowTest["DLRxLevAdjusted"]);
										else
											rxlev->Add(DataRowTest["RxlevNCell" + k]);
									}
								}
							}
							//调用平滑
							this->SmoothFilter1(rxlev, mm);
							index++;//注释掉的是原程序的写法。武凯因为要统计平滑的mr数量，因此index++在这里并不能起到作用。
							//填充回测量报告中。无论改或没改，都回填。武凯注
							int m=0;
							for(int h=0;h<=window-1&&(j*window+h)<rownum;h++)//这里的两个for顺序要与前面“提取”rxlev的顺序一致。因为rxlev[]是按序存储的，并没有其他标识。因此，回插的时候一定要按序处理。
							{
								DataRow^ DataRowTest = dt2->Rows[j*window + h];
								for(int k=0;k<=6;k++)
								{
									String^ lac = DataRowTest["LAC" + k]->ToString();
									String^ ci =  DataRowTest["CI" + k]->ToString();
									//cli::array <System::Data ::DataRow ^,1>^ drs1 = dtable1->Select ("LAC = '"+ lac+"' and CI = '"+ci+"'");
									if(lacci[a]->ToString ()==Convert::ToString (lac+"-"+ci))
									{
										if(k==0)
											DataRowTest["DLRxLevAdjusted"] = rxlev[m++];//注释掉的是原if内容，下面的if是武凯修改。目的是为了统计平滑的mr数量
										/*if(k==0)
										{
											if(Convert::ToDouble (dt2->Rows [j*window+h]["DLRxLevAdjusted"])==Convert::ToDouble (rxlev[m++]))
												;
											else
											{
												dt2->Rows [j*window+h]["DLRxLevAdjusted"]=rxlev[m++];
											}
										}*/
											
										else
										{
											if (Convert::ToDouble(DataRowTest["RxlevNCell" + k]) == Convert::ToDouble(rxlev[m++]))
												;
											else
											{
												DataRowTest["RxlevNCell" + k] = rxlev[m++];
												//dt2->Rows [j*window+h]["BcchNCell"+k]=99;
											}
										}
									}
								}
							}
						}//end if
					}//end forh
				}//end for j

				String^  getConnectString = linkdb->GetConnectionString(dbname);
				Data::SqlClient::SqlBulkCopy^  bcp = gcnew Data::SqlClient::SqlBulkCopy(getConnectString);
				Data::DataTable^  dtTemp = gcnew  Data::DataTable();
				Data::SqlClient::SqlConnection::ClearAllPools();
				dtTemp = dt2;
				bcp->BatchSize = dtTemp->Rows->Count;
				if(i==0)
					for (int  k = 0; k < dtTemp->Columns->Count; k++)
					{
						String^  ColumnName = dtTemp->Columns[k]->ColumnName;
						Data::SqlClient ::SqlBulkCopyColumnMapping^ columnMap = gcnew Data::SqlClient::SqlBulkCopyColumnMapping(ColumnName, ColumnName);
						bcp->ColumnMappings->Add(columnMap);
					}	
				if(i==0)
				{
//					String ^sqlcopy = "if exists(select 1 from sysobjects where name='MRPreprocessbefore') drop table MRPreprocessbefore" ;
					String^ sqlcopy = "if exists(select 1 from sysobjects where name='original_mr_by_wukai_copy') drop table original_mr_by_wukai_copy" ;
					linkdb->Modify(sqlcopy, this->dbname  );
					
//					String ^sqlcopy1 = "select top 1 *  into MRPreprocessbefore from temp1;delete from MRPreprocessbefore;" ;
					String^ sqlcopy1 = "select top 1 *  into original_mr_by_wukai_copy from temp1;delete from original_mr_by_wukai_copy;" ;
					linkdb->Modify(sqlcopy1, this->dbname  );
				}

				bcp->DestinationTableName = "original_mr_by_wukai_copy ";
				bcp->WriteToServer(dtTemp); 
				bcp->Close();
			}//end for i
			String^ sqldelete = "drop table temp1" ;
			linkdb->Modify(sqldelete, this->dbname);
			MessageBox::Show (index.ToString());
			MessageBox::Show (mm.ToString());
		return true;

}
//平滑处理，处理数据，拉依达准则，用拉格朗日线性插值多项式值代替野点
//处理所有的数据，当长度小于一个指定值时，可以直接对所有的值进行处理
//bool DataProcess::SmoothFilter1( ArrayList^ value)武凯修改，为了统计。这是原函数  
bool DataProcess::SmoothFilter1( ArrayList^ value, int &mm)
{
			int n = value->Count;
			double sum = 0;
			for (int i = 0; i <= n-1; i++)
			{
				sum += Convert::ToDouble(value[i]);
			}
			double averageValue = sum / n;      //求均值

			ArrayList^ diff = gcnew ArrayList();   //求残差
			for (int i = 0; i <= n-1; i++)
			{
				diff->Add(Convert::ToDouble(value[i]) - averageValue);
			}

			double sumDiff = 0;
			for (int i = 0; i <= diff->Count - 1; i++)
			{
				sumDiff += Math::Pow(Convert::ToDouble(diff[i]), 2);//求残差平方和
			}
			double standardDiff = Math::Sqrt(sumDiff / (diff->Count - 1));        //求标准差

			ArrayList^ temp = gcnew ArrayList();
			ArrayList^ record = gcnew ArrayList();
			for (int i = 0; i <= n - 1; i++)      //剔除不合准则的点
			{
				if (Math::Abs(Convert::ToDouble(value[i]) - averageValue) <= 3 * standardDiff)//武凯添加，因为如果窗口内的小区电平值都一样，那么显然不等号两边相等。因此要加上等于这个条件
				{
					temp->Add(value[i]);
				}
				else
				{
					record->Add(i);
				}
			}

			//终于搞懂了。原来，全体窗口内的小区都不存在依拉达野点。所以才出现了以前的诸多情况。
			for (int i = 0; i <= record->Count - 1; i++)         //在剔除点处插入替换数据  
			{
//				InsertValue2(temp, Convert::ToInt32(record[i]));武凯修改，为了统计。这是原函数
				InsertValue2(temp, Convert::ToInt32(record[i]), mm, averageValue, standardDiff);
			}
			value = temp;
			return true;
 }
 //补全缺失的数据，拉格朗日线性插值，野点剔除的时候用到的
//bool DataProcess::InsertValue2(ArrayList^ target,int index)这是原函数声明。武凯修改，为了统计
bool DataProcess::InsertValue2(ArrayList^ target,int index, int& mm, double average, double diff)//average是均值，diff是标准差
{
			   //if (index < 0 || index > target->Count || target->Count < 2)//对于超过边界的野点和只有一个非野点的情况，不予平滑
	  //      {
	  //          return false;
	  //      }//对于这种情况，要做处理。9月13日武凯修改

			if (index < 0 || index > target->Count || target->Count < 2)//对于超过边界的野点和只有一个非野点的情况，武凯添加新规则。9月13日
			{
				double value = Convert::ToDouble(target[index]);
				mm++;
				if (value > average)
				{
					value = average + diff;
				}
				if (value < average)
				{
					value = average - diff;
				}
				target->Insert(index, value);
				return true;
			}

			else if (target->Count == 0)
			{
				return true;
			}
			else if (target->Count == 1)
			{
				mm++;//武凯添加
				target->Insert(index, target[0]);
				return true;

			}
			else
			{
				if (index == 0)
				{
					mm++;//武凯添加
					double left = index + 1;
					double right = index + 2;
					double leftValue = Convert::ToDouble(target[index]);
					double rightValue = Convert::ToDouble(target[index + 1]);
					double value = ((index - right) / (left - right)) * leftValue + ((index - left) / (right - left)) * rightValue;     //自变量为正整数情况下的拉格朗日线性插值多项式
					target->Insert(index, value);
					return true;
				}
				else if (index == target->Count)
				{
					mm++;//武凯添加
					double left = index - 2;
					double right = index - 1;
					double leftValue = Convert::ToDouble(target[index - 2]);
					double rightValue = Convert::ToDouble(target[index - 1]);
					double value = ((index - right) / (left - right)) * leftValue + ((index - left) / (right - left)) * rightValue;
					target->Insert(index, value);
					return true;
				}
				else
				{
					mm++;//武凯添加
					double left = index - 1;
					double right = index + 1;

					double leftValue = Convert::ToDouble(target[index - 1]);
					double rightValue = Convert::ToDouble(target[index]);

					double value = ((index - right) / (left - right)) * leftValue + ((index - left) / (right - left)) * rightValue;
					target->Insert(index, value);
					return true;
				}
			}
 }