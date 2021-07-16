#include <SDL.h>
#include "Program.h"

int main(int argc, char** argv)
{
	Program* prog = new Program();
	prog->Load("test.ptm");

	delete prog;

	return 0;
}
