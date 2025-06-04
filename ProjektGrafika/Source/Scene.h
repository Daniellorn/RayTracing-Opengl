#pragma once

#include <vector>

#include "glm/glm.hpp"


struct alignas(16) Material
{
	glm::vec3 albedo;
	float lol = 0.0f;
};

struct alignas(16) Sphere
{
	glm::vec3 position;
	float radius;

	Material material;
};


class Scene
{
public:

	Scene() = default;
	~Scene() = default;

	void AddObject(const Sphere& sphere);

	const std::vector<Sphere>& GetSpheres() const { return m_Objects; }

private:
	std::vector<Sphere> m_Objects;
	
};