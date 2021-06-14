#include "Renderer3D.hpp"
#include <iostream>
#include <algorithm>


Renderer3D::Renderer3D(const sf::Vector2u& winSize)
{
	renderingMode = RENDER_MODE::TRIANGLE;
	cube.loadFromObjectFile("data/teapot.obj");

	matProj = Matrix4x4::Projection(0.1, 1000.0f, 90.0f, (float)winSize.y / (float)winSize.x);

	fTheta = 0.f;
	yaw = 0.f;

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
	Vector offset{ 1, 1, 0 };
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

void Renderer3D::clear()
{
	if (renderingMode == RENDER_MODE::PIXEL) {
		this->clearPixels();
	}
	else if (renderingMode == RENDER_MODE::TRIANGLE){
		this->trianglestoRaster.clear();
	}
}

void Renderer3D::update(float dt)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))camera.y += 5 * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))camera.y -= 5 * dt;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))camera.x -= 5 * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))camera.x += 5 * dt;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))yaw -= 2 * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))yaw += 2 * dt;

	Vector forward = lookDir * (8.0f * dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))camera = camera + forward;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))camera = camera - forward;


	Matrix4x4 matWorld =
		Matrix4x4::Identity() *
		Matrix4x4::RotationZ(fTheta * 0.5f) *
		Matrix4x4::RotationX(fTheta) *
		Matrix4x4::Translation(0, 0, 5.f);

	Vector up{ 0.f,1.f,0.f };
	Vector target{ 0.f, 0.f, 1.f };
	Matrix4x4 matCameraRot = Matrix4x4::RotationY(yaw);
	lookDir = matCameraRot * target;
	target = camera + lookDir;

	Matrix4x4 matCamera = Matrix4x4::PointAt(camera, target, up);
	Matrix4x4 matView = Matrix4x4::QuickInverse(matCamera);

	sf::Color COLOR = sf::Color::White;
	COLOR.a = 0;

	for (auto& tri : cube.tris)
	{
		Triangle triTransformed = matWorld * tri;

		Vector normal = unit(cross(triTransformed[1] - triTransformed[0], triTransformed[2] - triTransformed[0]));

		if (dot(normal, triTransformed[0] - camera) < 0.0f) {
			
			Triangle triViewed = matView * triTransformed;

			Triangle triProjected = matProj * triViewed;

			triProjected[0] = triProjected[0] / triProjected[0].w;
			triProjected[1] = triProjected[1] / triProjected[1].w;
			triProjected[2] = triProjected[2] / triProjected[2].w;

			this->scaleToView(triProjected, sf::Vector2f(pixel_x, pixel_y));

			float dp = dot(unit({ 0, 0, -1 }), normal);  //  lightDir * normal
			triProjected.color = sf::Color(COLOR.r * dp, COLOR.g * dp, COLOR.b * dp);
			
			if(this->renderingMode == RENDER_MODE::PIXEL)this->drawTriangleToPixels(triProjected);
			else if(this->renderingMode == RENDER_MODE::TRIANGLE)trianglestoRaster.push_back(triProjected);
		}
	}

	if (this->renderingMode == RENDER_MODE::TRIANGLE) {
		sort(trianglestoRaster.begin(), trianglestoRaster.end(), [](Triangle& a, Triangle& b) {
			return (a[0].z + a[1].z + a[2].z) / 3.0f > (b[0].z + b[1].z + b[2].z) / 3.0f;
		});
	}
}

void Renderer3D::drawPixels(sf::RenderWindow& window)
{
	window.draw(pixels, pixel_x * pixel_y, sf::Points);
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
	if (renderingMode == RENDER_MODE::PIXEL) {
		this->drawPixels(window);
	}

	else if (renderingMode == RENDER_MODE::TRIANGLE){
		this->drawTriangles(window);
	}
}
