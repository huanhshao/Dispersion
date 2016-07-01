#include <Windows.h>
#include <cmath>
#include <vector>
#include <iostream>
using namespace std;
#include "complex.h"
#include "multi_thread.h"

HANDLE _mutex = CreateMutex(nullptr, false, nullptr);
HANDLE _print = CreateEvent(nullptr, true, true, nullptr);

unsigned __stdcall RunSim(void* lpParam){
	SimParam* sim_param=reinterpret_cast<SimParam*>(lpParam);

	double dt = 1e-12;
	double fps = 100e3;
	int num_points = int(1 / fps / dt);
	vector<double> t(num_points), wl(num_points), ws(num_points), phi(num_points);
	vector<double> ts(num_points);
	vector<Complex> ps(num_points),sps(num_points);
	double wl0 = 1310e-9;
	double Dw = 50e-9;
	double n = 1.4672;
	double c = 3e8;
	double s0=86;
	double l=1;
	for (int i = 0; i < num_points; i++) {
		t[i] = dt*i;
		wl[i] = wl0 + Dw*cos(2*PI*fps*t[i]);
		ws[i] = c / n / wl[i];
		ts[i]=(n/c+s0/8*(wl[i]*wl[i]-wl0*wl0+wl0*wl0*wl0*wl0*(1/wl[i]/wl[i]-1/wl0/wl0)))*l;
	}
	double mm=-1;double mn=1;
	for (int i=0;i<ts.size();i++){
		if (mm<ts[i]) mm=ts[i];
		if (mn>ts[i]) mn=ts[i];
	}
	cout <<mm<<' '<<mn<<' '<<mm-mn<<endl;
	phi[0] = PI / 3;
	for (int i = 1; i < num_points; i++) {
		phi[i] = phi[i - 1] + 2 * PI * ws[i] * dt;
		ps[i].r = cos(phi[i]);
		ps[i].i = sin(phi[i]);
	}
	vector<double> output(sps.size());
	double dl=1e-3;
	//for (double dl = 300e-6; dl < 3000e-6; dl += 100e-6) {
		if (WaitForSingleObject(sim_param->end_thread, 0) == WAIT_OBJECT_0){
			//break;
		}
		double td = dl*n / c;
		int ti = int(td / dt);
		sim_param->dl=ti;
		for (int i = 0; i < ps.size(); i++) {
			int j = i - ti;
			if (j < 0) j += ps.size();
			sps[i] = ps[i] + ps[j];
		}
		double maxi = 0;
		for (int i = 0; i < sps.size(); i++) {
			output[i] = sps[i].Power();
			if (maxi < output[i]) {
				maxi = output[i];
			}
		}
		//if (maxi==0) continue;
		for (int i = 0; i < output.size(); i++) {
			output[i] = 2 * output[i] / maxi - 1;
		}
		vector<double> ooxx(2000);
		int step=int(double(output.size())/ooxx.size()/2);
		for (int i=0;i<ooxx.size();i++){
			ooxx[i]=output[i*step];
		}
		//WaitForSingleObject(_print, INFINITE);
		//ResetEvent(_print);
		sim_param->gl->SetData(ooxx);
		sim_param->gl->RenderScene();
	//}
	_endthreadex(0);
	return 0;
}