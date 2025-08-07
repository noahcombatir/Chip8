#include <iostream>

class Chip8
{
	public:
		Chip8();
		~Chip8();

		bool drawFlag;

		void init();
		void emulateCycle();
		void loadGame(const char* filepath);

		unsigned char gfx[64 * 32];
		unsigned char key[16];
	private:
		unsigned short opcode;
		unsigned char memory[4096];
		unsigned char V[16];
		unsigned short I;
		unsigned short pc;
		unsigned char delay_timer;
		unsigned char sound_timer;
		unsigned short stack[16];
		unsigned short sp;
};
