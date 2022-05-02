#pragma once

#define CHECK_VALID(_v) 0
#define Assert( _exp ) ((void)0)

struct Vector2
{
public:
	float x;
	float y;
	Vector2 operator-(const Vector2& rhs) const;
	float lenght();
	Vector2(float x, float y);
	Vector2();
};

struct Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);
	Vector3 operator-(const Vector3& rhs) const;
	Vector3 operator-=(const Vector3& rhs) const;
	Vector3 operator+(const Vector3& rhs) const;
	Vector3 operator*(const Vector3& rhs) const;
	Vector3 operator/(const Vector3& rhs) const;
	Vector3 operator+=(const Vector3& rhs) const;
	Vector3 operator*=(const Vector3& rhs) const;
	Vector3 operator/=(const Vector3& rhs) const;
	Vector3 operator+(const float& rhs) const;
	Vector3 operator-(const float& rhs) const;
	Vector3 operator/(const float& rhs) const;
	Vector3 operator*(const float& rhs) const;
	float distance(Vector3 vector);
	Vector3 Zero();
	float Dot(const Vector3& rhs) const;
	bool IsZero()const;
	float distance_to(const Vector3& other);
	float length();

	
public:
	float x;
	float y;
	float z;
};

struct Vector4
{
public:
	float x;
	float y;
	float z;
	float w;
	Vector4 Zero();
};

struct Matrix3x4
{
	float m[3][4];
};

struct Matrix4x3
{
	float m[4][3];
};

struct Matrix4x4
{
	union { Vector4 v[4]; float m[4][4]; struct { Vector4 right; Vector4 up; Vector4 forward; Vector4 trans; }; };
};


