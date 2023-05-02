/*
Semestre 2023-2
Práctica 6: Texturizado
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

using std::vector;
const float PI = 3.14159265f;

Camera camera;

Texture dirtTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture sailorTexture;
Texture sailorColetasTexture;

Model Camino_M;
Model LapidaPH1;
Model vela;

Skybox skybox;

Sphere coletas= Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);


	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.34f,  0.51f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.65f,	0.51f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.65f,	0.74f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.34f,	0.74f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	   0.67f,	0.51f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.99f,	0.51f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.99f,	0.74f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	   0.67f,	0.74f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.65f,	0.01f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.34f,	0.01f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,	    0.34f,	0.24f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.65f,  0.24f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.01f,  0.51f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.32f,	0.51f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.32f,	0.74f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.01f,	0.74f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.34f,	0.49f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.65f,	0.49f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.65f,	0.26f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.34f,  0.26f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.34f,  0.76f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.65f,	0.76f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.65f,	0.99f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.34f,	0.99f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

}

void CrearSailorMoonCabeza()
{
	unsigned int sailor_indices[] = {
		// front
		0, 1, 10,
		0, 10, 11,
		1, 2, 9,
		1, 9, 10,
		2, 3, 8,
		2, 8, 9,
		3, 4, 7,
		3, 7, 8,
		4, 5, 6,
		4, 6, 7,

		//derecho
		12, 13, 14,
		12, 14, 15,
		13, 16, 17,
		13, 17, 14,
		16, 18, 19,
		16, 19, 17,
		18, 20, 21,
		18, 21, 19,
		20, 22, 23,
		20, 23, 21,

		// izq
		24, 25, 26,
		24, 26, 27,
		25, 28, 29,
		25, 29, 26,
		28, 30, 31,
		28, 31, 29,
		30, 32, 33,
		30, 33, 31,
		32, 34, 35,
		32, 35, 33,

		//abajo
		36, 37, 38,
		36, 38, 39,

		40, 41, 46,
		40, 45, 46,
		41, 42, 47,
		41, 46, 47,
		42, 43, 48,
		48, 42, 47,
		43, 44, 49,
		43, 49, 48
	};
	
	GLfloat sailor_vertices[] = {

		//cabeza

		// rostro
		//x		y		z			S				T					NX		NY		NZ
		-0.35f, 0.25f,  0.3f,		(0.009f * 27),  (0.009f * 90),		0.0f,	0.0f,	0.0f,	//0
		-0.35f, -0.1f,  0.3f,		(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//1
		-0.33f,  -0.15f,  0.28f,	(0.009f * 28),	(0.009f * 60),		0.0f,	0.0f,	0.0f,	//2
		-0.30f,  -0.2f,  0.25f,		(0.009f * 30),	(0.009f * 56),		0.0f,	0.0f,	0.0f,	//3
		-0.25f,  -0.23f,  0.20f,	(0.009f * 34),	(0.009f * 54),		0.0f,	0.0f,	0.0f,	//4
		-0.20f,  -0.25f,  0.15f,	(0.009f * 38),	(0.009f * 53),		0.0f,	0.0f,	0.0f,	//5
		0.20f,  -0.25f,  0.15f,		(0.009f * 69),	(0.009f * 53),		0.0f,	0.0f,	0.0f,	//6
		0.25f,  -0.23f,  0.20f,		(0.009f * 73),	(0.009f * 54),		0.0f,	0.0f,	0.0f,	//7
		0.30f,  -0.2f,  0.25f,		(0.009f * 77),	(0.009f * 56),		0.0f,	0.0f,	0.0f,	//8
		0.33f,  -0.15f,  0.28f,		(0.009f * 79),	(0.009f * 60),		0.0f,	0.0f,	0.0f,	//9
		0.35f, -0.1f,  0.3f,		(0.009f * 80),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//10
		0.35f, 0.25f,  0.3f,		(0.009f * 80),  (0.009f * 90),		0.0f,	0.0f,	0.0f,	//11
	
		// derecho
		//x		y		z			S				T					NX		NY		NZ
		0.35f, 0.25f,  0.3f,		(0.009f * 83),  (0.009f * 90),		0.0f,	0.0f,	0.0f,	//12
		0.35f, -0.1f,  0.3f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//13
		0.35f, -0.1f,  -0.3f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//14
		0.35f, 0.25f,  -0.3f,		(0.009f * 83),  (0.009f * 90),		0.0f,	0.0f,	0.0f,	//15
		0.33f, -0.15f,  0.28f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//16
		0.33f, -0.15f,  -0.28f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//17
		0.3f, -0.2f,  0.25f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//18
		0.3f, -0.2f,  -0.25f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//19
		0.25f, -0.23f,  0.2f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//20
		0.25f, -0.23f,  -0.2f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//21
		0.2f, -0.25f,  0.15f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//22
		0.2f, -0.25f,  -0.15f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//23

		// izquierdo
		//x		y		z			S				T					NX		NY		NZ
		-0.35f, 0.25f,  0.3f,		(0.009f * 83),  (0.009f * 90),		0.0f,	0.0f,	0.0f,	//24
		-0.35f, -0.1f,  0.3f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//25
		-0.35f, -0.1f,  -0.3f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//26
		-0.35f, 0.25f,  -0.3f,		(0.009f * 83),  (0.009f * 90),		0.0f,	0.0f,	0.0f,	//27
		-0.33f, -0.15f,  0.28f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//28
		-0.33f, -0.15f,  -0.28f,	(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//29
		-0.3f, -0.2f,  0.25f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//30
		-0.3f, -0.2f,  -0.25f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//31
		-0.25f, -0.23f,  0.2f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//32
		-0.25f, -0.23f,  -0.2f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//33
		-0.2f, -0.25f,  0.15f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//34
		-0.2f, -0.25f,  -0.15f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//35

		// abajo
		//x		y		z			S				T					NX		NY		NZ
		0.2f,  -0.25f,  0.15f,		(0.009f * 72),	(0.009f * 53),		0.0f,	0.0f,	0.0f,	//36 
		0.2f, -0.25f,  -0.15f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//37
		-0.2f,  -0.25f,  -0.15f,	(0.009f * 72),	(0.009f * 53),		0.0f,	0.0f,	0.0f,	//38
		-0.2f,  -0.25f,  0.15f,		(0.009f * 72),	(0.009f * 53),		0.0f,	0.0f,	0.0f,	//39

		0.35f, -0.1f,  -0.3f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//40
		0.33f, -0.15f,  -0.28f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//41
		0.3f, -0.2f,  -0.25f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//42
		0.25f, -0.23f,  -0.2f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//43
		0.2f, -0.25f,  -0.15f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//44
		-0.35f, -0.1f,  -0.3f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//45
		-0.33f, -0.15f,  -0.28f,	(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//46
		-0.3f, -0.2f,  -0.25f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//47
		-0.25f, -0.23f,  -0.2f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//48
		-0.2f, -0.25f,  -0.15f,		(0.009f * 83),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//49

	};

	Mesh* sailor = new Mesh();
	sailor->CreateMesh(sailor_vertices, sailor_indices, 400, 120);
	meshList.push_back(sailor);

}

void CrearSailorMoonPelo()
{
	unsigned int indices[] = {
		// fleco 18
		0, 1, 2,
		1, 2, 3,
		2, 3, 4,
		3, 4, 5,
		4, 5, 6,
		5,6,7,
		5,7,8,
		3,5,8,
		8, 9,10,
		3,8,10,
		1,3,15,
		3, 10,11,
		11,12,13,
		3,11,14,
		11,13,14,
		3, 14, 15,
		14, 15, 16,
		14, 16, 17,
		// 36
		0+18,	1 + 18, 2 + 18,
		1 + 18, 2 + 18, 3 + 18,
		2 + 18, 3 + 18, 4 + 18,
		3 + 18, 4 + 18, 5 + 18,
		4 + 18, 5 + 18, 6 + 18,
		5 + 18,	6 + 18,	7 + 18,
		5 + 18,	7 + 18,	8 + 18,
		3 + 18,	5 + 18,	8 + 18,
		8 + 18, 9 + 18,	10 + 18,
		3 + 18,	8 + 18,	10 + 18,
		1 + 18,	3 + 18,	15 + 18,
		3 + 18, 10 + 18,11 + 18,
		11 + 18,12 + 18,13 + 18,
		3 + 18,	11 + 18,14 + 18,
		11 + 18,13 + 18,14 + 18,
		3 + 18, 14 + 18, 15 + 18,
		14 + 18, 15 + 18, 16 + 18,
		14 + 18, 16 + 18, 17 + 18,
		//41
		14, 17, 36,
		16, 17, 36, 
		15, 16, 37,
		16, 36, 37,
		36, 37, 38,
		//46
		32, 35, 39,
		34, 35, 39,
		33, 34, 40,
		34, 39, 40,
		39, 40, 41,
		//54
		15, 42,33,
		42, 33, 43,
		15, 42, 37,
		42, 37, 44,
		33, 43, 40,
		43, 40, 45,
		42, 43, 44,
		45, 44, 43,
		//58
		38, 37, 41,
		37, 41, 40,
		37, 40, 44,
		40, 44, 45
		
	};

	GLfloat vertices[] = {

		// fleco
		//x		y		z			S				T					NX		NY		NZ
		0.0f, 0.25f,  0.3f,		(0.009f * 27),  (0.009f * 90),		0.0f,	0.0f,	0.0f,	//0
		0.0f, 0.29f,  0.35f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//1
		0.08f, 0.2f,  0.3f,		(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//2
		0.09f, 0.2f,  0.35f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//3
		0.09f, 0.17f,  0.3f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//4
		0.1f, 0.17f,  0.35f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//5
		0.07f, 0.1f,  0.3f,		(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//6
		0.13f, 0.1f,  0.3f,		(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//7
		0.18f, 0.11f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//8
		0.19f, 0.09f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//9
		0.23f, 0.09f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//10
		0.26f, 0.1f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//11
		0.27f, 0.08f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//12
		0.3f, 0.08f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//13
		0.36f, 0.09f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//14
		0.36f, 0.29f,  0.34f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//15
		0.38f, 0.23f,  0.35f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//16
		0.38f, 0.15f,  0.35f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//17



		0.0f, 0.25f,  0.3f,		(0.009f * 27),  (0.009f * 90),		0.0f,	0.0f,	0.0f,	//0 18
		0.0f, 0.29f,  0.35f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//1 19
		-0.08f, 0.2f,  0.3f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//2 20
		-0.09f, 0.2f,  0.35f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//3 21
		-0.09f, 0.17f,  0.3f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//4 22
		-0.1f, 0.17f,  0.35f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//5 23
		-0.07f, 0.1f,  0.3f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//6 24
		-0.13f, 0.1f,  0.3f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//7 25
		-0.18f, 0.11f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//8 26
		-0.19f, 0.09f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//9 27
		-0.23f, 0.09f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//10 28
		-0.26f, 0.1f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//11 29
		-0.27f, 0.08f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//12 30
		-0.3f, 0.08f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//13 31
		-0.36f, 0.09f,  0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//14 32
		-0.36f, 0.29f,  0.34f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//15 33
		-0.38f, 0.23f,  0.35f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//16 34
		-0.38f, 0.15f,  0.35f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//17 35

		//// izquierdo
		////x		y		z			S				T					NX		NY		NZ
		0.36f, 0.0f,  0.0f,		(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//36
		0.36f, 0.29f,  -0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//37
		0.36f, -0.17f,  -0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//38

		//// derecho
		////x		y		z			S				T					NX		NY		NZ
		-0.36f, 0.0f,  0.0f,		(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//39
		-0.36f, 0.29f,  -0.31f,		(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//40
		-0.36f, -0.17f,  -0.31f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//41

		//// arriba
		////x		y		z			S				T					NX		NY		NZ
		0.28f, 0.35f,  0.3f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//42
		-0.28f, 0.35f,  0.3f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//43
		0.33f, 0.35f,  -0.20f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//44
		-0.33f, 0.35f,  -0.20f,	(0.009f * 27),	(0.009f * 64),		0.0f,	0.0f,	0.0f,	//45
	};

	Mesh* sailor = new Mesh();
	sailor->CreateMesh(vertices, indices, 368, 174);
	meshList.push_back(sailor);

}

void CrearDodecaedro()
{
	unsigned int dodecaedro_indices[] = {
		// sup
		0,1,2, 
		0,2,3,
		0,3,4,
		// sup lado 1
		5,6,7,
		6,7,9,
		7,8,9,
		//sup lado 2
		10,11,12,
		11,12,13,
		11,13,14,
		//sup lado 3
		15,16,17,
		16,17,18,
		16,18,19,
		//sup lado 4
		21,22,23,
		21,23,24,
		20,21,24,
		//sup lado 5
		25,26,27,
		26,27,29,
		26,28,29,
		//INF LADO 1
		30,31,34,
		31,33,34,
		31,32,33,
		//INF LADO 2
		35,36,37,
		35,37,38,
		35,38,39,
		//INF LADO 3
		40,41,44,
		41,43,44,
		41,42,43,
		//INF LADO 4
		45,46,49,
		46,48,49,
		46,47,48,
		//INF LADO 5
		50,51,52,
		50,52,53,
		50,53,54,
		// INF
		55,56,59,
		56,57,59,
		57,58,59
	};
	
	GLfloat dodecaedro_vertices[] = {
		//cara superior (6)
		//x			y		z	s		t		x		y		z
		0.0f,	0.62f,	1.62f,	0.295f,	0.436f,	0.0f,	0.0f,	0.0f, //A 0
		0.0f,	-0.62f,	1.62f,	0.186f,	0.354f,	0.0f,	0.0f,	0.0f, //C 1
		1.0f,	-1.0f,	1.0f,	0.226f,	0.221f,	0.0f,	0.0f,	0.0f, //O 2
		1.62f,	0.0f,	0.62f,	0.365f,	0.221f,	0.0f,	0.0f,	0.0f, //E 3
		1.0f,	1.0f,	1.0f,	0.406f,	0.354f,	0.0f,	0.0f,	0.0f, //M 4
		//cara superior lado 1 (5)
		//x			y		z	s		t		x		y		z
		0.0f,	-0.62f,	1.62f,	0.186f,	0.354f,	1.0f,	0.0f,	0.0f, //C 5
		1.0f,	-1.0f,	1.0f,	0.226f,	0.221f,	1.0f,	0.0f,	0.0f, //O 6
		-1.0f,	-1.0f,	1.0f,	0.055f,	0.344f,	0.0f,	0.0f,	0.0f, //S 7
		-0.62f,	-1.62f,	0.0f,	0.01f,	0.221f,	0.0f,	0.0f,	0.0f, //L 8
		0.62f,	-1.62f,	0.0f,	0.113f,	0.141f,	0.0f,	0.0f,	0.0f, //J 9
		//cara superior lado 2 (4)
		//x			y		z	s		t		x		y		z
		1.0f,	-1.0f,	1.0f,	0.236f,	0.211f,	0.0f,	0.0f,	0.0f, //O 10
		1.62f,	0.0f,	0.62f,	0.365f,	0.211f,	0.0f,	0.0f,	0.0f, //E 11
		0.62f,	-1.62f,	0.0f,	0.186f,	0.088f,	0.0f,	0.0f,	0.0f, //J 12
		1.0f,	-1.0f,	-1.0f,	0.295f,	0.01f,	0.0f,	0.0f,	0.0f, //P 13
		1.62f,	0.0f,	-0.62f,	0.406f,	0.088f,	0.0f,	0.0f,	0.0f, //F 14
		//cara superior lado 3 (3)
		//x			y		z	s		t		x		y		z
		1.62f,	0.0f,	0.62f,	0.365f,	0.221f,	0.0f,	0.0f,	0.0f, //E 15
		1.0f,	1.0f,	1.0f,	0.406f,	0.354f,	0.0f,	0.0f,	0.0f, //M 16
		1.62f,	0.0f,	-0.62f,	0.477f,	0.141f,	0.0f,	0.0f,	0.0f, //F 17
		1.0f,	1.0f,	-1.0f,	0.588f,	0.221f,	0.0f,	0.0f,	0.0f, //N 18
		0.62f,	1.62f,	0.0f,	0.545f,	0.354f,	0.0f,	0.0f,	0.0f, //I 19
		//cara superior lado 4 (2)
		//x			y		z	s		t		x		y		z
		0.0f,	0.62f,	1.62f,	0.295f,	0.436f,	0.0f,	0.0f,	0.0f, //A 20
		1.0f,	1.0f,	1.0f,	0.406f,	0.354f,	0.0f,	0.0f,	0.0f, //M 21
		0.62f,	1.62f,	0.0f,	0.52f,	0.436f,	0.0f,	0.0f,	0.0f, //I 22
		-0.62f,	1.62f,	0.0f,	0.479f,	0.56f,	0.0f,	0.0f,	0.0f, //K 23
		-1.0f,	1.0f,	1.0f,	0.348f,	0.56f,	0.0f,	0.0f,	0.0f, //Q 24
		//cara superior lado 5 (1)
		//x			y		z	s		t		x		y		z
		0.0f,	0.62f,	1.62f,	0.285f,	0.436f,	0.0f,	0.0f,	0.0f, //A 25
		0.0f,	-0.62f,	1.62f,	0.186f,	0.364f,	1.0f,	0.0f,	0.0f, //C 26
		-1.0f,	1.0f,	1.0f,	0.252f,	0.55f,	0.0f,	0.0f,	0.0f, //Q 27
		-1.0f,	-1.0f,	1.0f,	0.09f,	0.436f,	0.0f,	0.0f,	0.0f, //S 28
		-1.62f,	0.0f,	0.62f,	0.133f,	0.55f,	0.0f,	0.0f,	0.0f, //G 29
		//cara inferior 1 (7)
		//x			y		z	s		t		x		y		z
		-1.62f,	0.0f,	0.62f,	0.60f,	0.91f,	0.0f,	0.0f,	0.0f, //G 30
		-1.0f,	-1.0f,	1.0f,	0.701f,	0.99f,	0.0f,	0.0f,	0.0f, //S 31
		-0.62f,	-1.62f,	0.0f,	0.799f,	0.91f,	0.0f,	0.0f,	0.0f, //L 32
		-1.0f,	-1.0f,	-1.0f,	0.761f,	0.797f,	0.0f,	0.0f,	0.0f, //T 33
		-1.62f,	0.0f,	-0.62f,	0.641f,	0.797f,	0.0f,	0.0f,	0.0f, //H 34
		//cara inferior 2 (8)
		//x			y		z	s		t		x		y		z
		1.0f,	-1.0f,	-1.0f,	0.953f,	0.652f,	0.0f,	0.0f,	0.0f, //P 35
		0.62f,	-1.62f,	0.0f,	0.99f,	0.787f,	0.0f,	0.0f,	0.0f, //J 36
		-0.62f,	-1.62f,	0.0f,	0.881f,	0.859f,	0.0f,	0.0f,	0.0f, //L 37
		-1.0f,	-1.0f,	-1.0f,	0.771f,	0.787f,	0.0f,	0.0f,	0.0f, //T 38
		0.0f,	-0.62f,	-1.62f,	0.813f,	0.652f,	0.0f,	0.0f,	0.0f, //D 39
		//cara inferior 3 (9)
		//x			y		z	s		t		x		y		z
		0.0f,	-0.62f,	-1.62f,	0.813f,	0.652f,	0.0f,	0.0f,	0.0f, //D 40
		1.0f,	-1.0f,	-1.0f,	0.924f,	0.57f,	0.0f,	0.0f,	0.0f, //P 41
		1.62f,	0.0f,	-0.62f,	0.881f,	0.436f,	0.0f,	0.0f,	0.0f, //F 42
		1.0f,	1.0f,	-1.0f,	0.748f,	0.436f,	0.0f,	0.0f,	0.0f, //N 43
		0.0f,	0.62f,	-1.62f,	0.701f,	0.57f,	0.0f,	0.0f,	0.0f, //B 44
		//cara inferior 4 (10)
		//x			y		z	s		t		x		y		z
		0.0f,	0.62f,	-1.62f,	0.701f,	0.57f,	0.0f,	0.0f,	0.0f, //B 45
		1.0f,	1.0f,	-1.0f,	0.658f,	0.436f,	0.0f,	0.0f,	0.0f, //N 46
		0.62f,	1.62f,	0.0f,	0.52f,	0.436f,	0.0f,	0.0f,	0.0f, //I 47
		-0.62f,	1.62f,	0.0f,	0.479f,	0.57f,	0.0f,	0.0f,	0.0f, //K 48
		-1.0f,	1.0f,	-1.0f,	0.588f,	0.642f,	0.0f,	0.0f,	0.0f, //R 49
		//cara inferior 4 (11)
		//x			y		z	s		t		x		y		z
		-0.62f,	1.62f,	0.0f,	0.461f,	0.662f,	0.0f,	0.0f,	0.0f, //K 50
		-1.0f,	1.0f,	-1.0f,	0.578f,	0.662f,	0.0f,	0.0f,	0.0f, //R 51
		-1.62f,	0.0f,	-0.62f,	0.631f,	0.787f,	0.0f,	0.0f,	0.0f, //H 52
		-1.62f,	0.0f,	0.62f,	0.52f,	0.859f,	0.0f,	0.0f,	0.0f, //G 53
		-1.0f,	1.0f,	1.0f,	0.41f,	0.787f,	0.0f,	0.0f,	0.0f, //Q 54
		//CARA INF (12)
		//x			y		z	s		t		x		y		z
		-1.0f,	1.0f,	-1.0f,	0.598f,	0.652f,	0.0f,	0.0f,	0.0f, //R 55
		-1.62f,	0.0f,	-0.62f,	0.641f,	0.777f,	0.0f,	0.0f,	0.0f, //H 56
		-1.0f,	-1.0f,	-1.0f,	0.761f,	0.777f,	0.0f,	0.0f,	0.0f, //T 57
		0.0f,	-0.62f,	-1.62f,	0.803f,	0.652f,	0.0f,	0.0f,	0.0f, //D 58
		0.0f,	0.62f,	-1.62f,	0.701f,	0.58f,	0.0f,	0.0f,	0.0f, //B 59

	};

	Mesh* dodecaedro = new Mesh();
	dodecaedro->CreateMesh(dodecaedro_vertices, dodecaedro_indices, 480, 108);
	meshList.push_back(dodecaedro);

}

void CrearTorzo()
{
	unsigned int indices[] = {
		// sup
		0,1,2,
		
	
	};

	GLfloat vertices[] = {
		//cara superior (6)
		//x			y		z	s		t		x		y		z
		1.0f,	0.0f,	1.0f,	0.295f,	0.436f,	0.0f,	0.0f,	0.0f, // 0
		-1.0f,	0.0f,	1.0f,	0.295f,	0.436f,	0.0f,	0.0f,	0.0f, // 1
		0.0f,	2.0f,	1.0f,	0.295f,	0.436f,	0.0f,	0.0f,	0.0f, // 2
		

	};

	Mesh* dodecaedro = new Mesh();
	dodecaedro->CreateMesh(vertices, indices, 32, 3);
	meshList.push_back(dodecaedro);

}

void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMesh(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CreateShaders();
	CrearDodecaedro();
	CrearSailorMoonCabeza();
	CrearSailorMoonPelo();
	CrearTorzo();
	//CrearCilindro(50, 1.0f);

	coletas.init();
	coletas.load();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dadoe.jpg");
	dadoTexture.LoadTextureA();
	sailorTexture = Texture("Textures/sailormoon_textura.png");
	sailorTexture.LoadTextureA();
	sailorColetasTexture = Texture("Textures/coletas.png");
	sailorColetasTexture.LoadTextureA();

	LapidaPH1 = Model();
	LapidaPH1.LoadModel("Models/lapidaPH1.obj");

	vela = Model();
	vela.LoadModel("Models/vela.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//pisoTexture.UseTexture();
		//meshList[2]->RenderMesh();

		//Piso 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, -4.5f, -2.0f));
		model = glm::scale(model, glm::vec3(50.0f, 5.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		/*dadoTexture.UseTexture();*/
		dirtTexture.UseTexture();
		meshList[4]->RenderMesh();

		//sailor
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 4.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sailorTexture.UseTexture();
		meshList[6]->RenderMesh();

		//sailor pelo
		color = glm::vec3(1.00000f, 0.9f, 0.19608f);
		model = glm::mat4(1.0);
		model = modelaux;
		//model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//sailorTexture.UseTexture();
		meshList[7]->RenderMesh();

		//coletas
		model = glm::mat4(1.0);
		color = glm::vec3(1.00000f, 0.9f, 0.19608f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.0f, 1.3f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sailorColetasTexture.UseTexture();
		coletas.render(); //Renderiza esfera

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0f, 1.3f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sailorColetasTexture.UseTexture();
		coletas.render(); //Renderiza esfera
		
		//sailor coletas
		color = glm::vec3(1.00000f, 0.9f, 0.19608f);
		model = glm::mat4(1.0);
		model = modelaux;
		//model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		model = glm::scale(model, glm::vec3(0.2f, 3.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//meshList[8]->RenderMeshGeometry();

		color = glm::vec3(0.0f, 0.0f, 0.0f);
		model = glm::mat4(1.0);
		model = modelaux;
		//model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//sailorTexture.UseTexture();
		meshList[8]->RenderMesh();
		
		//Lapida
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(4.0f , 0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 10.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LapidaPH1.RenderModel();

		//vela
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 10.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		vela.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
