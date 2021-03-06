#pragma once

#include <SFML/Graphics.hpp>
#include "Vector.hpp"
#include <vector>
#include <string>

struct Triangle {
	Vector vertex[3];
	sf::Color color;

	Vector& operator[](int i) {
		return vertex[i];
	}
	Vector operator[](int i) const {
		return vertex[i];
	}
};

struct Mesh {
	std::vector<Triangle> tris;

	void loadFromObjectFile(std::string fileName);
};

struct Matrix4x4 {
	float m[4][4] = { 0 };

	static Matrix4x4 Identity();
	static Matrix4x4 RotationX(float theta);
	static Matrix4x4 RotationY(float theta);
	static Matrix4x4 RotationZ(float theta);
	static Matrix4x4 Translation(float x, float y, float z);
	static Matrix4x4 Projection(float zNear, float zFar, float fov, float aspectRatio);
	static Matrix4x4 PointAt(const Vector& pos, const Vector& target, const Vector& up);
	static Matrix4x4 QuickInverse(const Matrix4x4& mat);
};

Vector operator*(const Matrix4x4& mat, const Vector& u);
Triangle operator*(const Matrix4x4& mat, const Triangle& t);
Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b);