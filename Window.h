#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getIniciaAnimacion() { return banderaAnimacion; }
	GLfloat getCamara() { return banderaCamara; }
	GLfloat getapagaLuces(){ return apagaLuces; }
	GLfloat getsailorx() { return sailorx; }
	GLfloat getsailorz() { return sailorz; }
	GLint getpierna() { return pierna; }
	GLint getbrazo() { return brazo; }
	GLint getgiro() { return giro; }

	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat sentidox;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat sailorx;
	GLfloat sailorz;
	GLint pierna;
	GLint brazo;
	GLint giro;
	bool banderaAnimacion;
	bool banderaCamara;
	GLfloat apagaLuces;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

