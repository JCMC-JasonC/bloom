#pragma once

#include "glm/glm.hpp"
#include <glm\gtx\color_space.hpp>
#include "MeshBase.h"
#include <memory>

// Simple function to get a rgb color at the specified hue point
// t is meant to be between 0 and 1
glm::vec4 getColorFromHue(float t);

std::shared_ptr<TTK::MeshBase> createQuadMesh();

// Returns a random float between 0 and 1
float randomFloat01();

float randomFloatRange(float min = -1.0f, float max = 1.0f);

glm::vec3 randomDirection();

glm::vec3 randomColourRGB();