#include "TTK\PointHandle.h"
#include "TTK\GraphicsUtils.h"

PointHandle::PointHandle(float _pointSize, glm::vec3 _position, std::string _label)
{
	pointSize = _pointSize;
	position = _position;
	label = _label;
}

bool PointHandle::isInside(glm::vec3 p)
{
	return glm::length((p - position)) < pointSize;
}

void PointHandle::draw()
{
	TTK::Graphics::DrawPoint(position, pointSize);
	TTK::Graphics::DrawText2D(label, (int)position.x, (int)(position.y + pointSize * 0.5f));
}
