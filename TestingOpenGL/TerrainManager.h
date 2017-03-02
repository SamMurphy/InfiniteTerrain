#pragma once
#include "Block.h"
#include "GLSLProgram.h"
#include "Camera3D.h"

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <thread>
#include <mutex>

class GLSLProgram;

struct blockData
{
	float x;
	float z;
	std::vector<float> north;
	std::vector<float> south;
	std::vector<float> east;
	std::vector<float> west;
	float roughness;
};

class TerrainManager
{
public:
	TerrainManager();
	~TerrainManager();

	// Shader used for rendering terrain
	GLSLProgram terrainShader;
	// Seed used as basis for generating per block seeds
	int GlobalSeed = 252521;
	float smoothConst = 1.0f;
	int smoothIterations = 1.0f;
	float roughness = 0.6f;
	

	bool wireframe;
	int blockDepth = 2;
	bool VaryRoughness;

	void OnLoad();
	void Initialise();
	void OnUpdate(Camera3D &camera);
	void OnRender();
	void OnUnload();
	float getHeightFromCamera(float x, float z);

private:
	// Stores the currently active blocks
	std::vector<Block*> blocksToRender;
	// Stores details about blocks that have been previously generated.
	std::vector<blockData*> generatedBlockData;
	float tileSize;
	

	void GenerateVAO();

	void AddBlock(float x, float z);
	void SaveBlockData(Block* block);

	std::vector<glm::vec3> verticies;
	std::vector<int> indices;

	GLuint indexBuffer;
	GLuint vao, vertexbuffer;

	glm::vec2 lastCheckedPosition;
	glm::vec2 getBlockCoordinatesFromCamera(float x, float z);

	void AddSurroundingBlocks(float x, float z);

	glm::vec2 cameraXZposition;
	void GenerateTerrain();
	std::thread generationThread;
	bool generationThreadRunning = false;
	std::mutex mtx;
	bool tilesWaiting = false;
	std::thread spawn() { return std::thread(&TerrainManager::GenerateTerrain, this); }

	bool generatingInitialTiles = false;
	int cachedSeed;

	float fRand(float fMin, float fMax);
};

