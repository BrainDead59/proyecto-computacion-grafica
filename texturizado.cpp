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
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture dodecaedroTexture;

Texture dadoT;


Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
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



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CreateShaders();
	CrearDodecaedro();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	dadoTexture = Texture("Textures/dadoe.jpg");
	dadoTexture.LoadTextureA();

	dodecaedroTexture = Texture("Textures/dodecaedro.jpg");
	dodecaedroTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt.3ds");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");

	Dado_M = Model();
	Dado_M.LoadModel("Models/dado.obj");

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


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -1.5f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Kitt_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, -1.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Blackhawk_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.53f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Camino_M.RenderModel();

		//Dado de Opengl
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//dadoTexture.UseTexture();
	//	meshList[4]->RenderMesh();
		
		//Dado importado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 3.0f, -2.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Dado_M.RenderModel();
		


		//DODECAEDRO EN OPENGL
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dodecaedroTexture.UseTexture();
		meshList[5]->RenderMesh();


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
