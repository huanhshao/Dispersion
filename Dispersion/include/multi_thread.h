#pragma once
#include <Windows.h>
#include <process.h>
#include "opengl.h"

extern HANDLE _mutex;// = CreateMutex(nullptr, false, nullptr);
extern HANDLE _print;// = CreateEvent(nullptr, true, true, nullptr);
unsigned __stdcall RunSim(void* lpParam);

typedef class _SSimThread
{
public:
	_SSimThread(){
		thread_handle = NULL;
		end_thread = CreateEvent(NULL, false, false, NULL);
		dl=0;
	}
	~_SSimThread(){
		EndThread();
		CloseHandle(end_thread);
		end_thread = NULL;
	}
	void EndThread(){
		if (thread_handle){
			SetEvent(end_thread);
			WaitForSingleObject(thread_handle, INFINITE);
			CloseHandle(thread_handle);
			thread_handle = NULL;
		}
	}
public:
	HANDLE thread_handle;
	HANDLE end_thread;
	OpenGL* gl;
	double dl;
}SimParam;