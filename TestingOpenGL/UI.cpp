#pragma once
#include "UI.h"
#include <AntTweakBar.h>
#include "MyGame.h"
#include "TerrainManager.h"
#include <time.h>

UI::UI()
{

}


UI::~UI()
{
}

void TW_CALL GenerateBlockButton(void *clientData)
{
	MyGame* game = static_cast<MyGame*>(clientData);
	game->_camera.SetPosition(glm::vec3(64, 30, 64));
	game->UpdateCamera();
	game->terrain.Initialise();
}

void TW_CALL GenerateNewSeed(void *clientData)
{
	TerrainManager* terrain = static_cast<TerrainManager*>(clientData);
	terrain->GlobalSeed = clock();
}

int GetModeNumber(UI* ui)
{
	int modeNumber = 0;
	switch (ui->mode)
	{
	case LightMode::COLOURFOG:
		modeNumber = 0;
		break;
	case LightMode::COLOURLIGHT:
		modeNumber = 1;
		break;
	case LightMode::COLOUR:
		modeNumber = 2;
		break;
	case LightMode::LIGHT:
		modeNumber = 3;
		break;
	case LightMode::NORMAL:
		modeNumber = 4;
		break;
	default:
		break;
	}
	return modeNumber;
}

void TW_CALL UpdateLighting(void *clientData)
{
	UI* ui = static_cast<UI*>(clientData);
	ui->gameInstance->UpdateLightVariables(GetModeNumber(ui));
}

void TW_CALL GetMode(void *value, void *clientData)
{
	UI* ui = static_cast<UI*>(clientData);
	*(LightMode *)value = ui->mode;
}

void TW_CALL SetMode(const void *value, void *clientData)
{
	UI* ui = static_cast<UI*>(clientData);
	ui->mode = *(LightMode *)value;
	ui->gameInstance->UpdateLightVariables(GetModeNumber(ui));
}

void TW_CALL GetCamMode(void *value, void *clientData)
{
	UI* ui = static_cast<UI*>(clientData);
	*(CameraMode *)value = ui->camMode;
}

void TW_CALL SetCamMode(const void *value, void *clientData)
{
	UI* ui = static_cast<UI*>(clientData);
	ui->camMode = *(CameraMode *)value;
	//ui->gameInstance->UpdateLightVariables(GetModeNumber(ui));

	int modeNumber = 0;
	switch (ui->camMode)
	{
	case CameraMode::ABOVE:
		ui->gameInstance->fly = false;
		ui->gameInstance->_camera._position.y = 900.0f;
		ui->gameInstance->_camera.pitch = -89.0f;
		ui->gameInstance->_camera.updateMatricies();
		ui->gameInstance->UpdateCamera();
		modeNumber = 0;
		break;
	case CameraMode::FLY:
		ui->gameInstance->fly = true;
		modeNumber = 1;
		break;
	case CameraMode::FREE:
		ui->gameInstance->fly = false;
		ui->gameInstance->_camera._position.y = 100.0f;
		ui->gameInstance->_camera.pitch = 0.0f;
		ui->gameInstance->_camera.updateMatricies();
		ui->gameInstance->UpdateCamera();
		modeNumber = 2;
		break;
	default:
		break;
	}
}

void TW_CALL GetLOD(void *value, void *clientData)
{
	MyGame* game = static_cast<MyGame*>(clientData);
	*(float *)value = game->lod_factor;
}

void TW_CALL SetLOD(const void *value, void *clientData)
{
	MyGame* game = static_cast<MyGame*>(clientData);
	game->lod_factor = *(const float *)value;
	game->UpdateRenderVariables();
}

void TW_CALL GetFogMin(void *value, void *clientData)
{
	MyGame* game = static_cast<MyGame*>(clientData);
	*(float *)value = game->fogMin;
}

void TW_CALL SetFogMin(const void *value, void *clientData)
{
	MyGame* game = static_cast<MyGame*>(clientData);
	game->fogMin = *(const float *)value;
	game->UpdateLightVariables(GetModeNumber(game->ui));
}

void TW_CALL GetFogMax(void *value, void *clientData)
{
	MyGame* game = static_cast<MyGame*>(clientData);
	*(float *)value = game->fogMax;
}

void TW_CALL SetFogMax(const void *value, void *clientData)
{
	MyGame* game = static_cast<MyGame*>(clientData);
	game->fogMax = *(const float *)value;
	game->UpdateLightVariables(GetModeNumber(game->ui));
}

void TW_CALL GetLight(void *value, void *clientData)
{
	MyGame* game = static_cast<MyGame*>(clientData);
	*(glm::vec3 *)value = game->lightIntensity;
}

void TW_CALL SetLight(const void *value, void *clientData)
{
	MyGame* game = static_cast<MyGame*>(clientData);
	game->lightIntensity = *(const glm::vec3 *)value;
	game->UpdateLightVariables(GetModeNumber(game->ui));
}

