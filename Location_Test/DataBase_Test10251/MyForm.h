#pragma once
#include "windows.h"
#include "linkDB.h"
namespace Location {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MyForm 摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public: 
		static bool okk = false;
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO:  在此处添加构造函数代码
			//
			if (true)
			{
				this->okk = true;
				if (LinkDB::InitialDataBase())
				{
					//计算设备的内容空间及可应用的空间大小
					/*GSMCellCover::GetTotalPhysicalMemory ^ GetMemoClass = gcnew GetTotalPhysicalMemory();
					int Memory = GetMemoClass->GetTotalPhysicalMemoryMB();*/
					int Memory = 800;
					int MinMemo = 0, MaxMemo = 2147483647;
					if (Memory > 700)
					{
						MinMemo = 80;
						MaxMemo = Math::Max(600, Memory / 2);
					}
					else
					{
						MinMemo = 40;
						MaxMemo = 300;
					}
					//设置程序运行时数据所占用的空间大小
					if (MessageBox::Show("是否重新配置SQL Server2005内存空间占用？\n\n本计算机物理内存大小为" + System::Convert::ToString(Memory) + "MB\n\n(1)选择‘是’将修改SQL Server2005内存空间占用\n\n最小内存占用为" + System::Convert::ToString(MinMemo) + "M，最大内存占用为" + System::Convert::ToString(MaxMemo) + "M\n\n(2)选择‘否’，SQL Server2005内存空间占用将维持原有设置\n\n建议选择‘是’", "提示", MessageBoxButtons::YesNo, MessageBoxIcon::Information) == System::Windows::Forms::DialogResult::Yes)
					{
						if (!LinkDB::InitialDataBase())
						{
							MessageBox::Show("请设置数据库初始化连接！", "提示", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
							return;
						}
						LinkDB^ dbase = gcnew LinkDB();
						SqlConnection^ con = dbase->GetConnection("master");
						//SqlConnection con = new SqlConnection(@"Data Source=(local);Integrated Security=SSPI;" + "Initial Catalog=master");
						SqlCommand^ cmd = con->CreateCommand();
						cmd->CommandTimeout = 600000;
						cmd->CommandText = "CHECKPOINT\n" + "DBCC DROPCLEANBUFFERS\n" + "exec sp_configure 'show advanced options', 1;\n" + "RECONFIGURE;" +
							"exec sp_configure \"min server memory\", " + System::Convert::ToString(MinMemo) + "\nexec sp_configure \"max server memory\", " + System::Convert::ToString(MaxMemo) + "\nexec sp_configure \"min memory per query\", 2048" + "\nRECONFIGURE;";
						cmd->ExecuteNonQuery();
						con->Close();
					}
				}

			}
		}

	protected:
		/// <summary>
		/// 清理所有正在使用的资源。
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	protected:
	private: System::Windows::Forms::ToolStripMenuItem^  mRToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  局部匹配在线定位ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  mRToolStripMenuItem1;

	private:
		/// <summary>
		/// 必需的设计器变量。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 设计器支持所需的方法 - 不要
		/// 使用代码编辑器修改此方法的内容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->mRToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->局部匹配在线定位ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mRToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->mRToolStripMenuItem,
					this->局部匹配在线定位ToolStripMenuItem, this->mRToolStripMenuItem1
			});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(345, 25);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// mRToolStripMenuItem
			// 
			this->mRToolStripMenuItem->Name = L"mRToolStripMenuItem";
			this->mRToolStripMenuItem->Size = System::Drawing::Size(76, 21);
			this->mRToolStripMenuItem->Text = L"MR预处理";
			this->mRToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::mRToolStripMenuItem_Click);
			// 
			// 局部匹配在线定位ToolStripMenuItem
			// 
			this->局部匹配在线定位ToolStripMenuItem->Name = L"局部匹配在线定位ToolStripMenuItem";
			this->局部匹配在线定位ToolStripMenuItem->Size = System::Drawing::Size(116, 21);
			this->局部匹配在线定位ToolStripMenuItem->Text = L"局部匹配在线定位";
			this->局部匹配在线定位ToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::局部匹配在线定位ToolStripMenuItem_Click);
			// 
			// mRToolStripMenuItem1
			// 
			this->mRToolStripMenuItem1->Name = L"mRToolStripMenuItem1";
			this->mRToolStripMenuItem1->Size = System::Drawing::Size(76, 21);
			this->mRToolStripMenuItem1->Text = L"MR后处理";
			this->mRToolStripMenuItem1->Click += gcnew System::EventHandler(this, &MyForm::mRToolStripMenuItem1_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->ClientSize = System::Drawing::Size(345, 262);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void mRToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
	}
private: System::Void 局部匹配在线定位ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
}
private: System::Void mRToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) {
}
};
}
