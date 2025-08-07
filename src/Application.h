#pragma once
#include <SDL.h>
#include "Chip8.h"

class Application
{
public:
	Application(char const* title, int scale);
	~Application();

	void run(const char* filepath);

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;
	Chip8 chip8;

	bool setKeys(unsigned char* key);
	void update();
};
