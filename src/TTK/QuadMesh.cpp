#include "TTK/QuadMesh.h"
#include <vector>

QuadMesh::QuadMesh()
{
	vertices.reserve(4); // performance tip, explain how vectors work
	textureCoordinates.reserve(4);

// 	vertices.push_back(glm::vec3(0, 0, 0)); // bottom left
// 	vertices.push_back(glm::vec3(0, 1, 0)); // top left
// 	vertices.push_back(glm::vec3(1, 1, 0)); // top right
// 	vertices.push_back(glm::vec3(1, 0, 0)); // bottom right

	vertices.push_back(glm::vec3(-0.5f, -0.5f, 0)); // bottom left
	vertices.push_back(glm::vec3(-0.5f, 0.5f, 0)); // top left
	vertices.push_back(glm::vec3(0.5f, 0.5f, 0)); // top right
	vertices.push_back(glm::vec3(0.5f, -0.5f, 0)); // bottom right

	textureCoordinates.push_back(glm::vec3(0, 0, 0)); // bottom left
	textureCoordinates.push_back(glm::vec3(0, 1, 0)); // top left
	textureCoordinates.push_back(glm::vec3(1, 1, 0)); // top right
	textureCoordinates.push_back(glm::vec3(1, 0, 0)); // bottom right

	colours = std::vector<glm::vec4>(4, glm::vec4(0, 0, 0, 1));

	primitiveType = TTK::PrimitiveType::Quads;
}

QuadMesh::~QuadMesh()
{

}

void QuadMesh::setUVbottomLeft(float u, float v)
{
	textureCoordinates[0].x = u;
	textureCoordinates[0].y = v;
}

void QuadMesh::setUVtopLeft(float u, float v)
{
	textureCoordinates[1].x = u;
	textureCoordinates[1].y = v;
}

void QuadMesh::setUVtopRight(float u, float v)
{
	textureCoordinates[2].x = u;
	textureCoordinates[2].y = v;
}


void QuadMesh::setUVbottomRight(float u, float v)
{
	textureCoordinates[3].x = u;
	textureCoordinates[3].y = v;
}