void UI::init(int screenWidth, int screenHeight, MyGame* GameInstance)
{
	gameInstance = GameInstance;
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(screenWidth, screenHeight);
	bar = TwNewBar("TerrainGeneration");
	TwDefine(" TerrainGeneration refresh=1.0 label='Terrain Generation' size='260 385' valueswidth=fit resizable=false fontresizable = false movable=false ");	
	TwDefine(" GLOBAL buttonalign=left ");
}

void UI::LoadVariables()
{
	// Add Variables and Buttons
	TwAddVarRO(bar, "Avg FPS", TW_TYPE_INT32, &gameInstance->fps.avg_fps, " group=Stats");
	TwAddVarRW(bar, "Wireframe", TW_TYPE_BOOL32, &gameInstance->terrain.wireframe, " group=Stats");
	TwAddVarRW(bar, "V-Sync", TW_TYPE_BOOL32, &vsync, " group=Stats");
	TwAddVarCB(bar, "LOD Factor", TW_TYPE_FLOAT, SetLOD, GetLOD, gameInstance, " group=Stats");

	TwAddVarRW(bar, "Animate Light", TW_TYPE_BOOL32, &gameInstance->animateLight, " group=Lighting");
	TwAddVarCB(bar, "Light Intensity", TW_TYPE_COLOR3F, SetLight, GetLight, gameInstance, " group=Lighting");

	TwEnumVal lightModeEV[5];
	lightModeEV[0] = { COLOURFOG, "Colour Light Fog" };
	lightModeEV[1] = { COLOURLIGHT, "Colour Light" };
	lightModeEV[2] = { COLOUR, "Colour" };
	lightModeEV[3] = { LIGHT, "Light" };
	lightModeEV[4] = { NORMAL, "Normals" };
	TwType modeType = TwDefineEnum("Mode", lightModeEV, 5);
	TwAddVarCB(bar, "Lighting Mode", modeType, SetMode, GetMode, this, " group=Lighting");

	TwAddVarCB(bar, "Fog Min", TW_TYPE_FLOAT, SetFogMin, GetFogMin, gameInstance, " group=Lighting min=0 max=500");
	TwAddVarCB(bar, "Fog Max", TW_TYPE_FLOAT, SetFogMax, GetFogMax, gameInstance, " group=Lighting min=0 max=1000");

	TwAddVarRW(bar, "Camera Position", TW_TYPE_STDSTRING, &gameInstance->cameraString, " group=Camera");
	TwEnumVal camModeEV[3];
	camModeEV[0] = { FLY, "Fly" };
	camModeEV[1] = { FREE, "Free Cam" };
	camModeEV[2] = { ABOVE, "Looking Down" };
	TwType camModeType = TwDefineEnum("Camera Mode", camModeEV, 3);
	TwAddVarCB(bar, "Camera Mode", camModeType, SetCamMode, GetCamMode, this, " group=Camera");
	
	TwAddVarRW(bar, "Seed", TW_TYPE_INT32, &gameInstance->terrain.GlobalSeed, " group='Terrain Controls' min=0");
	TwAddButton(bar, "New Seed", GenerateNewSeed, &gameInstance->terrain, " group='Terrain Controls'");
	TwAddVarRW(bar, "Roughness", TW_TYPE_FLOAT, &gameInstance->terrain.roughness, " group='Terrain Controls' min=0 max=1 step=0.05");
	TwAddVarRW(bar, "Vary Roughness", TW_TYPE_BOOL32, &gameInstance->terrain.VaryRoughness, " group='Terrain Controls'");
	TwAddVarRW(bar, "Smooth Constant", TW_TYPE_FLOAT, &gameInstance->terrain.smoothConst, " group='Terrain Controls' min=0 max=1 step=0.05");
	TwAddVarRW(bar, "Smooth Iterations", TW_TYPE_INT32, &gameInstance->terrain.smoothIterations, " group='Terrain Controls' min=0 max=10");
	TwAddVarRW(bar, "Block Depth", TW_TYPE_INT32, &gameInstance->terrain.blockDepth, " group='Terrain Controls' min=1");
	TwAddButton(bar, "Generate", GenerateBlockButton, gameInstance, " group='Terrain Controls'");
}

void UI::Update()
{
	TwSetParam(bar, "Fog Max", "min", TwParamValueType::TW_PARAM_FLOAT, 1, &gameInstance->fogMin);
	TwSetParam(bar, "Fog Min", "max", TwParamValueType::TW_PARAM_FLOAT, 1, &gameInstance->fogMax);

	if (vsync)
		SDL_GL_SetSwapInterval(1);
	else
		SDL_GL_SetSwapInterval(0);
}

void UI::Draw()
{
	TwDraw();
}

void UI::UnLoad()
{
	TwTerminate();
}

void UI::Visible()
{
	visible = !visible;
	if (visible) {
		TwDefine(" TerrainGeneration visible=true ");
	}
	else {
		TwDefine(" TerrainGeneration visible=false ");
	}
}
