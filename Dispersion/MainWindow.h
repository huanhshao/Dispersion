#pragma once
#include <Windows.h>
#include <vector>
#include "opengl.h"
#include "multi_thread.h"
namespace Dispersion {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MainWindow 摘要
	/// </summary>
	public ref class MainWindow : public System::Windows::Forms::Form
	{
	public:
		MainWindow(void)
		{
			InitializeComponent();
			gl_=new OpenGL();
			HDC dc = GetDC((HWND)(this->Handle.ToInt32()));
			gl_->Init(dc);
			gl_->RenderScene();
			sim_param_=new SimParam();
			sim_param_->gl=gl_;
			sim_param_->thread_handle=(HANDLE)_beginthreadex(nullptr,0,RunSim,sim_param_,false,nullptr);
			//
			//TODO: 在此处添加构造函数代码
			//
		}

	protected:
		/// <summary>
		/// 清理所有正在使用的资源。
		/// </summary>
		~MainWindow(){
			if (components){
				delete components;
			}
			if (gl_){
				delete gl_;
			}
			if (sim_param_){
				delete sim_param_;
			}
		}
	private: System::ComponentModel::IContainer^  components;
	protected: 
	private: System::Windows::Forms::Timer^  timer1;

	private:
		/// <summary>
		/// 必需的设计器变量。
		/// </summary>

		OpenGL* gl_;
	private: System::Windows::Forms::Label^  label1;
			 SimParam* sim_param_;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 设计器支持所需的方法 - 不要
		/// 使用代码编辑器修改此方法的内容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// timer1
			// 
			this->timer1->Interval = 30;
			this->timer1->Tick += gcnew System::EventHandler(this, &MainWindow::timer1_Tick);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(379, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(41, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"label1";
			// 
			// MainWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(792, 566);
			this->Controls->Add(this->label1);
			this->Name = L"MainWindow";
			this->Text = L"MainWindow";
			this->Load += gcnew System::EventHandler(this, &MainWindow::MainWindow_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void MainWindow_Load(System::Object^  sender, System::EventArgs^  e) {
				 this->timer1->Enabled = true;
			 }
	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
				 this->label1->Text=System::Convert::ToString(sim_param_->dl);
				 gl_->RenderScene();
			 }
	};
}
