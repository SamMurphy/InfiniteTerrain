#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>

#include <string>

class Window
{
public:
	Window();
	~Window();

	void Create(const std::string &title, int screenWidth, int screenHeight);
	void Swap();

	int screenWidth() { GetScreenSize(); return _screenWidth; }
	int screenHeight(){ GetScreenSize(); return _screenHeight; }

	SDL_Window* GetWindow() { return _window; }
	void SetTitle(const std::string &title);
	
private:
	SDL_Window* _window;
	SDL_GLContext _context;

	int _screenWidth;
	int _screenHeight;
	std::string _title;

	void GetScreenSize();
};

