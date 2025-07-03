#pragma once

#include <vector>

#include "glm/glm.hpp"

enum class Model
{
	DIFFUSE = 0,
	SPECULAR,
	EMISSIVE
};


struct alignas(16) Material
{
	glm::vec4 albedo; //0-15

	float roughness; //16-19

	float metallic = 0.0f; //20-23

	//padding 24-32
	float padding[2] = { 0.0f, 0.0f };

	 glm::vec4 EmissionColor{ 0.0f }; //32-47

	float EmissionPower = 0.0f; // 48-51
	//52-64
};

struct alignas(16) Sphere
{
	glm::vec4 position; //0-15
	float radius; //16-19

	int materialIndex; //20-23
	int type;
	//alignment 27-32
};


struct Lights
{
	glm::vec4 position;
	glm::vec4 color;
	float radius;
	float intensity;
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

	int m_Bounces = 5;
	
};