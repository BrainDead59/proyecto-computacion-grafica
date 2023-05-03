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
Texture plainTexture;
Texture pisoTexture;
Texture sailorTexture;
Texture sailorColetasTexture;

Model LapidaPH1;
Model vela;
Model lampara;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Sphere coletas = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

/*Cálculo del promedio de las normales para sombreado de Phong
en algunas ocasiones nos ayuda para no tener que declarar las normales manualmente dentro del VAO
*/
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
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

void CreaPiso()
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

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
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
		0 + 18,	1 + 18, 2 + 18,
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

	float apagaluz;

	CreaPiso();
	CreateShaders();
	CrearSailorMoonCabeza();
	CrearSailorMoonPelo();
	CrearTorzo();
	CrearCilindro(50, 1.0f);

	coletas.init();
	coletas.load();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	sailorTexture = Texture("Textures/sailormoon_textura.png");
	sailorTexture.LoadTextureA();
	sailorColetasTexture = Texture("Textures/coletas.png");
	sailorColetasTexture.LoadTextureA();


	vela = Model();
	vela.LoadModel("Models/vela.obj");
	lampara = Model();
	lampara.LoadModel("Models/lampara.obj");
	LapidaPH1 = Model();
	LapidaPH1.LoadModel("Models/miLapida.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//posición inicial del helicóptero
	glm::vec3 posblackhawk = glm::vec3(-20.0f, 6.0f, -1.0);

	//luz direccional, sólo 1 y siempre debe de existir
	//luz direccional es el sol\
	// 1 ambiental al maximo, 0 todo negro
	//difusa 1 por caras
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, 0.0f, -1.0f);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Veladora
	pointLights[0] = PointLight(1.0f, 0.5f, 0.0f,
		1.0f, 10.0f,
		0.0f, 1.0f, 25.0f,
		0.3f, 0.2f, 0.1f);//ec de 2do grado
	pointLightCount++;

	//Lampara
	pointLights[1] = PointLight(1.0f, 0.9f, 0.0f,
		1.0f, 10.0f,
		0.0f, 1.0f, 45.0f,
		0.3f, 0.1f, 0.1f);//ec de 2do grado
	pointLightCount++;

	unsigned int spotLightCount = 0;

	/*luz de tipo Spotlight
	spotLights[0] = SpotLight(0.4f, 0.4f, 0.0f,
		0.8f, 0.8f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;*/

	//Luz de la lampara - camara
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);//alcance
	spotLightCount++;
	
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformColor = 0;
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

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();
		uniformColor = shaderList[0].getColorLocation();

		//color blanco inicializado para todos los objetos a menos que se asigne nuevo color
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//luz ligada a la cámara de tipo flash 
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		apagaluz=mainWindow.getapagalinterna();

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);

		//prende y apaga linterna
		if (apagaluz < 1.0) {
			 shaderList[0].SetSpotLights(spotLights, spotLightCount-1);
		}
		else {
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}

		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
		model = glm::scale(model, glm::vec3(400.0f, 20.0f, 400.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();

		//agregar material al plano de piso
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		//sailor
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 4.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sailorTexture.UseTexture();
		meshList[1]->RenderMesh();

		//sailor pelo
		color = glm::vec3(1.00000f, 0.9f, 0.19608f);
		model = glm::mat4(1.0);
		model = modelaux;
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMesh();

		//coletas sailor
		model = glm::mat4(1.0);
		color = glm::vec3(1.00000f, 0.9f, 0.19608f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.0f, 1.3f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		sailorColetasTexture.UseTexture();
		coletas.render(); //Renderiza esfera

		//coletas sailor
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0f, 1.3f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		sailorColetasTexture.UseTexture();
		coletas.render(); //Renderiza esfera

		//cuerpo sailor
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		model = glm::mat4(1.0);
		model = modelaux;
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMesh();
		
		//Regresar el color normal
		color = glm::vec3(1.f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Vela
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		vela.RenderModel();

		//Lampara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 45.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lampara.RenderModel();

		//Lapida
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 10.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LapidaPH1.RenderModel();

		//Cilindro
		model = glm::mat4(1.0f);
		color = glm::vec3(0.5f, 0.5f, 0.4f);
		model = glm::translate(model, glm::vec3(1.25f, 10.0f, -3.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMeshGeometry();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
