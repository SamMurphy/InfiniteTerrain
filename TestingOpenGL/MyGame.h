#pragma once
#include "UI.h"
#include "Game.h"
#include "Block.h"
#include "TerrainManager.h"
#include <time.h>

class UI;

class MyGame :
	public Game
{
public:
	MyGame();
	MyGame(const std::string &windowName, int screenWidth, int screenHeight) : Game(windowName, screenWidth, screenHeight) {};
	~MyGame();
	void UpdateCamera();

	// Lighting and Fog Variables
	glm::vec3 lightIntensity = glm::vec3(1.0f, 1.0f, 1.0f);
	bool fog = false;
	float fogMin = 100;
	float fogMax = 200;
	bool animateLight = false;
	// Render Variables
	float lod_factor = 4.0f;

	void UpdateLightVariables(int mode = 0);
	void UpdateRenderVariables();
	
	UI* ui;
	TerrainManager terrain;

	std::string cameraString;
	bool fly = false;
private:
	void OnLoad();
	void OnKeyPress(SDL_Keycode key);
	void OnUpdateFrame();
	void OnRenderFrame();
	void OnUnload();

	glm::vec4 lightPos = glm::vec4(0.0f, 5000.0f, 0.0f, 1.0f);
};

