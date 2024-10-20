#pragma once

struct vector3
{
	float x, y, z;

	constexpr const vector3& operator+(const vector3& v) const
	{
		return vector3(x + v.x, y + v.y, z + v.z);
	}

	constexpr const vector3& operator-(const vector3& v) const
	{
		return vector3(x - v.x, y - v.y, z - v.z);
	}

	constexpr const vector3& operator/(const float N) const
	{
		return vector3(x / N, y / N, z / N);
	}

	constexpr const vector3& operator*(const float N) const
	{
		return vector3(x * N, y * N, z * N);
	}

};

struct viewMatrix
{
	float data[16];
};
