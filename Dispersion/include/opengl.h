#pragma once 
#pragma warning(once : 4394)
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"User32.lib")

#include <queue>
#include <Windows.h>
#include <vector>
#include "GL/glew.h"

class OpenGL{
public:
	OpenGL();
	~OpenGL(void);
	bool Init(HDC hdc);
	void RenderScene();
	void ViewPort(int x, int y, int width, int height);
	bool CaptureScreen(int c);
	void SwapTexture();
	GLuint GetFrontTex();
	GLuint GetBackTex();
	void SetContext(int i);
	void GetTexSize(int& w,int& h);
	void ReshapeTex(int n,int m);
	void SetData(const std::vector<double>& data);
private:
	int OpenGL::IsGLExtensionSupported(const char *extension);
	void InitOpenGL();
	GLuint GenerateGLTex(int w,int h,int type,GLubyte* pixels=NULL);
private:
	HDC hdc_;
	HGLRC rc_;
	HGLRC rc_share_tex_;
	int gl_list_;
	std::vector<double> data_;
};