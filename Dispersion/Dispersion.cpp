// Dispersion.cpp: ����Ŀ�ļ���

#include "stdafx.h"
#include "MainWindow.h"

using namespace Dispersion;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// �ڴ����κοؼ�֮ǰ���� Windows XP ���ӻ�Ч��
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// ���������ڲ�������
	Application::Run(gcnew MainWindow());
	return 0;
}
