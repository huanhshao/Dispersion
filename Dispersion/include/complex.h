#pragma once
#define PI 3.141592653
struct Complex{
	double r;
	double i;
	Complex():r(0),i(0){}
	Complex(double real, double imag) :r(real), i(imag) {}
	Complex operator +(Complex& b) { return Complex(r + b.r, i + b.i); }
	double Length() { return sqrt(r*r + i*i); }
	double Power() { return Length()*Length(); }
};