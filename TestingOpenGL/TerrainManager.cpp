#pragma once
#include "TerrainManager.h"
#include <time.h>
#include <thread>
#include "Camera3D.h";
#include <math.h>


TerrainManager::TerrainManager()
{
}

TerrainManager::~TerrainManager()
{
}

void TerrainManager::OnLoad()
{
	// Initilise the shader
	if (!terrainShader.compileShaderFromFile("Shaders/SimpleVertex3.vertexshader", GLSLShader::VERTEX)) printf("Vertex Shader Compilation Error");
	if (!terrainShader.compileShaderFromFile("Shaders/SimpleTessCon3.tessconshader", GLSLShader::TESS_CONTROL)) printf("Tessellation Control Shader Compilation Error");
	if (!terrainShader.compileShaderFromFile("Shaders/SimpleTessEval3.tessvalshader", GLSLShader::TESS_EVALUATION)) printf("Tessellation Evaluation Shader Compilation Error");
	if (!terrainShader.compileShaderFromFile("Shaders/SimpleFragment3.fragmentshader", GLSLShader::FRAGMENT)) printf("Fragment Shader Compilation Error");
	if (!terrainShader.link()) printf("Shader Linking Error");
	if (!terrainShader.validate()) printf("Shader Validation Error");
	terrainShader.use();
	terrainShader.printUniforms();
	terrainShader.printAttributes();

	GenerateVAO();

	terrainShader.setUniform("blockSize", 128.0f);
	tileSize = 128.0f;

	Initialise();

	if (!generationThreadRunning)
		generationThread = spawn();
}

void TerrainManager::Initialise()
{
	generatingInitialTiles = true;
	cachedSeed = GlobalSeed;
	lastCheckedPosition = glm::vec2(64, 64);
	// Delete existing blocks and data
	mtx.lock();
	for each (Block* b in blocksToRender)
	{
		delete b;
	}
	blocksToRender.clear();
	for each (blockData* b in generatedBlockData)
	{
		delete b;
	}
	generatedBlockData.clear();
	mtx.unlock();

	// Add Blocks
	AddSurroundingBlocks(0.0f, 0.0f);
	generatingInitialTiles = false;
}

void TerrainManager::GenerateVAO()
{
	// Create the vertex array object
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	verticies.push_back(glm::vec3(0.0f, 0.0f, 0.0f));		//0
	verticies.push_back(glm::vec3(0.0f, 0.0f, 64.0f));		//1
	verticies.push_back(glm::vec3(0.0f, 0.0f, 128.0f));		//2
	verticies.push_back(glm::vec3(64.0f, 0.0f, 0.0f));		//3
	verticies.push_back(glm::vec3(64.0f, 0.0f, 64.0f));		//4
	verticies.push_back(glm::vec3(64.0f, 0.0f, 128.0f));	//5
	verticies.push_back(glm::vec3(128.0f, 0.0f, 0.0f));		//6
	verticies.push_back(glm::vec3(128.0f, 0.0f, 64.0f));	//7
	verticies.push_back(glm::vec3(128.0f, 0.0f, 128.0f));	//8

	indices = { 6, 7, 4, 3,
				7, 8, 5, 4,
				4, 5, 2, 1,
				3, 4, 1, 0 };

	// Create & Bind Buffer for vertex array
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(glm::vec3), &verticies[0], GL_STATIC_DRAW);
	// Enable attrib pointer for vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);

	// Create & Bind Index buffer to store indices array.
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void TerrainManager::OnUpdate(Camera3D &camera)
{
	if (generatingInitialTiles) return;
	cameraXZposition = glm::vec2(camera._position.x, camera._position.z);

	std::vector<int> toDelete;
	mtx.lock();
	for (int i = 0; i < blocksToRender.size(); i++)
	{
		// centre point of block
		glm::vec2 blockPosition = glm::vec2(blocksToRender[i]->X + (tileSize / 2.0f), blocksToRender[i]->Z + (tileSize / 2.0f));
		float length = glm::length((blockPosition - cameraXZposition));
		// Check if in render distance
		if (length >(tileSize * blockDepth) + tileSize)
		{
			// Delete Block
			toDelete.push_back(i);
		}
	}
	for (int i = 0; i < toDelete.size(); i++)
	{
		if (blocksToRender.size() > toDelete.at(i))
		{
			delete blocksToRender.at(toDelete.at(i));
			blocksToRender.erase(blocksToRender.begin() + toDelete.at(i));
		}
	}
	toDelete.clear();
	mtx.unlock();
}

