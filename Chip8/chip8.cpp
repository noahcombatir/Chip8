#include "chip8.h"
#include <iostream>

unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8::initialize() {
	pc = 0x200; // program counter starts at 0x200
	opcode = 0; // reset current opcode
	I = 0; // reset index register
	sp = 0; // reset stack pointer

	// clear display
	for (int i = 0; i < 2048; i++) {
		gfx[i] = 0;
	}

	// clear registers V0-VF, stack, and keys
	for (int i = 0; i < 16; i++) {
		V[i] = 0;
		stack[i] = 0;
		key[i] = 0;
	}

	// clear memory
	for (int i = 0; i < 4096; i++) {
		memory[i] = 0;
	}

	// load fontset
	for (int i = 0; i < 80; i++) {
		memory[i] = chip8_fontset[i];
	}

	// reset timers
	delay_timer = 0;
	sound_timer = 0;
}

void chip8::emulateCycle() {
	// fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// decode opcode
	switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode & 0x000F) {
				case 0x0000: // 0x00E0: clears the screen
				// execute...
				break;

				case 0x000E: //  0x00EE: returns from subroutine
				// execute...
				break;

				default:
					std::cout << "Unknown opcode [0x0000]: "
						<< std::showbase << std::hex << opcode << std::endl;

			}
		break;


	}

	// update timers
	if (delay_timer > 0) {
		--delay_timer;
	}

	if (sound_timer > 0) {
		if (sound_timer = 1) {
			printf("BEEP!\n");
			--sound_timer;
		}
	}
}

void chip8::loadGame(char* game) {
	// open ROM file
	FILE* rom = fopen(game, "r");
	char buffer[100];

	if (rom == NULL) {
		printf("ERROR: file could not be opened.");
		return;
	}

	// accesses ROM size
	struct stat file_info;
	long rom_size;

	if (stat(game, &file_info) == 0) {
		rom_size = file_info.st_size;
	}
	else {
		std::cerr << "ERROR: File does not exist" << std::endl;
	}

	// allocates memory to store ROM
	char* rom_buffer = (char*) malloc(sizeof(char) * rom_size);

	if (rom_buffer == NULL) {
		std::cerr << "ERROR: Failed to read ROM" << std::endl;
		return;
	}

	// reads ROM file and stores in memory
	size_t result = fread(rom_buffer, sizeof(char), (size_t) rom_size, rom);

	for (int i = 0; i < rom_size; i++) {
		memory[i + 512] = (uint8_t) rom_buffer[i];
	}

	// clean up
	fclose(rom);
	free(rom_buffer);
}

void chip8::setKeys() {

}
