//The MIT License (MIT)
//
//Copyright (c) 2015-2020 Stanislav
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once

struct double2 {
	double x,y;
	double2():x(0),y(0){};
	double2(double x, double y):x(x),y(y){};
	void Set(const double& x_, const double& y_) { x = x_; y = y_; }
};

struct double3
{
	/// Default constructor does nothing (for performance).
	double3() {}
	/// Construct using coordinates.
	double3(double x, double y, double z) : x(x), y(y), z(z) {}
	/// Set this vector to some specified coordinates.
	void Set(const double& x_, const double& y_, const double& z_) { x = x_; y = y_; z = z_;}
	/// Negate this vector.
	double3 operator -() const { double3 v; v.Set(-x, -y, -z); return v; }
	/// Add a vector to this vector.
	void operator += (const double3& v){	x += v.x; y += v.y; z += v.z;}
	/// Subtract a vector from this vector.
	void operator -= (const double3& v){	x -= v.x; y -= v.y; z -= v.z;}
	/// Multiply this vector by a scalar.
	void operator *= (const double& a){	x *= a; y *= a; z *=a;}
	/// Get the length of this vector (the norm).
	double Length() const{ return sqrt(x * x + y * y + z * z);}
	/// Get the length squared.
	double Length2() const{ return x * x + y * y + z * z;}
	/// Convert this vector into a unit vector. Returns the length.
	void Normalize()
	{
		double length = Length();
		double invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}
	void rotatex(const double& a)
	{
		double tmp = cos(a) * y - sin(a) * z;
		z = sin(a) * y + cos(a) * z;
		y = tmp;
	}
	void rotatey(const double& a)
	{
		double tmp = cos(a) * x + sin(a) * z;
		z = - sin(a) * x + cos(a) * z;
		x = tmp;
	}
	double x, y, z;
};

inline double3 operator + (const double3& a, const double3& b){	return double3(a.x + b.x, a.y + b.y, a.z + b.z);};
inline double3 operator - (const double3& a, const double3& b){	return double3(a.x - b.x, a.y - b.y, a.z - b.z);};
inline double3 operator * (const double a, const double3& b){	return double3(a * b.x, a * b.y, a * b.z);};
inline double3 operator * (const double3& b, double a){	return double3(a * b.x, a * b.y, a * b.z);};
inline double3 operator / (const double3& a, double b){	return double3(a.x / b, a.y / b, a.z / b);};
inline double dot (const double3& a, const double3& b){	return a.x * b.x + a.y * b.y + a.z * b.z;};

template<typename T>
inline T clamp(const T&, double a, double b);

template<>
inline double clamp(const double& v, double a, double b)
{
	double x = v;
	if (v < a) {x = a;}
	if (v > b) {x = b;}
	return x;
}

template<>
inline double3 clamp(const double3& v, double a, double b)
{
	double3 x = v;
	x.x = clamp(x.x, a, b);
	x.y = clamp(x.y, a, b);
	x.z = clamp(x.z, a, b);
	return x;
}

template<>
inline double2 clamp(const double2& v, double a, double b)
{
	double2 x = v;
	x.x = clamp(x.x, a, b);
	x.y = clamp(x.y, a, b);
	return x;
}

inline int Round(double number)
{
    return static_cast<int>(number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5));
}
