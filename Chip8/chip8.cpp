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

void chip8::init() {
	pc = 0x200; // program counter starts at 0x200
	opcode = 0; // reset current opcode
	I = 0; // reset index register
	sp = 0; // reset stack pointer

	// clear display
	for (int i = 0; i < 2048; i++)
	{
		gfx[i] = 0;
	}

	// clear registers V0-VF, stack, and keys
	for (int i = 0; i < 16; i++)
	{
		V[i] = 0;
		stack[i] = 0;
		key[i] = 0;
	}

	// clear memory
	for (int i = 0; i < 4096; i++)
	{
		memory[i] = 0;
	}

	// load fontset
	for (int i = 0; i < 80; i++) 
	{
		memory[i] = chip8_fontset[i];
	}

	// reset timers
	delay_timer = 0;
	sound_timer = 0;

	// set drawFlag
	drawFlag = true;
}

void chip8::emulateCycle() {
	// fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// decode opcode
	switch (opcode & 0xF000)
	{
		case 0x0000:
		{
			switch (opcode & 0x000F)
			{
				case 0x0000: // 0x00E0: clears the screen
				{
					for (int i = 0; i < 2048; i++)
					{
						gfx[i] = 0x0;
					}
					drawFlag = true;
					pc += 2;
				}
				break;

				case 0x000E: //  0x00EE: returns from subroutine
				{
					sp--;
					pc = stack[sp];
					pc += 2;
				}
				break;

				default:
					std::cout << "Unknown opcode [0x0000]: "
						<< std::showbase << std::hex << opcode << std::endl;
			}
		}
		break;

		case 0x1000: // 1NNN: jump to address NNN
		{
			pc = opcode & 0x0FFF;
		}
		break;

		case 0x2000: // 2NNN: call subroutine at NNN
		{
			stack[sp] = pc;
			sp++;
			pc = opcode & 0x0FFF;
		}
		break;

		case 0x3000: // 3XNN: skip the following instruction if VX equals NN
		{
			if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
		}
		break;

		case 0x4000: // 4XNN: skip the following instruction if VX is not equal to NN
		{
			if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
		}
		break;

		case 0x5000: // 5XY0: skip the following instruction if VX is equal to the register of VY
		{
			if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
		}
		break;

		case 0x6000: // 6XNN: store number NN in register VX
		{
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			pc += 2;
		}
		break;

		case 0x7000: // 7XNN: add value NN to register VX
		{
			V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			pc += 2;
		}
		break;

		case 0x8000:
		{
			switch (opcode & 0x000F)
			{
				case 0x0001: // 8XY1: bitwise OR
				{
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;

				case 0x0002: // 8XY2: bitwise AND
				{
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;

				case 0x0003: // 8XY3: bitwise XOR
				{
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;

				case 0x0004: // 8XY4: add VY to VX, set VF if carry occurs
				{
					if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
					{
						V[0xF] = 1;
					}
					else
					{
						V[0xF] = 0;
					}

					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;

				case 0x0005: // 8XY5: subtract VY from VX, set VF if borrow occurs
				{
					if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
					{
						V[0xF] = 0;
					}
					else
					{
						V[0xF] = 1;
					}

					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;

				case 0x0006: // 8XY6: store VY >> 1 in VX, set VF to LEAST significant bit
				{
					V[0xF] = V[(opcode & 0x00F0) >> 4] & 0x1;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] >> 1;
					pc += 2;
				}
				break;

				case 0x0007: // 8XY7: set VX to VY - VX, set VF if borrow occurs
				{
					if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
					{
						V[0xF] = 0;
					}
					else
					{
						V[0xF] = 1;
					}

					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;

				case 0x000E: // 8XYE: store VY << 1 in VX, set VF to MOST significant bit
				{
					V[0xF] = V[(opcode & 0x00F0) >> 4] >> 7;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) > 4] << 1;
					pc += 2;
				}
				break;
			}
		}
		break;

		case 0x9000: // 9XY0: skip instruction if VX is not equal to VY
		{
			if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			{
				pc += 4;
			}
			else
			{
				pc += 2;
			}
		}
		break;

		case 0xA000: // ANNN: store memory address NNN in register I
		{
			I = opcode & 0x0FFF;
			pc += 2;
		}
		break;

		case 0xB000: // BNNN: jump to address NNN + V0
		{
			pc = (opcode & 0x0FFF) + V[0];
		}
		break;
		
		case 0xC000: // CXNN: set VX to a random number with a mask of NN
		{
			V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
			pc += 2;
		}
		break;

		case 0xD000: // DXYN: draw a sprite at (VX, VY) with a width of 8 and height of N bytes of data starting
		{			//  at address in I set VF to 1 if any pixels are changed to unset, and 00 otherwise
			// position and height of sprite
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;

			unsigned short pixel;

			for (int i = 0; i < height; i++)
			{
				// pixel data
				pixel = memory[I + i];

				for (int j = 0; j < 8; j++)
				{
					// checks if current pixel is 1 (scans through byte one bit at a time)
					if (pixel & (0x80 >> j) != 0)
					{
						// checks if pixel on display is set to 1
						if (gfx[x + j + ((y + i) * 64)] == 1)
						{
							V[0xF] = 1;
						}

						// set pixel value using XOR
						gfx[x + j + ((y + i) * 64)] ^= 1;
					}
				}
			}

			drawFlag = true;
			pc += 2;
		}
		break;

		case 0xE000:
		{
			switch (opcode & 0x000F)
			{
				case 0x000E: // EX9E: skip instruction if key of value in VX is pressed
				{
					if (key[V[(opcode & 0x0F00) >> 8]] != 0)
					{
						pc += 4;
					}
					else
					{
						pc += 2;
					}
				}
				break;

				case 0x0001: // EXA1: skip instruction if key of value in VX is NOT pressed
				{
					if (key[V[(opcode & 0x0F00) >> 8]] == 0)
					{
						pc += 4;
					}
					else
					{
						pc += 2;
					}
				}
				break;
			}
		}
		break;

		case 0xF000:
		{
			switch (opcode & 0x00FF)
			{
				case 0x0007: // FX07: store value of delay timer in register VX
				{
					V[(opcode & 0x0F00) >> 8] = delay_timer;
					pc += 2;
				}
				break;

				case 0x000A: // FX0A: wait for keypress and store result in register VX
				{
					bool keyPress = false;

					for (int i = 0; i < 16; i++)
					{
						if (key[i] != 0)
						{
							V[(opcode & 0x0F00) >> 8] = i;
							keyPress = true;
						}
					}

					// no key pressed, repeat cycle and try again
					if (!keyPress)
					{
						return;
					}

					pc += 2;
				}
				break;

				case 0x0015: // FX15: set delay timer to VX
				{
					delay_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;

				case 0x0018: // FX18: set sound timer to VX
				{
					sound_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;

				case 0x001E: // FX1E: add VX to I, set VF if range overflow
				{
					if (I + V[(opcode & 0x0F00) >> 8] < 0xFFF)
					{
						V[0xF] = 1;
					}
					else
					{
						V[0xF] = 0;
					}

					I += V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;

				case 0x0029: // FX29: set I to memory address of sprite data stored in VX
				{
					I = V[(opcode & 0x0F00) >> 8] * 0x5;
					pc += 2;
				}
				break;

				case 0x0033: // FX33: store binary-coded decimal equivalent of VX at memory addresses I, I + 1, and I + 2
				{
					memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
					memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
					memory[I + 2] = (V[(opcode & 0x0F00) >> 8] / 100) % 10;
					pc += 2;
				}
				break;

				case 0x0055: // FX55: stores values of registers V0 to VX in memory starting at address I
				{
					unsigned short x = V[(opcode & 0x0F00) >> 8];

					for (int i = 0; i <= x; i++)
					{
						memory[I + i] = V[i];
					}

					I += x + 1;
					pc += 2;
				}
				break;

				case 0x0065: // FX65: fills registers V0 to VX with the values stored in memory at address I
				{
					unsigned short vx = V[(opcode & 0x0F00) >> 8];

					for (int i = 0; i <= vx; i++)
					{
						V[i] = memory[I + i];
					}

					I += vx + 1;
					pc += 2;
				}
				break;
			}
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
		}
		--sound_timer;
	}
}

void chip8::loadGame(char* filepath) {
	// open ROM file
	FILE* rom;
	errno_t err;

	if ((err = fopen_s(&rom, filepath, "r")) != 0)
	{
		std::cout << "ERROR! Cannot open file " << filepath << std::endl;
		return;
	}

	if (rom == NULL)
	{
		printf("ERROR: file could not be opened.");
		return;
	}

	// accesses ROM size
	struct stat file_info;
	long rom_size;

	if (stat(filepath, &file_info) == 0)
	{
		rom_size = file_info.st_size;
	}
	else
	{
		std::cerr << "ERROR: File does not exist" << std::endl;
	}

	// allocates memory to store ROM
	char* rom_buffer = (char*) malloc(sizeof(char) * rom_size);

	if (rom_buffer == NULL)
	{
		std::cerr << "ERROR: Failed to read ROM" << std::endl;
		return;
	}

	// reads ROM file and stores in memory
	size_t result = fread(rom_buffer, sizeof(char), (size_t) rom_size, rom);

	for (int i = 0; i < rom_size; i++)
	{
		memory[i + 512] = (uint8_t) rom_buffer[i];
	}

	// clean up
	fclose(rom);
	free(rom_buffer);
}

void chip8::setKeys() {

}