#pragma once
#include "ImageUtility.h"
#include <vector>
#include <glm\glm.hpp>

ImageUtility::ImageUtility()
{
}


ImageUtility::~ImageUtility()
{
}

GLuint ImageUtility::CreateTexture1D(const char *fileName, GLenum TextureUnit, Window window)
{
	SDL_Surface* tempSurface;
	tempSurface = loadImage(fileName, SDL_GetWindowSurface(window.GetWindow()));
	Uint8 colours = tempSurface->format->BytesPerPixel;
	GLenum texture_format;
	if (colours == 4) {   // alpha
		if (tempSurface->format->Rmask == 0x000000ff)
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	}
	else {             // no alpha
		if (tempSurface->format->Rmask == 0x000000ff)
			texture_format = GL_RGB;
		else
			texture_format = GL_BGR;
	}

	GLuint thisIsMyTexture;
	glActiveTexture(TextureUnit);
	glGenTextures(1, &thisIsMyTexture);
	glBindTexture(GL_TEXTURE_1D, thisIsMyTexture);

	glTexImage1D(
		GL_TEXTURE_1D,
		0,
		colours,
		tempSurface->w,
		0,
		texture_format,
		GL_UNSIGNED_BYTE,
		tempSurface->pixels);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return thisIsMyTexture;
}

GLuint ImageUtility::CreateTexture2D(const char *fileName, GLenum TextureUnit, Window window)
{
	SDL_Surface* tempSurface;
	tempSurface = loadImage(fileName, SDL_GetWindowSurface(window.GetWindow()));
	Uint8 colours = tempSurface->format->BytesPerPixel;
	GLenum texture_format;
	if (colours == 4) {   // alpha
		if (tempSurface->format->Rmask == 0x000000ff)
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	}
	else {             // no alpha
		if (tempSurface->format->Rmask == 0x000000ff)
			texture_format = GL_RGB;
		else
			texture_format = GL_BGR;
	}

	GLuint thisIsMyTexture;
	glActiveTexture(TextureUnit);
	glGenTextures(1, &thisIsMyTexture);
	glBindTexture(GL_TEXTURE_2D, thisIsMyTexture);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		colours,
		tempSurface->w,
		tempSurface->h,
		0,
		texture_format,
		GL_UNSIGNED_BYTE,
		tempSurface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return thisIsMyTexture;
}

GLuint ImageUtility::CreateTexture2D(std::vector<glm::vec4> data, int width, int height, GLenum TextureUnit)
{
	GLuint thisIsMyTexture = -1;
	glActiveTexture(TextureUnit);
	glGenTextures(1, &thisIsMyTexture);
	glBindTexture(GL_TEXTURE_2D, thisIsMyTexture);

	GLfloat* pixels = new GLfloat[16];
	pixels = (GLfloat*)&data[0];

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, &data[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return thisIsMyTexture;
}


SDL_Surface* ImageUtility::loadImage(const char *filename, SDL_Surface* gScreenSurface)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(filename);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", filename, IMG_GetError());
	}
	else
	{
		if (gScreenSurface == nullptr)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", filename, SDL_GetError());
			optimizedSurface = loadedSurface;
		}
		else
		{
			//Convert surface to screen format
			optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, NULL);
			if (optimizedSurface == NULL)
			{
				printf("Unable to optimize image %s! SDL Error: %s\n", filename, SDL_GetError());
			}
		}
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}
