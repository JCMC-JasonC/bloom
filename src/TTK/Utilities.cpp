#include "TTK/Utilities.h"

glm::vec4 getColorFromHue(float t)
{
	return glm::vec4(glm::rgbColor(glm::vec3(glm::mix(0.0f, 360.0f, t), 1.0f, 0.5f)), 1.0f);
}

std::shared_ptr<TTK::MeshBase> createQuadMesh()
{
	std::shared_ptr<TTK::MeshBase> quadMesh = std::make_shared<TTK::MeshBase>();

	// Triangle 1
	quadMesh->vertices.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(1.0f, 1.0f));

	quadMesh->vertices.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(0.0f, 1.0f));

	quadMesh->vertices.push_back(glm::vec3(-1.0, -1.0, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(0.0f, 0.0f));

	// Triangle 2
	quadMesh->vertices.push_back(glm::vec3(1.0, 1.0, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(1.0f, 1.0f));

	quadMesh->vertices.push_back(glm::vec3(-1.0, -1.0, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(0.0f, 0.0f));

	quadMesh->vertices.push_back(glm::vec3(1.0, -1.0, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(1.0f, 0.0f));

	quadMesh->createVBO();

	return quadMesh;
}

float randomFloat01()
{
	return (float)rand() / (float)RAND_MAX;
}

float randomFloatRange(float min /*= -1.0f*/, float max /*= 1.0f*/)
{
	return min + ((max - min) * rand()) / (RAND_MAX + 1.0f);
}

glm::vec3 randomDirection()
{
	return glm::vec3(randomFloatRange(), randomFloatRange(), randomFloatRange());
}

glm::vec3 randomColourRGB()
{
	return glm::vec3(randomFloatRange(0.0f), randomFloatRange(0.0f), randomFloatRange(0.0f));
}

