#pragma once
#include "Block.h"
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <iostream>
#include "ImageUtility.h"

void Block::initialiseGrid()
{
	if (grid != nullptr)
		delete[] grid;
	//if (indices != nullptr)
	//	delete[] indices;

	calcSize();
	max = _size - 1;
	numberOfTriangles = (_size - 1) * (_size - 1) * 2;

	grid = new float[_size*_size];
	for (int i = 0; i < _size * _size; i++)
	{
		//grid[i] = fRand(0.0f, 1.0f);
		grid[i] = 0.0f;
	}
}

void Block::generateGrid()
{
	srand(Seed);
	// Check if corners have been seeded
	//if (get(0, 0) == -9999.0f)
	//	set(0, 0, fRand(0.0f, 1.0f));
	//if (get(max, 0) == -9999.0f)
	//	set(max, 0, fRand(0.0f, 1.0f));
	//if (get(max, max) == -9999.0f)
	//	set(max, max, fRand(0.0f, 1.0f));
	//if (get(0, max) == -9999.0f)
	//	set(0, max, fRand(0.0f, 1.0f));
	
	divide(128);
	smooth(smoothConst, smoothIterations);
	generateVertexDataFromGrid();
	Texture = StoreHeightMapAsTexture();
}

void Block::set(int x, int y, float value)
{
	grid[x *_size + y] = value;
}

float Block::get(const int x, const int y)
{
	if (x < 0 || x > max || y < 0 || y > max) return -1;
	return grid[x * _size + y];
}

void Block::divide(int size)
{
	int x, y, half = size / 2;
	float scale = roughness * size;
	if (half < 1) return;

	for (y = half; y <= max; y += size) // max = _size - 1
	{
		for (x = half; x <= max; x += size)
		{
			square(x, y, half, fRand(0.0f, 1.0f) * scale * 2 - scale);
		}
	}
	for (y = 0; y <= max; y += half)
	{
		for (x = (y + half) % size; x <= max; x += size)
		{
			diamond(x, y, half, fRand(0.0f, 1.0f) * scale * 2 - scale);
		}
	}
	divide(size / 2);
}

float Block::average(float one, float two, float three, float four)
{
	return((one + two + three + four) / 4);
}

void Block::square(int x, int y, int size, float offset)
{
	if (get(x, y) == 0.0f)
	{
		float avg = average(get((x - size), (y - size)),
			get((x + size), (y - size)),
			get((x + size), (y + size)),
			get((x - size), (y + size)));
		set(x, y, avg + offset);
	}
}

void Block::diamond(int x, int y, int size, float offset)
{
	if (get(x, y) == 0.0f)
	{
		//std::cout << offset << std::endl;
		float avg = average(get(x, (y - size)),
			get((x + size), y),
			get(x, (y + size)),
			get((x - size), y));
		set(x, y, avg + offset);
	}
}

void Block::generateVertexDataFromGrid()
{
	// Clear arrays / vectors
	if (verticies.size() > 0)
		verticies.clear();
	if (normals.size() > 0)
		normals.clear();
	if (indices.size() > 0)
		indices.clear();

	verticies.resize(_size * _size);
	normals.resize(_size * _size);
	indices.resize(numberOfTriangles * 3);
	// Create vector of verticies from height map (grid) assinging x and z coordinates
	float interval = 128.0f / float(_size - 1);
	for (int x = 0; x < _size; x++)
	{
		for (int z = 0; z < _size; z++)
		{
			verticies[x *_size + z] = glm::vec3(interval * x, grid[x * _size + z], interval * z);
		}
	}
	// Store numbered indicies in indicies array
	int index = 0;
	for (int x = 0; x < _size - 1; x++)
	{
		for (int z = 0; z < _size - 1; z++)
		{
			indices[index++] = x * _size + z;
			indices[index++] = (x + 1) * _size + z;
			indices[index++] = (x + 1) * _size + (z + 1);

			indices[index++] = x * _size + z;
			indices[index++] = (x + 1) * _size + (z + 1);
			indices[index++] = x * _size + (z + 1);
		}
	}
	// Calculate Normals
	for (int i = 0; i < numberOfTriangles * 3; i += 3)
	{
		// Get the three points of the triangle
		glm::vec3 v0 = verticies[indices[i + 0]];
		glm::vec3 v1 = verticies[indices[i + 1]];
		glm::vec3 v2 = verticies[indices[i + 2]];
		// Calculate and normalise the normal vector
		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
		// Add normal vector to temp normal storage
		normals[indices[i + 0]] += normal;
		normals[indices[i + 1]] += normal;
		normals[indices[i + 2]] += normal;
	}
	for (int i = 0; i < normals.size(); i++)
	{
		normals[i] = glm::normalize(normals[i]);
	}
	//range();
}

