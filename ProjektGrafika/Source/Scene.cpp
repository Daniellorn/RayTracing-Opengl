#include "Scene.h"

void Scene::AddObject(const Sphere& sphere)
{
	m_Objects.push_back(sphere);
}
