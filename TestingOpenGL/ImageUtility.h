#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "Window.h"
#include <glm\glm.hpp>

class ImageUtility
{
public:
	ImageUtility();
	~ImageUtility();

	static GLuint CreateTexture1D(const char *fileName, GLenum TextureUnit, Window window);
	static GLuint CreateTexture2D(const char *fileName, GLenum TextureUnit, Window window);
	static GLuint CreateTexture2D(std::vector<glm::vec4> data, int width, int height, GLenum TextureUnit);
	static SDL_Surface *loadImage(const char *filename, SDL_Surface* gScreenSurface = nullptr);

private:

};

