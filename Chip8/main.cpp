#include "chip8.h"
#include "SDL3/SDL.h"

chip8 myChip8;

int main(int argc, char **argv) {
	// Set up rendering system
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_Window* window = SDL_CreateWindow("Chip 8 Emulator", 640, 320, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

	// initialize Chip8 system and load the game into memory

	return 0;
}