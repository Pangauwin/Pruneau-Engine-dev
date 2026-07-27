#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "asset/asset.h"

namespace Renderer {

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv_coord;

	Vertex(glm::vec3 _position = glm::vec3(), glm::vec3 _normal = glm::vec3(), glm::vec2 _uv_coord = glm::vec2()) : position(_position), normal(_normal), uv_coord(_uv_coord) {}
};

class Mesh {
public:
	Mesh(const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& _indices);
	~Mesh();

	/*static Mesh* CreateTriangle();
	static Mesh* CreatePlane();
	static Mesh* CreateCube();*/
	// TODO : recreate these functions

	unsigned int VAO, VBO, EBO;
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
};

}