#pragma once
#include "MyGame.h"
#include "Block.h"
#include "ImageUtility.h"
#include <AntTweakBar.h>

MyGame::MyGame()
{
}

MyGame::~MyGame()
{
}

void MyGame::OnLoad()
{
	ui = new UI();
	ui->init(_window.screenWidth(), _window.screenHeight(), this);

	std::cout << "MyGame::OnLoad called" << std::endl;

	terrain.OnLoad();

	ImageUtility::CreateTexture1D("Textures/rocksWU.png", GL_TEXTURE0, _window);
	terrain.terrainShader.setUniform("colorGradient", 0);

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices);

	// Set screen_size anda lod_factor
	UpdateRenderVariables();
	UpdateLightVariables();

	ui->LoadVariables();

	Game::OnLoad();
}

void MyGame::UpdateRenderVariables()
{
	terrain.terrainShader.setUniform("screen_size", glm::vec2(_window.screenWidth(), _window.screenHeight()));
	terrain.terrainShader.setUniform("lod_factor", lod_factor);
}

void MyGame::UpdateLightVariables(int mode)
{
	// Directional Light
	terrain.terrainShader.setUniform("Light.position", lightPos);
	terrain.terrainShader.setUniform("Light.intensities", lightIntensity);
	terrain.terrainShader.setUniform("Light.ambientCoefficient", 0.05f);
	// Fog
	terrain.terrainShader.setUniform("Fog.maxDistance", fogMax);
	terrain.terrainShader.setUniform("Fog.minDistance", fogMin);
	terrain.terrainShader.setUniform("Fog.colour", glm::vec3(0.529f, 0.808f, 0.9803f));
	// Lighting mode used for debugging
	terrain.terrainShader.setUniform("mode", mode);
}

void MyGame::OnKeyPress(SDL_Keycode key)
{
	//std::cout << "MyGame::OnKeyPress called" << std::endl;
	switch (key)
	{
	case SDLK_w:
		_camera.Forward();
		UpdateCamera();
		break;
	case SDLK_s:
		_camera.Backward();
		UpdateCamera();
		break;
	case SDLK_a:
		_camera.RotateLeft();
		UpdateCamera();
		break;
	case SDLK_d:
		_camera.RotateRight();
		UpdateCamera();
		break;
	case SDLK_q:
		_camera.LookUp();
		UpdateCamera();
		break;
	case SDLK_e:
		_camera.LookDown();
		UpdateCamera();
		break;
	case SDLK_SPACE:
		_camera.Up();
		UpdateCamera();
		break;
	case SDLK_LSHIFT:
		_camera.Down();
		UpdateCamera();
		break;
	case SDLK_ESCAPE:
		_gameState = GameState::EXIT;
		break;
	case SDLK_F1:
		ui->Visible();
		break;
	case SDLK_F2:
		fly = !fly;
		break;
	case SDLK_LEFT:
		_camera.StrafeLeft();
		UpdateCamera();
		break;
	case SDLK_RIGHT:
		_camera.StrafeRight();
		UpdateCamera();
		break;
	case SDLK_UP:
		_camera.ForwardZ();
		UpdateCamera();
		break;
	case SDLK_DOWN:
		_camera.BackwardZ();
		UpdateCamera();
		break;
	}
	Game::OnKeyPress(key);
}

void MyGame::OnUpdateFrame()
{
	ui->Update();
	//std::cout << "MyGame::OnUpdateFrame called" << std::endl;
	if (animateLight)
	{
		lightPos.x += 10.0f;
		terrain.terrainShader.setUniform("Light.position", lightPos);
	}
	else
	{
		lightPos = glm::vec4(0.0f, 5000.0f, 0.0f, 1.0f);
		terrain.terrainShader.setUniform("Light.position", lightPos);
	}
	terrain.OnUpdate(_camera);

	if (fly)
	{
		float TargetHeight = terrain.getHeightFromCamera(_camera.Position().x, _camera.Position().z) + 60.0f;
		if (_camera.Position().y > TargetHeight)
			_camera.SetPosition(glm::vec3(_camera.Position().x, _camera.Position().y - 0.5f, _camera.Position().z));
		else if (_camera.Position().y < TargetHeight)
			_camera.SetPosition(glm::vec3(_camera.Position().x, _camera.Position().y + 0.5f, _camera.Position().z));
		_camera.Forward();
		UpdateCamera();
	}
	cameraString = std::to_string((int)_camera.Position().x) + ", " 
				+ std::to_string((int)_camera.Position().y) + ", "
				+ std::to_string((int)_camera.Position().z);
	Game::OnUpdateFrame();
}

void MyGame::OnRenderFrame()
{
	//std::cout << "MyGame::OnRenderFrame called" << std::endl;
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	terrain.OnRender();
	ui->Draw();
	
	Game::OnRenderFrame();
}

void MyGame::OnUnload()
{
	ui->UnLoad();
	terrain.OnUnload();

	Game::OnUnload();
}

void MyGame::UpdateCamera()
{
	terrain.terrainShader.setUniform("V", _camera.View());
	terrain.terrainShader.setUniform("P", _camera.Projection());
}
