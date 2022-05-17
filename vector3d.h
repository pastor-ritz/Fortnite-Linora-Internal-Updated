#pragma once
#include <cmath>
#include <Windows.h>
#include <d3d9types.h>
#include <iostream>



class Vector2
{
public:
	double x, y;
	Vector2()
	{
		x = y = 0.f;
	}

	Vector2(double _x, double _y)
	{
		x = _x;
		y = _y;
	}

	Vector2 operator+(const Vector2& v) {
		return Vector2{ x + v.x, y + v.y };
	}


	Vector2 operator-(const Vector2& v) {
		return Vector2{ x - v.x, y - v.y };
	}

	double DistanceFrom(const Vector2& Other) const {
		const Vector2& a = *this;
		double dx = (a.x - Other.x);
		double dy = (a.y - Other.y);

		return (sqrt((dx * dx) + (dy * dy)));
	}
};

class Vector3
{
public:
	double x, y, z;
	Vector3()
	{
		x = y = z = 0.f;
	}

	Vector3(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3 operator+(const Vector3& v) {
		return Vector3{ x + v.x, y + v.y, z + v.z };
	}

	Vector3 operator-(const Vector3& v) {
		return Vector3{ x - v.x, y - v.y, z - v.z };
	}

	Vector3 operator*(const double v) {
		return Vector3{ x * v, y * v, z * v };
	}

	Vector3 operator/(const double fl) const {
		return Vector3(x / fl, y / fl, z / fl);
	}

	inline double DistanceFrom(const Vector3 Other) {
		double dx = (this->x - Other.x);
		double dy = (this->y - Other.y);
		double dz = (this->z - Other.z);

		return sqrt((dx * dx) + (dy * dy) + (dz * dz));
	}
	inline double Distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}
	inline double Length() {
		return sqrt(x * x + y * y + z * z);
	}
};