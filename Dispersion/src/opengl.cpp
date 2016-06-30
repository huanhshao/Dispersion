#include "stdafx.h"
#include "opengl.h"
#include "multi_thread.h"
#include <string>
#include <math.h>
#include <fstream>

OpenGL::OpenGL(){
	hdc_=NULL;
	rc_=NULL;
	rc_share_tex_=NULL;
	gl_list_=0;
}
OpenGL::~OpenGL(void)
{  
	if (gl_list_!=0&&glIsList(gl_list_)){
		glDeleteLists(gl_list_,1);
		gl_list_=0;
	}
	wglMakeCurrent(0,0);
	wglDeleteContext(rc_);
	wglDeleteContext(rc_share_tex_);
	rc_=NULL;
	rc_share_tex_=NULL;
}
void OpenGL::SetData(const std::vector<double>& data){
	WaitForSingleObject(_mutex,INFINITE);
	data_.clear();
	data_.insert(data_.end(),data.begin(),data.end());
	ReleaseMutex(_mutex);
}
bool OpenGL::Init(HDC hdc){   
	hdc_=hdc;
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd,0,sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion=1;
	pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType=PFD_TYPE_RGBA;
	pfd.cColorBits=32;
	pfd.cDepthBits=32;
	pfd.iLayerType=PFD_MAIN_PLANE;
	int cpf=ChoosePixelFormat(hdc_,&pfd);
	if(cpf){
		SetPixelFormat(hdc_,cpf,&pfd);
		rc_=wglCreateContext(hdc_);
		rc_share_tex_=wglCreateContext(hdc_);
		wglShareLists(rc_share_tex_,rc_);
	}
	wglMakeCurrent(hdc_,rc_);
	glewInit();
	InitOpenGL();
	return true;
}
void OpenGL::InitOpenGL(){	
	glShadeModel(GL_SMOOTH);       // Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);       // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);        // The Type Of Depth Testing To D
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
GLuint OpenGL::GenerateGLTex(int w,int h,int type,GLubyte* pixels){
	GLuint tmp_tex=0;
	glGenTextures(1,&tmp_tex);
	if (tmp_tex!=0){
		glBindTexture(GL_TEXTURE_2D,tmp_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		if (type==0){
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F_ARB,w,h,0,GL_LUMINANCE,GL_FLOAT,NULL);
		}else if (type==1){
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F_ARB,w,h,0,GL_BGR_EXT,GL_UNSIGNED_BYTE,pixels);
		}
		glBindTexture(GL_TEXTURE_2D,0);
	}
	return tmp_tex;
}
void OpenGL::ViewPort(int x, int y, int width, int height)
{
	glViewport(x,y,width,height);
	glMatrixMode(GL_PROJECTION);		// Select The Projection Matrix
	glLoadIdentity();					// Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);			// Select The Modelview Matrix
	glLoadIdentity(); 
	gluLookAt(4,3,10,4,3,0,0,1,0);
	RenderScene();
}
void OpenGL::RenderScene(){
	WaitForSingleObject(_mutex,INFINITE);
	if( gl_list_ == 0 ){
		gl_list_= glGenLists(1);
		glNewList(gl_list_,GL_COMPILE);
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_LINES);
		glPointSize(10.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(-1.0f, 0.0f);//动态绘制x坐标  
		glVertex2f(1.0f, 0.0f);
		glVertex2f(0.0f, 1.0f);//动态绘制y坐标  
		glVertex2f(0.0f, -1.0f);
		glEnd();
		glEndList();
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
	glPushMatrix();
	glCallList(gl_list_);
	glBegin(GL_LINE_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < data_.size(); i++) {
		glVertex2f(double(i) / data_.size() * 2 - 1, data_[i]);
	}
	glEnd();
	glPopMatrix();
	SwapBuffers(hdc_);
	ReleaseMutex(_mutex);
}
bool OpenGL::CaptureScreen(int c){
	GLint viewport[4]; 
	glGetIntegerv( GL_VIEWPORT, viewport ); 
	GLint width  = viewport[2]; 
	GLint height = viewport[3]; 
	width -= width%4;
	GLubyte * bmpBuffer = NULL; 
	bmpBuffer = (GLubyte *)malloc(width*height*3*sizeof(GLubyte)); 
	if (!bmpBuffer) 
		return false; 
	glReadPixels(0, 0, width, height, 
		GL_BGR_EXT, GL_UNSIGNED_BYTE, bmpBuffer);  
	BITMAPFILEHEADER bitmapFileHeader; 
	BITMAPINFOHEADER bitmapInfoHeader; 
	bitmapFileHeader.bfType = 0x4D42;
	bitmapFileHeader.bfSize = width*height*3; 
	bitmapFileHeader.bfReserved1 = 0; 
	bitmapFileHeader.bfReserved2 = 0; 
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); 
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER); 
	bitmapInfoHeader.biWidth = width; 
	bitmapInfoHeader.biHeight = height; 
	bitmapInfoHeader.biPlanes = 1; 
	bitmapInfoHeader.biBitCount = 24; 
	bitmapInfoHeader.biCompression = BI_RGB; 
	bitmapInfoHeader.biSizeImage = 0; 
	bitmapInfoHeader.biXPelsPerMeter = 0; 
	bitmapInfoHeader.biYPelsPerMeter = 0; 
	bitmapInfoHeader.biClrUsed = 0; 
	bitmapInfoHeader.biClrImportant = 0; 
	char index[3];
	index[2]='\0';
	if (c>=100){
		c=c%100;
	}
	index[0]='0'+c/10;
	index[1]='0'+c%10;
	std::string filename="Capture\\grab"+std::string(index)+".bmp";
	std::ofstream ofs(filename,std::ios::out|std::ios::binary);
	if (!ofs){
		free(bmpBuffer);
		return false;
	}
	ofs.write((const char*)&bitmapFileHeader, sizeof(BITMAPFILEHEADER));
	ofs.write((const char*)&bitmapInfoHeader, sizeof(BITMAPINFOHEADER));
	ofs.write((const char*)bmpBuffer,width*height*3);
	ofs.close();
	free(bmpBuffer); 
	return true; 
}
void OpenGL::SetContext(int i){
	if (i==0){
		wglMakeCurrent(hdc_,0);
	}else{
		wglMakeCurrent(hdc_,rc_share_tex_);
	}
}
int OpenGL::IsGLExtensionSupported(const char *extension){
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;
	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '/0')
		return 0;
	extensions = glGetString(GL_EXTENSIONS);
	start = extensions;
	for (;;) {
		where = ( GLubyte* ) strstr( (const char *) start, extension );
		if ( !where ) 
			break;
		terminator = where + strlen(extension);
		if ( (where == start || *(where - 1) == ' ') && (*terminator == ' ' || *terminator == '/0') )
			return 1;
		start = terminator;
	}
	return 0;
}