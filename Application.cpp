//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

#include "Application.h"

#include "SceneText.h"
#include "SceneMG1.h"
#include "SceneMG2.h"
#include "SceneMG3.h"

#include "SceneManager.h"

GLFWwindow* m_window;
const unsigned char FPS = 60; // FPS of this game
const unsigned int frameTime = 1000 / FPS; // time for each frame

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void resize_callback(GLFWwindow* window, int w, int h)
{
	//update opengl new window size
	glViewport(0, 0, w, h);
}

bool Application::IsKeyPressed(unsigned short key)
{
	
    return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

Application::Application()
{
}

Application::~Application()
{
}

void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 


	//Create a window and create its OpenGL context
	m_window = glfwCreateWindow(800, 600, "Test Window", NULL, NULL);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK) 
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}

	glfwSetWindowSizeCallback(m_window, resize_callback);

}

SceneManager* sManage = nullptr;

void Application::Run()
{
	//Main Loop

	SceneManager* sManage = SceneManager::getInstance();

	sManage->AddScene(new SceneText);
	sManage->AddScene(new SceneMG1);
	sManage->AddScene(new SceneMG2);
	sManage->AddScene(new SceneMG3);

	(sManage->getScene())->Init(); //start first scene

	m_timer.startTimer();// Start timer to calculate how long it takes to render this frame
	while (!glfwWindowShouldClose(m_window) && !IsKeyPressed(VK_ESCAPE))
	{
		//UPDATE SCENES CONDITIONS AFTER MGS ARE DONE

		//ONCE START MG, CANNOT GO BACK TO MAIN SCENE
		//ONLY AFTER COMPLETION (bool gameEnd == true)
		//ONLY NEED TWO KEYS, TO GO BACK (-1) AND TO GO FORWARD (+1)

		if (IsKeyPressed(VK_NUMPAD2)) //go forward in scene array
		{
			if (sManage->getcurrSceneID() < (sManage->getvSize() - 1))
			{
				(sManage->getScene())->Exit(); //current scene exit

				sManage->setcurrSceneID(sManage->getcurrSceneID() + 1);
				(sManage->getScene())->Init();
			}
		}
		else if (IsKeyPressed(VK_NUMPAD1)) //go backward in scene array
		{
			if (sManage->getcurrSceneID() != 0)
			{
				(sManage->getScene())->Exit(); //current scene exit

				sManage->setcurrSceneID(sManage->getcurrSceneID() - 1);
				(sManage->getScene())->Init();
			}
		}

		sManage->Update(m_timer.getElapsedTime());
		//Swap buffers
		glfwSwapBuffers(m_window);
		glfwPollEvents();
		m_timer.waitUntil(frameTime);
	}

	sManage->~SceneManager(); //delete scene ptrs
}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}
