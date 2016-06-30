#include "stdafx.h"
#include <Windows.h>
#include <cmath>
#include <vector>
using namespace std;
#include "complex.h"
#include "multi_thread.h"

HANDLE _mutex = CreateMutex(nullptr, false, nullptr);
HANDLE _print = CreateEvent(nullptr, true, true, nullptr);

unsigned __stdcall RunSim(void* lpParam){
	SimParam* sim_param=reinterpret_cast<SimParam*>(lpParam);
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
	vector<double> output(sps.size());
	for (double dl = 300e-6; dl < 3000e-6; dl += 100e-6) {
		if (WaitForSingleObject(sim_param->end_thread, 0) == WAIT_OBJECT_0){
			break;
		}
		double td = dl*n / c;
		int ti = int(td / dt);
		sim_param->dl=ti;
		double max = 0;
		for (int i = 0; i < ps.size(); i++) {
			int j = i - ti;
			if (j < 0) j += ps.size();
			sps[i] = ps[i] + ps[j];
		}
		for (int i = 0; i < sps.size(); i++) {
			output[i] = sps[i].Power();
			if (max < output[i]) {
				max = output[i];
			}
		}
		if (max==0) continue;
		for (int i = 0; i < output.size(); i++) {
			output[i] = 2 * output[i] / max - 1;
		}
		vector<double> ooxx(2000);
		int step=output.size()/ooxx.size();
		for (int i=0;i<ooxx.size();i++){
			ooxx[i]=output[i*step];
		}
		//WaitForSingleObject(_print, INFINITE);
		//ResetEvent(_print);
		sim_param->gl->SetData(ooxx);
		sim_param->gl->RenderScene();
	}
	_endthreadex(0);
	return 0;
}