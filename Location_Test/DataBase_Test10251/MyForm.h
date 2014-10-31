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
	/// MyForm ժҪ
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public: 
		static bool okk = false;
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO:  �ڴ˴���ӹ��캯������
			//
			if (true)
			{
				this->okk = true;
				if (LinkDB::InitialDataBase())
				{
					//�����豸�����ݿռ估��Ӧ�õĿռ��С
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
					//���ó�������ʱ������ռ�õĿռ��С
					if (MessageBox::Show("�Ƿ���������SQL Server2005�ڴ�ռ�ռ�ã�\n\n������������ڴ��СΪ" + System::Convert::ToString(Memory) + "MB\n\n(1)ѡ���ǡ����޸�SQL Server2005�ڴ�ռ�ռ��\n\n��С�ڴ�ռ��Ϊ" + System::Convert::ToString(MinMemo) + "M������ڴ�ռ��Ϊ" + System::Convert::ToString(MaxMemo) + "M\n\n(2)ѡ�񡮷񡯣�SQL Server2005�ڴ�ռ�ռ�ý�ά��ԭ������\n\n����ѡ���ǡ�", "��ʾ", MessageBoxButtons::YesNo, MessageBoxIcon::Information) == System::Windows::Forms::DialogResult::Yes)
					{
						if (!LinkDB::InitialDataBase())
						{
							MessageBox::Show("���������ݿ��ʼ�����ӣ�", "��ʾ", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
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
		/// ������������ʹ�õ���Դ��
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
	private: System::Windows::Forms::ToolStripMenuItem^  �ֲ�ƥ�����߶�λToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  mRToolStripMenuItem1;

	private:
		/// <summary>
		/// ����������������
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �����֧������ķ��� - ��Ҫ
		/// ʹ�ô���༭���޸Ĵ˷��������ݡ�
		/// </summary>
		void InitializeComponent(void)
		{
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->mRToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->�ֲ�ƥ�����߶�λToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mRToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->mRToolStripMenuItem,
					this->�ֲ�ƥ�����߶�λToolStripMenuItem, this->mRToolStripMenuItem1
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
			this->mRToolStripMenuItem->Text = L"MRԤ����";
			this->mRToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::mRToolStripMenuItem_Click);
			// 
			// �ֲ�ƥ�����߶�λToolStripMenuItem
			// 
			this->�ֲ�ƥ�����߶�λToolStripMenuItem->Name = L"�ֲ�ƥ�����߶�λToolStripMenuItem";
			this->�ֲ�ƥ�����߶�λToolStripMenuItem->Size = System::Drawing::Size(116, 21);
			this->�ֲ�ƥ�����߶�λToolStripMenuItem->Text = L"�ֲ�ƥ�����߶�λ";
			this->�ֲ�ƥ�����߶�λToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::�ֲ�ƥ�����߶�λToolStripMenuItem_Click);
			// 
			// mRToolStripMenuItem1
			// 
			this->mRToolStripMenuItem1->Name = L"mRToolStripMenuItem1";
			this->mRToolStripMenuItem1->Size = System::Drawing::Size(76, 21);
			this->mRToolStripMenuItem1->Text = L"MR����";
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
private: System::Void �ֲ�ƥ�����߶�λToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
}
private: System::Void mRToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) {
}
};
}
