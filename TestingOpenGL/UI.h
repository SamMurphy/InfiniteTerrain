#pragma once
#include <AntTweakBar.h>
#include "MyGame.h";

class MyGame;

typedef enum { COLOURFOG, COLOURLIGHT, COLOUR, LIGHT, NORMAL } LightMode;
typedef enum { FLY, FREE, ABOVE } CameraMode;

class UI
{
public:
	UI();
	~UI();
	
	void init(int screenwidth, int screenHeight, MyGame * gameInstance);
	void LoadVariables();
	void Update();
	void Draw();
	void UnLoad();
	void Visible();

	LightMode mode = COLOURFOG;
	CameraMode camMode = FREE;
	MyGame * gameInstance;
	bool vsync = true;
private:
	TwBar * bar;
	bool visible = true;
};

