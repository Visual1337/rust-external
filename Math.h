#pragma once
#include <math.h>

#define M_PI 3.14159265358979323846f
#define M_RADPI	57.295779513082f
#define M_PI_F ((float)(M_PI))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))
#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))
#define atan2(a, b) ((float)atan2((double)(a), (double)(b)))

class Vector4
{
public:
	Vector4() : x(0.f), y(0.f), z(0.f), w(0.f)
	{

	}

	Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
	{

	}
	~Vector4()
	{

	}

	float x, y, z, w;
};

class Vector3
{
public:
	Vector3();
	Vector3(float _x, float _y, float _z);
	~Vector3();

	float x, y, z;

	Vector3 operator *(Vector3 a);
	Vector3 operator *(float f);
	Vector3 operator /(Vector3 a);
	Vector3 operator /(float f);
	Vector3 operator +(Vector3 a);
	Vector3 operator -(Vector3 a);
	bool operator ==(Vector3 a);
	bool operator !=(Vector3 a);

	bool IsZero() const;

	Vector4 Quaternion()
	{
		double heading = DEG2RAD(x);
		double attitude = DEG2RAD(y);
		double bank = DEG2RAD(z);

		double c1 = cos(heading / 2);
		double s1 = sin(heading / 2);
		double c2 = cos(attitude / 2);
		double s2 = sin(attitude / 2);
		double c3 = cos(bank / 2);
		double s3 = sin(bank / 2);
		double c1c2 = c1 * c2;
		double s1s2 = s1 * s2;
		Vector4 Quat;
		Quat.w = c1c2 * c3 - s1s2 * s3;
		Quat.x = c1c2 * s3 + s1s2 * c3;
		Quat.y = s1 * c2 * c3 + c1 * s2 * s3;
		Quat.z = c1 * s2 * c3 - s1 * c2 * s3;
		Vector4 Quat2;
		Quat2 = { Quat.y, Quat.z, (Quat.x * -1), Quat.w };
		return Quat2;
	}

	static float Dot(Vector3 left, Vector3 right);
	static float Distance(Vector3 a, Vector3 b);
	static int FormattedDistance(Vector3 a, Vector3 b);
	static Vector3 Zero();
	static Vector3 Lerp(Vector3 a, Vector3 b, float t);

	float Length() const;
	float LengthSqr() const;

	Vector3 Clamp() const;
};

struct ViewMatrix
{
public:
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
		}; float m[4][4];
	};

	Vector3 Transform(const Vector3 vector)
	{
	}
};

class Vector2
{
public:
	Vector2();
	Vector2(float _x, float _y);
	~Vector2();

	float x, y;

	Vector2 operator *(Vector2 a);
	Vector2 operator /(Vector2 a);
	Vector2 operator +(Vector2 a);
	Vector2 operator -(Vector2 a);
	bool operator ==(Vector2 a);
	bool operator !=(Vector2 a);

	void normalize()
	{

		if (x < -89)
			x = -89;


		else if (x > 89)
			x = 89;


		if (y < -360)
			y += 360;


		else if (y > 360)
			y -= 360;
	}

	bool IsZero() const;

	static Vector2 Zero();

	static float Distance(Vector2 a, Vector2 b);
}; 

inline double to_rad(double degree)
{
	double pi = 3.14159265359;
	return (degree * (pi / 180));
}

inline Vector4 to_quat(Vector3 Euler)
{
	double heading = to_rad(Euler.x);
	double attitude = to_rad(Euler.y);
	double bank = to_rad(Euler.z);

	double c1 = cos(heading / 2);
	double s1 = sin(heading / 2);
	double c2 = cos(attitude / 2);
	double s2 = sin(attitude / 2);
	double c3 = cos(bank / 2);
	double s3 = sin(bank / 2);
	double c1c2 = c1 * c2;
	double s1s2 = s1 * s2;
	Vector4 quat;
	quat.w = c1c2 * c3 - s1s2 * s3;
	quat.x = c1c2 * s3 + s1s2 * c3;
	quat.y = s1 * c2 * c3 + c1 * s2 * s3;
	quat.z = c1 * s2 * c3 - s1 * c2 * s3;
	return { quat.y, quat.z, (quat.x * -1), quat.w };
}

inline Vector2 CalculateAngle(const Vector3& player, const Vector3& target)
{
	Vector3 direction = Vector3(player.x - target.x, player.y - target.y, player.z - target.z);
	return Vector2(RAD2DEG(asin(direction.y / direction.Length())), RAD2DEG(-atan2(direction.x, -direction.z)));
}

inline Vector3 CalcAngle(Vector3 enemyPos, Vector3 cameraPos)
{
	Vector3 dir = enemyPos - cameraPos;
	float x = asinf(dir.z / dir.Length()) * 57.2957795131f;

	float z = atanf(dir.y / dir.x) * 57.2957795131f;

	if (dir.x >= 0.f) z += 180.f;
	if (x > 179.99f) x -= 360.f;
	else if (x < -179.99f) x += 360.f;

	return Vector3(x, 0.f, z + 90.f);
}