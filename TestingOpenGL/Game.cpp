#pragma once
#include "Game.h"
#include <AntTweakBar.h>


Game::Game() :
_window(),
_gameState(GameState::PLAY),
_windowName("GAME"),
_screenWidth(1280),
_screenHeight(720)
{
	std::cout << "Default contructor called" << std::endl;
}

Game::Game(const std::string &windowName, int screenWidth, int screenHeight) :
_window(),
_gameState(GameState::PLAY),
_windowName(windowName),
_screenWidth(screenWidth),
_screenHeight(screenHeight)
{
	std::cout << "Second constructor called" << std::endl;
}


Game::~Game()
{
}

void Game::run()
{
	std::cout << "Running Game" << std::endl;
	init();
	OnLoad();
	fps.fpsinit();
	gameLoop();
}

void Game::init()
{
	std::cout << "Initialised" << std::endl;
	// Initialise SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	_window.Create(_windowName, _screenWidth, _screenHeight);
	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// V-Sync
	SDL_GL_SetSwapInterval(1);

	_window.SetTitle("Fractal Landscapes - Sam Murphy");
}

void Game::gameLoop()
{
	std::cout << "Starting Game Loop" << std::endl;
	while (_gameState != GameState::EXIT)
	{
		processInput();
		OnUpdateFrame();
		OnRenderFrame();
		fps.fpsthink();
	}
	OnUnload();
}

void Game::OnLoad()
{
	std::cout << "Game::OnLoad called" << std::endl;
	// Camera
	_camera.init(glm::vec3(64, 30, 64), glm::vec3(0, 0, 1), 1.0f, (float)_screenWidth / (float)_screenHeight, 0.1f, 1000.0f, 1.0f, 1.0f);
	UpdateCamera();
}

void Game::processInput()
{
	//std::cout << "Game::Process Input" << std::endl;
	int handled;
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		handled = TwEventSDL(&evnt, SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
		if (!handled)
		{
			switch (evnt.type)
			{
			case SDL_QUIT:
				//OnUnload();
				_gameState = GameState::EXIT;
				break;
			case SDL_MOUSEMOTION:
				break;
			case SDL_KEYDOWN:
				OnKeyPress(evnt.key.keysym.sym);
				break;
			}
		}
	}
}

void Game::OnKeyPress(SDL_Keycode key)
{
	
}

void Game::OnUpdateFrame()
{
	
}

void Game::OnRenderFrame()
{
	// Swap buffers
	_window.Swap();
}

void Game::OnUnload()
{
	
}

void Game::UpdateCamera()
{
	_shader.setUniform("V", _camera.View());
	_shader.setUniform("P", _camera.Projection());
}
