#include "Renderer3D.hpp"
#include <iostream>
#include <algorithm>


Renderer3D::Renderer3D(const sf::Vector2u& winSize)
{
	this->winSize = sf::Vector2f(winSize);

	cube.loadFromObjectFile("data/teapot.obj");

	matProj = Matrix4x4::Projection(-0.1, -1000.0f, 90.0f, (float)winSize.y / (float)winSize.x);

	fTheta = 0.f;

	camera = { 0,0,0 };
	lookDir = { 0,0,-1 };
	cameraAngle = { 0, 0, 0 };
}

Renderer3D::~Renderer3D()
{

}

void Renderer3D::clear()
{

	this->trianglestoRaster.clear();
}

void Renderer3D::update(float dt)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))cameraAngle.y -= 2 * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))cameraAngle.y += 2 * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))cameraAngle.x -= 2 * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))cameraAngle.x += 2 * dt;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))camera.x -= 5 * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))camera.x += 5 * dt;

	Vector forward = lookDir * 0.1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))camera = camera + forward;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))camera = camera - forward;


	Matrix4x4 matWorld =
		Matrix4x4::Identity() *
		Matrix4x4::RotationZ(fTheta * 0.5f) *
		Matrix4x4::RotationX(fTheta) *
		Matrix4x4::Translation(0, 0, -5.f);


	Matrix4x4 camRot = Matrix4x4::RotationX(cameraAngle.x) * Matrix4x4::RotationY(cameraAngle.y);
	lookDir = camRot * Vector{ 0,0,-1 };

	Matrix4x4 viewMat = Matrix4x4::PointAt(camera, lookDir, { 0,1,0 });

	sf::Color COLOR = sf::Color::White;
	COLOR.a = 0;

	for (auto& tri : cube.tris)
	{
		Triangle triTransformed = matWorld * tri;

		Vector normal = unit(cross(triTransformed[1] - triTransformed[0], triTransformed[2] - triTransformed[0]));

		if (dot(normal, triTransformed[0] - camera) < 0.0f) {

			Triangle triViewed = viewMat * triTransformed;

			Triangle triProjected = matProj * triViewed;

			triProjected[0] = triProjected[0] / triProjected[0].w;
			triProjected[1] = triProjected[1] / triProjected[1].w;
			triProjected[2] = triProjected[2] / triProjected[2].w;

			Vector offset{ 1, 1, 0 };
			triProjected[0] = triProjected[0] + offset;
			triProjected[1] = triProjected[1] + offset;
			triProjected[2] = triProjected[2] + offset;
			triProjected[0].x *= 0.5f * winSize.x; triProjected[0].y *= 0.5f * winSize.y;
			triProjected[1].x *= 0.5f * winSize.x; triProjected[1].y *= 0.5f * winSize.y;
			triProjected[2].x *= 0.5f * winSize.x; triProjected[2].y *= 0.5f * winSize.y;

			float dp = abs(dot(unit(lookDir * -1), normal));  //  lightDir * normal
			triProjected.color = sf::Color(COLOR.r * dp, COLOR.g * dp, COLOR.b * dp);

			trianglestoRaster.push_back(triProjected);
		}
	}

	sort(trianglestoRaster.begin(), trianglestoRaster.end(), [](Triangle& a, Triangle& b) {
		return (a[0].z + a[1].z + a[2].z) / 3.0f < (b[0].z + b[1].z + b[2].z) / 3.0f;
		});
}

void Renderer3D::drawTriangles(sf::RenderWindow& window)
{
	for (auto& tri : trianglestoRaster) {
		sf::Vertex faces[3] = {
			{{tri[0].x, tri[0].y}, tri.color },
			{{tri[1].x, tri[1].y}, tri.color},
			{{tri[2].x, tri[2].y}, tri.color}
		};
		sf::Vertex lines[4] = {
			{{tri[0].x, tri[0].y}, sf::Color::Red },
			{{tri[1].x, tri[1].y}, sf::Color::Red},
			{{tri[2].x, tri[2].y}, sf::Color::Red},
			{{tri[0].x, tri[0].y}, sf::Color::Red }
		};
		window.draw(faces, 3, sf::Triangles);
		//window.draw(lines, 4, sf::LinesStrip);
	}
}

void Renderer3D::render(sf::RenderWindow& window)
{
	this->drawTriangles(window);
}