#include "vec.h"
#include <corecrt_math.h>

Vector3::Vector3()
{
	
}

Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
	Vector3 ret;
	ret.x = this->x - rhs.x;
	ret.y = this->y - rhs.y;
	ret.z = this->z - rhs.z;
	return ret;
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
	Vector3 ret;
	ret.x = this->x + rhs.x;
	ret.y = this->y + rhs.y;
	ret.z = this->z + rhs.z;
	return ret;
}

Vector3 Vector3::operator*(const Vector3& rhs) const
{
	Vector3 ret;
	ret.x = this->x * rhs.x;
	ret.y = this->y * rhs.y;
	ret.z = this->z * rhs.z;
	return ret;
}

Vector3 Vector3::operator/(const Vector3& rhs) const
{
	Vector3 ret;
	ret.x = this->x / rhs.x;
	ret.y = this->y / rhs.y;
	ret.z = this->z / rhs.z;
	return ret;
}

Vector3 Vector3::operator-=(const Vector3& rhs) const
{
	Vector3 ret;
	ret.x = this->x;
	ret.y = this->y;
	ret.z = this->z;

	ret.x -= rhs.x;
	ret.y -= rhs.y;
	ret.z -= rhs.z;

	return ret;
}

Vector3 Vector3::operator+=(const Vector3& rhs) const
{
	Vector3 ret;
	ret.x = this->x;
	ret.y = this->y;
	ret.z = this->z;

	ret.x += rhs.x;
	ret.y += rhs.y;
	ret.z += rhs.z;

	return ret;
}

Vector3 Vector3::operator*=(const Vector3& rhs) const
{
	Vector3 ret;
	ret.x = this->x;
	ret.y = this->y;
	ret.z = this->z;

	ret.x *= rhs.x;
	ret.y *= rhs.y;
	ret.z *= rhs.z;

	return ret;
}

Vector3 Vector3::operator/=(const Vector3& rhs) const
{
	Vector3 ret;
	ret.x = this->x;
	ret.y = this->y;
	ret.z = this->z;

	ret.x /= rhs.x;
	ret.y /= rhs.y;
	ret.z /= rhs.z;

	return ret;
}

Vector3 Vector3::operator+(const float& rhs) const
{
	Vector3 ret;
	ret.x = this->x + rhs;
	ret.y = this->y + rhs;
	ret.z = this->z + rhs;

	return ret;
}

Vector3 Vector3::operator-(const float& rhs) const
{
	Vector3 ret;
	ret.x = this->x - rhs;
	ret.y = this->y - rhs;
	ret.z = this->z - rhs;

	return ret;
}

Vector3 Vector3::operator/(const float& rhs) const
{
	Vector3 ret;
	ret.x = this->x / rhs;
	ret.y = this->y / rhs;
	ret.z = this->z / rhs;

	return ret;
}

Vector3 Vector3::operator*(const float& rhs) const
{
	Vector3 ret;
	ret.x = this->x * rhs;
	ret.y = this->y * rhs;
	ret.z = this->z * rhs;

	return ret;
}

float Vector3::distance(Vector3 vector)
{
	float distance = 0.0f;
	distance += (float)pow(this->x - vector.x, 2);
	distance += (float)pow(this->y - vector.y, 2);
	distance += (float)pow(this->z - vector.z, 2);
	return (float)sqrt(distance);
}

float Vector3::Dot(const Vector3& rhs) const
{
	return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
}

bool Vector3::IsZero() const
{
	return this->x + this->y + this->z == 0;
}

float Vector3::distance_to(const Vector3& other)
{
	Vector3 delta;
	delta.x = x - other.x;
	delta.y = y - other.y;
	delta.z = z - other.z;

	return delta.length();
}

float Vector3::length()
{
	auto sqr = [](float n) {
		return static_cast<float>(n * n);
	};

	return sqrt(sqr(x) + sqr(y) + sqr(z));
}

Vector3 Vector3::Zero()
{
	Vector3 ret;
	ret.x = 0;
	ret.y = 0;
	ret.z = 0;
	return ret;
}

Vector4 Vector4::Zero()
{
	Vector4 ret;
	ret.x = 0.f;
	ret.y = 0.f;
	ret.z = 0.f;
	ret.w = 0.f;
	return ret;
}
Vector2::Vector2(){};
Vector2 Vector2::operator-(const Vector2& rhs) const
{
	Vector2 ret;
	ret.x = this->x - rhs.x;
	ret.y = this->y - rhs.y;
	return ret;
}

float Vector2::lenght()
{
	
return sqrt((x * x) + (y * y));
	
}

Vector2::Vector2(float x, float y)
{
	this->x = x;
	this->y = y;
}