// Smooths the heightmap based on the smoothConstant - between 0 and 1
// 1 = height will be unchanged
// 0 = height will be set to the same as it's neighbour
// NumberOfIterations determines how many times the smoothing process will be done on the heightMap
// The more iterations the smoother it will be
void Block::smooth(float smoothConstant, int numberOfIterations)
{
	smoothConstant = glm::clamp(smoothConstant, 0.0f, 1.0f);
	for (int i = 0; i <= numberOfIterations; i++)
	{
		/* Rows, left to right */
		for (int x = 2; x < max; x++)
		{
			for (int z = 1; z < max; z++)
			{
				grid[x * _size + z] = grid[(x - 1) * _size + z] * (1 - smoothConstant) + grid[x * _size + z] * smoothConstant;
			}
		}
		/* Rows, right to left*/
		for (int x = max - 2; x <= -1; x--)
		{
			for (int z = 1; z < max; z++)
			{
				grid[x * _size + z] = grid[(x + 1) * _size + z] * (1 - smoothConstant) + grid[x * _size + z] * smoothConstant;
			}
		}
		/* Columns, bottom to top */
		for (int x = 1; x < max; x++)
		{
			for (int z = 2; z < max; z++)
			{
				grid[x * _size + z] = grid[x * _size + (z - 1)] * (1 - smoothConstant) + grid[x * _size + z] * smoothConstant;
			}
		}

		/* Columns, top to bottom */
		for (int x = 1; x < max; x++)
		{
			for (int z = max - 1; z <= -1; z--)
			{
				grid[x * _size + z] = grid[x * _size + (z + 1)] * (1 - smoothConstant) + grid[x * _size + z] * smoothConstant;
			}
		}
	}
}

GLuint Block::StoreHeightMapAsTexture()
{
	GLuint texture = -1;
	// Check for data
	if (normals.empty() || grid == nullptr)
	{
		printf("Error converting height & normals to texture: not enought data");
		return texture;
	}

	std::vector<glm::vec4> normalsAndHeight;

	for (int i = 0; i < normals.size(); i++)
	{
		normalsAndHeight.push_back(glm::vec4(normals[i], verticies[i].y));
	}

	texture = ImageUtility::CreateTexture2D(normalsAndHeight, _size, _size, GL_TEXTURE4);
	return texture;
}

float Block::fRand(float fMin, float fMax)
{
	float f = (float)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

float Block::range()
{
	float big = 0;
	float small = 1000;
	for (int i = 0; i < _size * _size; i++)
	{
		if (grid[i] > big)
		{
			big = grid[i];
		}
		if (grid[i] < small)
		{
			small = grid[i];
		}
	}
	float range = big - small;
	std::cout << "RANGE CALCULATED AS: " << range << std::endl << "SMALLEST: " << small << std::endl << "BIGGEST: " << big << std::endl;
	return range;
}

// Sets the edge heights of the grid to the passed values
// edgeNumber
// 1 = North
// 2 = South
// 3 = East
// 4 = West
void Block::SetEdge(int edgeNumber, std::vector<float> values)
{
	switch (edgeNumber)
	{
		case 1: // North
			for (int i = 0; i < values.size(); i++)
			{
				//set(i, max, values[i]);
				set(max, i, values[i]);
			}
			break;
		case 2: // South
			for (int i = 0; i < values.size(); i++)
			{
				//set(i, 0, values[i]);
				set(0, i, values[i]);
			}
			break;
		case 3: // East
			for (int i = 0; i < values.size(); i++)
			{
				//set(max, i, values[i]);
				set(i, 0, values[i]);
			}
			break;
		case 4: // West
			for (int i = 0; i < values.size(); i++)
			{
				//set(0, i, values[i]);
				set(i, max, values[i]);
			}
			break;
	}
}