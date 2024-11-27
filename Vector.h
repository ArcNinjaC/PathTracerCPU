#pragma once
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <cstdlib>  // For rand() and RAND_MAX
#include <ctime>    // For time()

// Call this once at the beginning of your program to seed the random number generator


struct Vec3
{
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;

	//Vec3() = default;
	Vec3() {}
	Vec3(double s): x(s), y(s), z(s) {}
	Vec3(double x, double y) : x(x), y(y), z(0.0) {}
	Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

	static Vec3 from_spherical(double theta = 0.0, double phi = 0.0)
	{
		// some error possible
		double ct = std::cos(theta);
		double st = std::sin(theta);
		double cp = std::cos(phi);
		double sp = std::sin(phi);



		double x_ = -st * cp;
		double y_ = sp;
		double z_ = ct * cp;
		
		//std::cout << z_;

		return Vec3(x_, y_, z_).normalized();
	}

	static Vec3 random1()
	{
		while (true)
		{
			double x = ((double)rand() / (RAND_MAX));
			double y = ((double)rand() / (RAND_MAX));
			double z = ((double)rand() / (RAND_MAX));
			x -= 0.5;
			x *= 2.0;
			y -= 0.5;
			y *= 2.0;
			z -= 0.5;
			z *= 2.0;

			if (Vec3(x, y, z).length() <= 1.0)
			{
				return Vec3(x, y, z).normalized();
				break;
			}
		}
	}

	Vec3& ApplyRotateTransformes(Vec3 orientation)
	{
		double cy = std::cos(orientation.y); double sy = std::sin(orientation.y);
		double new_x = x * cy - z * sy;
		double new_z = x * sy + z * cy;

		double cx = std::cos(orientation.x); double sx = std::sin(orientation.x);
		double new_y = y * cx - new_z * sx;
		new_z = y * sx + new_z * cx;

		double cz = std::cos(orientation.z); double sz = std::sin(orientation.z);
		double new_new_x = new_x * cz - new_y * sz;
		new_y = new_x * sz + new_y * cz;

		x = new_new_x;
		y = new_y;
		z = new_z;


		return *this;
	}

	Vec3 cross(const Vec3& other) const
	{
		Vec3 ans;
		ans.x = this->y * other.z - this->z * other.y;
		ans.y = this->z * other.x - this->x * other.z;
		ans.z = this->x * other.y - this->y * other.x;
		
		return ans;
	}

	double dot(const Vec3& other) const
	{
		return this->x * other.x + this->y * other.y + this->z * other.z;
	}

	double length2() const
	{
		return this->dot(*this);
	}

	double length() const
	{
		return std::sqrt(this->length2());
	}

	Vec3 normalized() const
	{
		if (this->length() == 0) { return *this; }
		return *this / this->length();
	}

	Vec3& normalize()
	{
		*this = this->normalized();
		return *this;
	}

	Vec3 operator+(const Vec3& other) const
	{
		return Vec3(this->x + other.x, this->y + other.y, this->z + other.z);
	}

	Vec3& operator+=(const Vec3& other)
	{
		*this = *this + other;
		return *this;
	}

	Vec3 operator+(double other) const
	{
		return Vec3(this->x + other, this->y + other, this->z + other);
	}

	Vec3& operator+=(double other)
	{
		*this = *this + other;
		return *this;
	}

	Vec3 operator-(const Vec3& other) const
	{
		return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
	}

	Vec3& operator-=(const Vec3& other)
	{
		*this = *this - other;
		return *this;
	}

	Vec3 operator*(const Vec3& other) const
	{
		return Vec3(this->x * other.x, this->y * other.y, this->z * other.z);
	}

	Vec3& operator*=(const Vec3& other)
	{
		*this = *this * other;
		return *this;
	}

	Vec3 operator*(const double& other) const
	{
		return Vec3(this->x * other, this->y * other, this->z * other);
	}

	Vec3& operator*=(const double& other)
	{
		*this = *this * other;
		return *this;
	}

	Vec3 operator/(double other) const
	{
		if (other == 0.0)
		{
			//return 1000000.0f;
			throw std::runtime_error("cant divide by zero!!! Vec3 class");
		}

		return *this * (1.0 / other);
	}

	Vec3& operator/=(const double& other)
	{
		*this = *this / other;
		return *this;
	}

	bool operator==(const Vec3& other)
	{
		return (x == other.x) && (y = other.y) && (z == other.z);
	}
};