void TerrainManager::OnRender()
{
	terrainShader.use();
	glBindVertexArray(vao);

	if (wireframe)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	mtx.lock();
	for (int i = 0; i < blocksToRender.size(); i++)
	{
		//Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(blocksToRender[i]->X, 0.0f, blocksToRender[i]->Z));
		// Get a handle for our "MVP" uniform
		terrainShader.setUniform("Model", Model);

		glActiveTexture(GL_TEXTURE4);

		if (blocksToRender[i]->Texture >= 4294967295)
			blocksToRender[i]->Texture = blocksToRender[i]->StoreHeightMapAsTexture();
		glBindTexture(GL_TEXTURE_2D, blocksToRender[i]->Texture);
		terrainShader.setUniform("heightNormalMap", 4);

		glEnable(GL_TEXTURE_1D);
		glEnable(GL_TEXTURE_2D);
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, nullptr);
		//glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, nullptr);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_1D);
	}
	mtx.unlock();

	if (wireframe)
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}

	glBindVertexArray(0);
}

void TerrainManager::OnUnload()
{
	generationThreadRunning = false;
	generationThread.join();
	mtx.lock();
	for each (Block* b in blocksToRender)
	{
		delete b;
	}
	blocksToRender.clear();
	for each (blockData* b in generatedBlockData)
	{
		delete b;
	}
	generatedBlockData.clear();
	mtx.unlock();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Cleanup VBO and shader
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexBuffer);

	glBindVertexArray(0);
	glDeleteProgram(terrainShader.ID());
	glDeleteVertexArrays(1, &vao);
}

void TerrainManager::AddBlock(float x, float z)
{
	mtx.lock();
	for (int i = 0; i < blocksToRender.size(); i++)
	{
		if (blocksToRender[i]->X == x && blocksToRender[i]->Z == z)
		{
			mtx.unlock();
			return;
		}
	}
	mtx.unlock();

	Block* tempBlock = new Block();
	tempBlock->X = x;
	tempBlock->Z = z;
	tempBlock->Seed = cachedSeed * x + z;
	tempBlock->roughness = roughness;
	tempBlock->smoothConst = smoothConst;
	tempBlock->smoothIterations = smoothIterations;
	tempBlock->initialiseGrid();
	
	// north = 1, south = 2, east = 3, west = 4
	float roughnessTotal = 0;
	int numberOfSurroundingBlocks = 0;
	mtx.lock();
	for each (blockData* b in generatedBlockData)
	{
		if (b->x == x && b->z == z)
		{
			tempBlock->SetEdge(1, b->north);
			tempBlock->SetEdge(2, b->south);
			tempBlock->SetEdge(3, b->east);
			tempBlock->SetEdge(4, b->west);
			tempBlock->roughness = b->roughness;
			break;
		}
		// Check North
		if (b->x == x && b->z == z + tileSize)
		{
			tempBlock->SetEdge(1, b->south);
			numberOfSurroundingBlocks++;
			roughnessTotal = b->roughness;
		}
		// Check South
		if (b->x == x && b->z == z - tileSize)
		{
			tempBlock->SetEdge(2, b->north);
			numberOfSurroundingBlocks++;
			roughnessTotal = b->roughness;
		}
		// Check East
		if (b->x == x - tileSize && b->z == z)
		{
			tempBlock->SetEdge(3, b->west);
			numberOfSurroundingBlocks++;
			roughnessTotal = b->roughness;
		}
		// Check West
		if (b->x == x + tileSize && b->z == z)
		{
			tempBlock->SetEdge(4, b->east);
			numberOfSurroundingBlocks++;
			roughnessTotal = b->roughness;
		}
	}
	if (numberOfSurroundingBlocks != 0 && VaryRoughness)
	{
		roughnessTotal = roughnessTotal / (float)numberOfSurroundingBlocks;
		tempBlock->roughness = roughnessTotal + (((fRand(0.0f, 4.0f) -1.0f) / 100.0f) * 5.0f);
		tempBlock->roughness = glm::clamp(tempBlock->roughness, 0.1f, 0.9f);
	}
	mtx.unlock();

	// Generate Block
	
	tempBlock->generateGrid(); // Too Slow to do on main thread
	// Add to lists
	SaveBlockData(tempBlock);
	mtx.lock();
	blocksToRender.push_back(tempBlock);
	mtx.unlock();
}

void TerrainManager::SaveBlockData(Block* block)
{
	blockData* b = new blockData();
	b->x = block->X;
	b->z = block->Z;
	for (int i = 0; i < tileSize; i++)
	{
		b->north.push_back(block->get(tileSize - 1, i));
		b->south.push_back(block->get(0, i));
		b->east.push_back(block->get(i, 0));
		b->west.push_back(block->get(i, tileSize - 1));
	}
	b->roughness = block->roughness;
	mtx.lock();
	generatedBlockData.push_back(b);
	mtx.unlock();
}

glm::vec2 TerrainManager::getBlockCoordinatesFromCamera(float x, float z)
{
	// output coordinates
	float blockX, blockZ;
	// Divide by block size
	x /= tileSize;
	z /= tileSize;
	// Get integer part which is the block number
	x = std::modf(x, &blockX);
	z = std::modf(z, &blockZ);
	// -1 if negative for block offset
	blockX < 0 ? blockX -= 1 : blockX = blockX;
	blockZ < 0 ? blockZ -= 1 : blockZ = blockZ;
	// Multiple by tile size to get coordinate for bottom left of tile
	blockX *= tileSize;
	blockZ *= tileSize;
	// Return coordinate for bottom left of tile
	return glm::vec2(blockX, blockZ);
}

