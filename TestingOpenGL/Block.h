#pragma once
#include <math.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Block
{
public:
	Block() : detail(7), _size(128), max(_size - 1), roughness(0.7f), smoothConst(1.0f), smoothIterations(1)
	{
	}

	float* grid;
	int numberOfTriangles;

	std::vector<glm::vec3> verticies;
	std::vector<glm::vec3> normals;
	std::vector<int> indices;

	int detail;
	float roughness;
	float smoothConst;
	int smoothIterations;

	int Seed;
	float X;
	float Z;
	GLuint Texture;

	int gridArraySize() { return _size; }
	void set(int x, int y, float value);
	float get(const int x, const int y);

	void initialiseGrid();
	void SetEdge(int edgeNumber, std::vector<float> values);
	void generateGrid();
	GLuint StoreHeightMapAsTexture();

	int Detail() { return detail; }
	float Roughness() { return roughness; }
	float SmoothConst() { return smoothConst; }
	int SmoothIterations() { return smoothIterations; }
	
	~Block()
	{
		verticies.clear();
		normals.clear();
		indices.clear();
		delete[] grid;
	}

private:
	int _size;
	int max;

	void divide(int size);
	float average(float one, float two, float three, float four);
	void square(int x, int y, int size, float offset);
	void diamond(int x, int y, int size, float offset);

	void smooth(float smoothConstant, int numberOfIterations = 1);
	void generateVertexDataFromGrid();
	

	void calcSize() { _size = pow(2.0, detail); }

	float fRand(float fMin, float fMax);
	float range();
};