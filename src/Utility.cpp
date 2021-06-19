#include "Utility.hpp"

#include <fstream>
#include <sstream>

void Mesh::loadFromObjectFile(std::string fileName)
{
	using namespace std;

	ifstream f(fileName);

	vector<Vector> verts;

	while (!f.eof())
	{
		string line;
		getline(f, line);

		stringstream s;
		s << line;

		char junk = 0;
		s >> junk;

		if (junk == 'v')
		{
			Vector v;
			s >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}
		if (junk == 'f')
		{
			int fn[3];
			string str, substr;

			for (int i = 0; i < 3; i++) {
				s >> str;
				stringstream sstr(str);
				getline(sstr, substr, '/');
				fn[i] = stoi(substr);
			}
			tris.push_back({ verts[fn[0] - 1], verts[fn[1] - 1], verts[fn[2] - 1] });
		}
	}
}

Matrix4x4 Matrix4x4::Identity()
{
	Matrix4x4 mat;
	mat.m[0][0] = 1;
	mat.m[1][1] = 1;
	mat.m[2][2] = 1;
	mat.m[3][3] = 1;
	return mat;
}

Matrix4x4 Matrix4x4::RotationX(float theta)
{
	Matrix4x4 mat;
	mat.m[0][0] = 1;
	mat.m[1][1] = cosf(theta);
	mat.m[1][2] = sinf(theta);
	mat.m[2][1] = -sinf(theta);
	mat.m[2][2] = cosf(theta);
	mat.m[3][3] = 1;
	return mat;
}

Matrix4x4 Matrix4x4::RotationY(float theta)
{
	Matrix4x4 mat;
	mat.m[0][0] = cosf(theta);
	mat.m[0][2] = sinf(theta);
	mat.m[2][0] = -sinf(theta);
	mat.m[1][1] = 1.0f;
	mat.m[2][2] = cosf(theta);
	mat.m[3][3] = 1.0f;
	return mat;
}

Matrix4x4 Matrix4x4::RotationZ(float theta)
{
	Matrix4x4 mat;
	mat.m[0][0] = cosf(theta);
	mat.m[0][1] = sinf(theta);
	mat.m[1][0] = -sinf(theta);
	mat.m[1][1] = cosf(theta);
	mat.m[2][2] = 1.0f;
	mat.m[3][3] = 1.0f;
	return mat;
}

Matrix4x4 Matrix4x4::Translation(float x, float y, float z)
{
	Matrix4x4 mat;
	mat.m[0][0] = 1.f;
	mat.m[1][1] = 1.f;
	mat.m[2][2] = 1.f;
	mat.m[3][3] = 1.f;
	mat.m[3][0] = x;
	mat.m[3][1] = y;
	mat.m[3][2] = z;
	return mat;
}

Matrix4x4 Matrix4x4::Projection(float zNear, float zFar, float fov, float aspectRatio)
{
	Matrix4x4 mat;

	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);

	mat.m[0][0] = aspectRatio * fovRad;
	mat.m[1][1] = fovRad;
	mat.m[2][2] = -zFar / (zFar - zNear);
	mat.m[3][2] = (-zFar * zNear) / (zFar - zNear);
	mat.m[2][3] = -1.0f;
	mat.m[3][3] = 0.0f;
	return mat;
}

Matrix4x4 Matrix4x4::PointAt(const Vector& pos, const Vector& dir, const Vector& up)
{
	Vector right = unit(cross(dir, up));
	Vector camUp = cross(right, dir);

	Matrix4x4 mat;
	mat.m[0][0] = right.x; mat.m[0][1] = right.y; mat.m[0][2] = right.z;
	mat.m[1][0] = camUp.x;    mat.m[1][1] = camUp.y;    mat.m[1][2] = camUp.z;
	mat.m[2][0] = -dir.x;  mat.m[2][1] = -dir.y;  mat.m[2][2] = -dir.z;
	mat.m[3][0] = pos.x;   mat.m[3][1] = pos.y;   mat.m[3][2] = pos.z;
	mat.m[3][3] = 1;
	
	return QuickInverse(mat);

}

Matrix4x4 Matrix4x4::QuickInverse(const Matrix4x4& mat) // Only for Rotation/Translation Matrices
{
	Matrix4x4 matrix;
	matrix.m[0][0] = mat.m[0][0]; matrix.m[0][1] = mat.m[1][0]; matrix.m[0][2] = mat.m[2][0]; 
	matrix.m[1][0] = mat.m[0][1]; matrix.m[1][1] = mat.m[1][1]; matrix.m[1][2] = mat.m[2][1]; 
	matrix.m[2][0] = mat.m[0][2]; matrix.m[2][1] = mat.m[1][2]; matrix.m[2][2] = mat.m[2][2];
	matrix.m[3][0] = -(mat.m[3][0] * matrix.m[0][0] + mat.m[3][1] * matrix.m[1][0] + mat.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(mat.m[3][0] * matrix.m[0][1] + mat.m[3][1] * matrix.m[1][1] + mat.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(mat.m[3][0] * matrix.m[0][2] + mat.m[3][1] * matrix.m[1][2] + mat.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

Vector operator*(const Matrix4x4& mat, const Vector& u)
{
	Vector v;
	v.x = u.x * mat.m[0][0] + u.y * mat.m[1][0] + u.z * mat.m[2][0] + u.w * mat.m[3][0];
	v.y = u.x * mat.m[0][1] + u.y * mat.m[1][1] + u.z * mat.m[2][1] + u.w * mat.m[3][1];
	v.z = u.x * mat.m[0][2] + u.y * mat.m[1][2] + u.z * mat.m[2][2] + u.w * mat.m[3][2];
	v.w = u.x * mat.m[0][3] + u.y * mat.m[1][3] + u.z * mat.m[2][3] + u.w * mat.m[3][3];
	return v;
}

Triangle operator*(const Matrix4x4& mat, const Triangle& t)
{
	Triangle tri;
	tri[0] = mat * t[0];
	tri[1] = mat * t[1];
	tri[2] = mat * t[2];
	return tri;
}

Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b)
{
	Matrix4x4 mat;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];

	return mat;
}