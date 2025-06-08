#pragma once

#include <vector>

#include "glm/glm.hpp"


struct alignas(16) Material
{
	glm::vec3 albedo;
	float roughness;
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

	std::vector<Sphere>& GetSpheres() { return m_Objects; }

private:
	std::vector<Sphere> m_Objects;
	
};