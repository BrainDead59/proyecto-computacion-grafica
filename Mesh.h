#pragma once

#include <glew.h>
#include <vector>

class Mesh
{
public:
	Mesh();

	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void CreateMesh(std::vector<GLfloat>& vertices, std::vector<unsigned int>& indices, unsigned int numOfVertices, unsigned int numberOfIndices);
	void RenderMesh();
	void RenderMeshGeometry();
	void ClearMesh();

	~Mesh();

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

