#include <chrono>
#include <iostream>
#include "SDL.h"

#include "Application.h"

const int SCALE = 20;

int main(int argc, char* args[])
{
	Application application("Chip8 SDL2", SCALE);

	application.run("roms/Space Invaders [David Winter].ch8");

	return 0;
}