float TerrainManager::getHeightFromCamera(float x, float z)
{
	float height = 50.0f;
	// output coordinates
	float blockX, blockZ;
	// Divide by block size
	x /= tileSize;
	z /= tileSize;
	// Get integer part which is the block number
	x = std::modf(x, &blockX);
	z = std::modf(z, &blockZ);
	// -1 if negative for block offset
	blockX < 0 ? blockX -= 1 : blockX = blockX;
	blockZ < 0 ? blockZ -= 1 : blockZ = blockZ;
	// Multiple by tile size to get coordinate for bottom left of tile
	blockX *= tileSize;
	blockZ *= tileSize;
	// Relative coordinte in grid space
	int rX = abs((int)(x * tileSize));
	int rZ = abs((int)(z * tileSize));

	Block* b = nullptr;
	mtx.lock();
	for (int i = 0; i < blocksToRender.size(); i++)
	{
		if (blocksToRender[i]->X == blockX && blocksToRender[i]->Z == blockZ)
		{
			b = blocksToRender[i];
			break;
		}
	}
	mtx.unlock();
	if (b != nullptr)
	{
		height = b->get(rX, rZ);
	}
	return height;
}

void TerrainManager::GenerateTerrain()
{
	printf("generation thread starting... \n");
	int cachedBlockDepth = blockDepth;
	generationThreadRunning = true;
	while (generationThreadRunning)
	{
		if (!generatingInitialTiles)
		{
			if (cachedBlockDepth != blockDepth)
			{
				// Block depth has been updated
				glm::vec2 currentBlockPosition = getBlockCoordinatesFromCamera(cameraXZposition.x, cameraXZposition.y);
				AddSurroundingBlocks(currentBlockPosition.x, currentBlockPosition.y);
				cachedBlockDepth = blockDepth;
			}
			// Check when more tiles were last added, if traveled far enough then add more.
			float distanceSinceLastChecked = glm::length(lastCheckedPosition - cameraXZposition);
			if (distanceSinceLastChecked > tileSize / 4.0f)
			{
				// Find current block
				glm::vec2 currentBlockPosition = getBlockCoordinatesFromCamera(cameraXZposition.x, cameraXZposition.y);
				// Add surrounding in the same style as above
				AddSurroundingBlocks(currentBlockPosition.x, currentBlockPosition.y);
				lastCheckedPosition = cameraXZposition;
			}
		}
	}
	
	printf("generation thread ending... \n");
}

// 21 13 09 14 22
// 17 05 01 06 19
// 11 03 00 04 12
// 18 07 02 08 20
// 23 15 10 16 24
void TerrainManager::AddSurroundingBlocks(float x, float z)
{
	AddBlock(x, z);
	int i;
	for (i = 1; i <= blockDepth; i++)
	{
		AddBlock(x, z + (tileSize * i)); // 09
		AddBlock(x, z - (tileSize * i)); // 10
		AddBlock(x - (tileSize * i), z); // 11
		AddBlock(x + (tileSize * i), z); // 12
		AddBlock(x - tileSize, z + (tileSize * i)); // 13
		AddBlock(x + tileSize, z + (tileSize * i)); // 14
		AddBlock(x - tileSize, z - (tileSize * i)); // 15
		AddBlock(x + tileSize, z - (tileSize * i)); // 16
		AddBlock(x - (tileSize * i), z + tileSize); // 17
		AddBlock(x - (tileSize * i), z - tileSize); // 18
		AddBlock(x + (tileSize * i), z + tileSize); // 19
		AddBlock(x + (tileSize * i), z - tileSize); // 20
		AddBlock(x - (tileSize * i), z + (tileSize * i)); // 21
		AddBlock(x + (tileSize * i), z + (tileSize * i)); // 22
		AddBlock(x - (tileSize * i), z - (tileSize * i)); // 23
		AddBlock(x + (tileSize * i), z - (tileSize * i)); // 24
	}
	i--;
	if (blockDepth >= 3)
	{
		for (int j = 2; j < blockDepth; j++)
		{
			AddBlock(x - (tileSize * j), z + (tileSize * i)); // 13
			AddBlock(x + (tileSize * j), z + (tileSize * i)); // 14

			AddBlock(x - (tileSize * j), z - (tileSize * i)); // 15
			AddBlock(x + (tileSize * j), z - (tileSize * i)); // 16

			AddBlock(x - (tileSize * i), z + (tileSize * j)); // 17
			AddBlock(x - (tileSize * i), z - (tileSize * j)); // 18

			AddBlock(x + (tileSize * i), z + (tileSize * j)); // 19
			AddBlock(x + (tileSize * i), z - (tileSize * j)); // 20
		}
	}
}

float TerrainManager::fRand(float fMin, float fMax)
{
	float f = (float)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}
