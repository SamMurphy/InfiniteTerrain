#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Window.h"
#include "GLSLProgram.h"
#include "Camera3D.h"
#include "fpsCounter.h"

enum class GameState { PLAY, EXIT };

class Game
{
public:
	Game();
	Game(const std::string &windowName, int screenWidth, int screenHeight);
	~Game();

	void run();

	fpsCounter fps;
	Camera3D _camera;
protected:
	Window _window;
	GameState _gameState;

	std::string _windowName;
	int _screenWidth;
	int _screenHeight;

	

	virtual void OnLoad();
	virtual void OnKeyPress(SDL_Keycode key);
	virtual void OnUpdateFrame();
	virtual void OnRenderFrame();
	virtual void OnUnload();

	virtual void UpdateCamera();

	GLSLProgram _shader;

private:
	void init();
	void gameLoop();
	void processInput();

	
	bool skip;
};

