#include "Renderer3D.hpp"
#include "BlinkingColors.hpp"
#include <iostream>
#include <algorithm>


Renderer3D::Renderer3D(const sf::Vector2u& winSize)
{
	renderingMode = RENDER_MODE::TRIANGLE;
	cube.loadFromObjectFile("data/idk.obj");

	matProj = Matrix4x4::Projection(0.1, 1000.0f, 90.0f, (float)winSize.y / (float)winSize.x);

	fTheta = 0.f;

	camera = { 0,0,0 };

	pixel_y = winSize.y; pixel_x = winSize.x;
	pixels = new sf::Vertex[pixel_x * pixel_y];
	zBuffer = new float[pixel_x * pixel_y];

	for (int i = 0; i < pixel_x * pixel_y; i++) {
		pixels[i].position = sf::Vector2f(i % pixel_x, i / pixel_x);
		zBuffer[i] = INFINITY;
	}
}

Renderer3D::~Renderer3D()
{
	delete[] pixels;
}

void Renderer3D::clearPixels()
{
	for (int i = 0; i < pixel_x * pixel_y; i++){
		pixels[i].color = sf::Color::Black;
		zBuffer[i] = INFINITY;
	}
}


void Renderer3D::scaleToView(Triangle& tri, const sf::Vector2f& winSize)
{
	Vector offset{ 1, 1 };
	tri[0] = tri[0] + offset;
	tri[1] = tri[1] + offset;
	tri[2] = tri[2] + offset;
	tri[0].x *= 0.5f * winSize.x; tri[0].y *= 0.5f * winSize.y;
	tri[1].x *= 0.5f * winSize.x; tri[1].y *= 0.5f * winSize.y;
	tri[2].x *= 0.5f * winSize.x; tri[2].y *= 0.5f * winSize.y;
}

bool pointInTri(const Vector& A, const Vector& B, const Vector& C, const Vector& P)
{
	Vector v0 = C - A, v1 = B - A, v2 = P - A;

	float dot00 = dot(v0, v0);
	float dot01 = dot(v0, v1);
	float dot02 = dot(v0, v2);
	float dot11 = dot(v1, v1);
	float dot12 = dot(v1, v2);

	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return ((u >= 0) && (v >= 0) && (u + v < 1));
}

void Renderer3D::updatePixel(const Vector& v, const sf::Color& c)
{
	int k = v.y * pixel_x + v.x;
	if (k >= 0 && k < pixel_x * pixel_y && zBuffer[k] > v.z)
	{
		zBuffer[k] = v.z;
		pixels[k].color = c;
	}
}

void Renderer3D::drawTriangleToPixels(const Triangle& tri)
{
	int min_x = std::min(std::min(tri[0].x, tri[1].x), tri[2].x); min_x = std::max(min_x, 0);
	int max_x = std::max(std::max(tri[0].x, tri[1].x), tri[2].x); max_x = std::min(max_x, pixel_x - 1);
	int min_y = std::min(std::min(tri[0].y, tri[1].y), tri[2].y); min_y = std::max(min_y, 0);
	int max_y = std::max(std::max(tri[0].y, tri[1].y), tri[2].y); max_y = std::min(max_y, pixel_y - 1);

	Vector n = cross(tri[1] - tri[0], tri[2] - tri[0]);
	float d = dot(n, tri[0]);

	for (int x = min_x; x <= max_x; x++) {
		for (int y = min_y; y <= max_y; y++) {
			Vector P{ x, y };
			if (pointInTri({ tri[0].x, tri[0].y }, { tri[1].x, tri[1].y }, { tri[2].x, tri[2].y }, P)) {
				float z = (d - x * n.x - y * n.y) / n.z;
				this->updatePixel({ (float)x, (float)y, z }, tri.color);
			}
		}
	}
}



void Renderer3D::updatePixels(float dt)
{
	fTheta += 0.1;
	
	Matrix4x4 matWorld = 
		Matrix4x4::Identity() *
		Matrix4x4::RotationZ(fTheta) *
		Matrix4x4::RotationX(fTheta) *
		Matrix4x4::Translation(0, 0, 5.f);

	sf::Color COLOR = sf::Color::White;
	COLOR.a = 0;

	for (auto& tri : cube.tris)
	{
		Triangle triTransformed = matWorld * tri;

		Vector normal = unit(cross(triTransformed[1] - triTransformed[0], triTransformed[2] - triTransformed[0]));

		if (dot(normal, triTransformed[0] - camera) < 0.0f) {

			float dp = dot(unit({ 0, 0, -1 }), normal);  //  lightDir * normal

			Triangle triProjected = matProj * triTransformed;

			triProjected[0] = triProjected[0] / triProjected[0].w;
			triProjected[1] = triProjected[1] / triProjected[1].w;
			triProjected[2] = triProjected[2] / triProjected[2].w;

			this->scaleToView(triProjected, sf::Vector2f(pixel_x, pixel_y));

			triProjected.color = COLOR + sf::Color(0, 0, 0, 255 * dp);
			this->drawTriangleToPixels(triProjected);
		}
	}
}

void Renderer3D::drawTriangles(sf::RenderWindow& window, float dt)
{
	fTheta += 0.05;

	Matrix4x4 matWorld =
		Matrix4x4::Identity() *
		Matrix4x4::RotationZ(fTheta) *
		Matrix4x4::RotationX(fTheta) *
		Matrix4x4::Translation(0, 0, 5.f);

	sf::Color COLOR = sf::Color::White;
	COLOR.a = 0;

	std::vector<Triangle> drawables;

	for (auto& tri : cube.tris)
	{
		Triangle triTransformed = matWorld * tri;

		Vector normal = unit(cross(triTransformed[1] - triTransformed[0], triTransformed[2] - triTransformed[0]));

		if (dot(normal, triTransformed[0] - camera) < 0.0f) {

			float dp = dot(unit({ 0, 0, -1 }), normal);  //  lightDir * normal

			Triangle triProjected = matProj * triTransformed;

			triProjected[0] = triProjected[0] / triProjected[0].w;
			triProjected[1] = triProjected[1] / triProjected[1].w;
			triProjected[2] = triProjected[2] / triProjected[2].w;

			this->scaleToView(triProjected, sf::Vector2f(pixel_x, pixel_y));

			triProjected.color = COLOR + sf::Color(0, 0, 0, 255 * dp);
			drawables.push_back(triProjected);
		}
	}

	sort(drawables.begin(), drawables.end(), [](Triangle& a, Triangle& b) {
		return (a[0].z + a[1].z + a[2].z) / 3 > (b[0].z + b[1].z + b[2].z) / 3;
	});

	for (auto& tri : drawables) {
		sf::Vertex faces[3] = {
			{{tri[0].x, tri[0].y}, tri.color },
			{{tri[1].x, tri[1].y}, tri.color},
			{{tri[2].x, tri[2].y}, tri.color}
		};
		window.draw(faces, 3, sf::Triangles);
	}
}

void Renderer3D::drawPixels(sf::RenderWindow& window)
{
	window.draw(pixels, pixel_x * pixel_y, sf::Points);
}

void Renderer3D::render(sf::RenderWindow& window, float dt)
{
	if (renderingMode == RENDER_MODE::PIXEL) {
		this->clearPixels();
		this->updatePixels(dt);
		this->drawPixels(window);
	}

	else if (renderingMode == RENDER_MODE::TRIANGLE){
		this->drawTriangles(window, dt);
	}
}
