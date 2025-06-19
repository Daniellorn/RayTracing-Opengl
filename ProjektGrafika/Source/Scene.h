#pragma once

#include <vector>

#include "glm/glm.hpp"


struct alignas(16) Material
{
	glm::vec3 albedo;
	float roughness;

	float metallic = 0.0f;
	float pad1 = 0.0f;


	glm::vec3 EmissionColor{ 0.0f };
	float EmissionPower = 0.0f;
	//float pad2 = 0.0f;

	//float padd[3];
};

struct alignas(16) Sphere
{
	glm::vec3 position;
	float radius;

	int materialIndex;
	int modelType;
};


class Scene
{
public:

	Scene() = default;
	~Scene() = default;

	void AddObject(const Sphere& sphere);
	void AddMaterial(const Material& material);

	std::vector<Sphere>& GetSpheres() { return m_Objects; }
	std::vector<Material>& GetMaterials() { return m_Materials; }

private:
	std::vector<Sphere> m_Objects;
	std::vector<Material> m_Materials;
	
};