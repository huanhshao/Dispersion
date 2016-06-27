#include <iostream>
#include <vector>
#include <cmath>
#include <mutex>
#include <Windows.h>
#include <process.h>
#include "include\GL\glut.h"
using namespace std;
const double PI = 3.1415926;
HANDLE _mutex = CreateMutex(nullptr, false, nullptr);
HANDLE _print = CreateEvent(nullptr, true, true, nullptr);
HANDLE _count = CreateEvent(nullptr, true, false, nullptr);
vector<double> my_data;
void Timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(20, Timer, 1);
}
void RenderScene(void) {
    if (WaitForSingleObject(_count, 0) != WAIT_OBJECT_0) {
        glutSwapBuffers();
        return;
    }
    WaitForSingleObject(_mutex, INFINITE);
    ResetEvent(_count);
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
    glPointSize(10.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(-1.0f, 0.0f);//动态绘制x坐标  
    glVertex2f(1.0f, 0.0f);
    glVertex2f(0.0f, 1.0f);//动态绘制y坐标  
    glVertex2f(0.0f, -1.0f);
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < my_data.size(); i++) {
        glVertex2f(double(i) / my_data.size() * 2 - 1, my_data[i]);
    }
    glEnd();
    glFlush();
    glutSwapBuffers();
    SetEvent(_print);
    ReleaseMutex(_mutex);
}
struct Complex
{
    double r;
    double i;
    Complex():r(0),i(0){}
    Complex(double real, double imag) :r(real), i(imag) {}
    Complex operator +(Complex& b) { return Complex(r + b.r, i + b.i); }
    double Length() { return sqrt(r*r + i*i); }
    double Power() { return Length()*Length(); }
};
struct dd {
    int* argc;
    char** argv;
};
static unsigned __stdcall GlutPlot(void * pParam) {
    dd* d = (dd*)pParam;
    glutInit(d->argc, d->argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1024,768);
    glutCreateWindow("GLUT Tutorial");
    glutDisplayFunc(RenderScene);
    //glutTimerFunc(20, Timer, 1);
    glutMainLoop();
    _endthreadex(0);
    return 0;
}

int main(int argc, char**argv) {
    dd d;
    d.argc = &argc;
    d.argv = argv;
    HANDLE thread_handle = (HANDLE)_beginthreadex(nullptr, 0, GlutPlot, &d, false, nullptr);
    double dt = 1e-12;
    double fps = 100e3;
    int num_points = 1 / fps / dt;
    vector<double> t(num_points), wl(num_points), ws(num_points), phi(num_points);
    vector<Complex> ps(num_points),sps(num_points);
    double wl0 = 1310e-9;
    double Dw = 50e-9;
    double n = 1.46;
    double c = 3e8;
    for (int i = 0; i < num_points; i++) {
        t[i] = dt*i;
        wl[i] = wl0 + Dw*cos(2 * PI*fps*t[i]);
        ws[i] = c / n / wl[i];
    }
    phi[0] = PI / 3;
    for (int i = 1; i < num_points; i++) {
        phi[i] = phi[i - 1] + 2 * PI * ws[i] * dt;
        ps[i].r = cos(phi[i]);
        ps[i].i = sin(phi[i]);
    }
    for (double dl = 300e-6; dl < 3000e-6; dl += 100e-6) {
        double td = dl*n / c;
        int ti = round(td / dt);
        my_data.resize(sps.size());
        double max = 0;
        for (int i = 0; i < ps.size(); i++) {
            int j = i - ti;
            if (j < 0) j += ps.size();
            sps[i] = ps[i] + ps[j];
        }
        WaitForSingleObject(_print, INFINITE);
        WaitForSingleObject(_mutex, INFINITE);
        ResetEvent(_print);
        for (int i = 0; i < sps.size(); i++) {
            my_data[i] = sps[i].Power();
            if (max < my_data[i]) {
                max = my_data[i];
            }
        }
        if (max == 0) {
            cout << "ERROR!!!" << endl;
        }
        for (int i = 0; i < my_data.size(); i++) {
            my_data[i] = 2 * my_data[i] / max - 1;
        }
        SetEvent(_count);
        ReleaseMutex(_mutex);
        glutPostRedisplay();
    }
    CloseHandle(thread_handle);
    thread_handle = nullptr;
    return 0;
}