#include "TTK/MeshBase.h"
#include "GLUT/glut.h"
#include <iostream>

void TTK::MeshBase::draw()
{
	vbo.draw();
}

void TTK::MeshBase::draw_1_0()
{
	if (vertices.size() == 0)
	{
		std::cout << "Mesh has no vertices!" << std::endl;
		return;
	}

	bool useColours = colours.size() > 0 ? true : false;
	bool useUVs = textureCoordinates.size() > 0 ? true : false;

	if (useUVs)
	{
		if (textureCoordinates.size() != vertices.size())
		{
			std::cout << "Number of texture coordinates does not match number of vertices!" << std::endl;
			return;
		}
	}

	if (useColours)
	{
		if (colours.size() != vertices.size())
		{
			std::cout << "Number of vertex colours does not match number of vertices!" << std::endl;
			return;
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

	if (primitiveType == TTK::PrimitiveType::Quads)
		glBegin(GL_QUADS);
	else
		glBegin(GL_TRIANGLES);

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		glTexCoord2f(textureCoordinates[i].x, textureCoordinates[i].y);

		if (useColours)
			glColor4fv(&colours[i][0]);
		else
			glColor4f(0.0, 0.0, 0.0, 1.0);

		glNormal3fv(&normals[i][0]);
		glVertex3fv(&vertices[i][0]);
	}

	glEnd();

	glDisable(GL_BLEND);
}

void TTK::MeshBase::setAllColours(glm::vec4 colour)
{
	for (unsigned int i = 0; i < colours.size(); i++)
	{
		colours[i] = colour;
	}
}

void TTK::MeshBase::createVBO()
{
	unsigned int numTris = (unsigned int)(vertices.size()) / 3; // todo: handle non-triangulated meshes

	// Setup VBO
	
	// Set up position (vertex) attribute
	if (vertices.size() > 0)
	{
		AttributeDescriptor positionAttrib;
		positionAttrib.attributeLocation = AttributeLocations::VERTEX;
		positionAttrib.attributeName = "vertex";
		positionAttrib.data = &vertices[0];
		positionAttrib.elementSize = sizeof(float);
		positionAttrib.elementType = GL_FLOAT;
		positionAttrib.numElements = numTris * 3 * 3; // (num triangles * three vertices per triangle * three floats per vertex)
		positionAttrib.numElementsPerAttrib = 3;
		vbo.addAttributeArray(positionAttrib);

	}

	// Set up UV attribute
	if (textureCoordinates.size() > 0)
	{
		AttributeDescriptor uvAttrib;
		uvAttrib.attributeLocation = AttributeLocations::TEX_COORD;
		uvAttrib.attributeName = "uv";
		uvAttrib.data = &textureCoordinates[0];
		uvAttrib.elementSize = sizeof(float);
		uvAttrib.elementType = GL_FLOAT;
		uvAttrib.numElements = numTris * 3 * 2;
		uvAttrib.numElementsPerAttrib = 2;
		vbo.addAttributeArray(uvAttrib);
	}

	// Set up normal attribute
	if (normals.size() > 0)
	{
		AttributeDescriptor normalAttrib;
		normalAttrib.attributeLocation = AttributeLocations::NORMAL;
		normalAttrib.attributeName = "normal";
		normalAttrib.data = &normals[0];
		normalAttrib.elementSize = sizeof(float);
		normalAttrib.elementType = GL_FLOAT;
		normalAttrib.numElements = numTris * 3 * 3;
		normalAttrib.numElementsPerAttrib = 3;
		vbo.addAttributeArray(normalAttrib);
	}

	// set up other attributes...

	vbo.createVBO(GL_STATIC_DRAW);
}
