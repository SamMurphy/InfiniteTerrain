#pragma once
#include <iostream>
#include "Window.h"


Window::Window()
{
	std::cout << "Window constructor called" << std::endl;
}


Window::~Window()
{

}

/// <summary>Creates a SDL window and openGL context and initilises glew </summary>
void Window::Create(const std::string &windowTitle, int screenWidth, int screenHeight)
{
	std::cout << "Window Created" << std::endl;
	_screenHeight = screenHeight;
	_screenWidth = screenWidth;
	//Open an SDL window
	_window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (_window == nullptr)
	{
		printf("SDL Window could not be created!");
	}

	_context = SDL_GL_CreateContext(_window);
	if (_context == nullptr)
	{
		printf("SDL_GL context could not be created!");
	}

	
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) 
	{
		printf("Failed to initialize GLEW\n");
	}

	// Dark blue background 135-206-250
	glClearColor(0.529f, 0.808f, 0.9803f, 1.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CW); // GL_CCW for counter clock-wise

	glViewport(0, 0, _screenWidth, _screenHeight);

	SDL_WINDOW_INPUT_GRABBED;
}

void Window::Swap()
{
	SDL_GL_SwapWindow(_window);
}

void Window::GetScreenSize()
{
	SDL_GetWindowSize(_window, &_screenWidth, &_screenHeight);
}

void Window::SetTitle(const std::string &title)
{
	SDL_SetWindowTitle(_window, title.c_str());
}
