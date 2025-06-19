#include "Scene.h"

void Scene::AddObject(const Sphere& sphere)
{
	m_Objects.push_back(sphere);
}

void Scene::AddMaterial(const Material& material)
{
	m_Materials.push_back(material);
}
