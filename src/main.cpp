/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <chrono>
#include <iostream>
#include <map>
#include <SDL.h>
#include <stdio.h>

#include "Chip8.h"

//Screen dimension constants
const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
const int SCALE = 20;
const int frameDelay = 1000 / 60;

bool setKeys(unsigned char* key)
{
	// map to assign SDL presses to Chip8 keys
	const std::map<SDL_Keycode, uint8_t> key_map = {
		{SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC},
		{SDLK_q, 0x4}, {SDLK_w, 0x5}, {SDLK_e, 0x6}, {SDLK_r, 0xD},
		{SDLK_a, 0x7}, {SDLK_s, 0x8}, {SDLK_d, 0x9}, {SDLK_f, 0xE},
		{SDLK_z, 0xA}, {SDLK_x, 0x0}, {SDLK_c, 0xB}, {SDLK_v, 0xF}
	};

	bool quit = false;

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT: // set quit to true
			{
				quit = true;
			}
			break;

			case SDL_KEYDOWN: // if key pressed
			{
				auto press = key_map.find(event.key.keysym.sym);

				if (press != key_map.end())
				{
					key[press->second] = 1;
				}
			}
			break;

			case SDL_KEYUP: // if key let go
			{
				auto press = key_map.find(event.key.keysym.sym);

				if (press != key_map.end())
				{
					key[press->second] = 0;
				}
			}
		}
	}

	return quit;
}

int main(int argc, char* args[])
{
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;

	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	//Create window
	window = SDL_CreateWindow("SDL Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	// renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	//Get window surface
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (texture == nullptr)
	{
		std::cout << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	Chip8 chip8;
	chip8.init();
	chip8.loadGame("roms/test_opcode.ch8");

	auto lastCycleTime = std::chrono::high_resolution_clock::now();

	bool quit = false;

	while (!quit)
	{

		quit = setKeys(chip8.key);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > 1)
		{
			lastCycleTime = currentTime;

			chip8.emulateCycle();

			if (chip8.drawFlag)
			{
				uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

				for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
				{
					pixels[i] = chip8.gfx[i] == 0 ? 0x000000FF : 0xFFFFFFFF;
				}

				SDL_UpdateTexture(texture, nullptr, pixels, SCREEN_WIDTH * sizeof(uint32_t));
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, nullptr, nullptr);
				SDL_RenderPresent(renderer);

				chip8.drawFlag = false;
			}
		}
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
