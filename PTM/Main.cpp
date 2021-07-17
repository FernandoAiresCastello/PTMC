#include <SDL.h>
#include "Program.h"
#include "Machine.h"
#include "Datafile.h"
#include "CommandShell.h"

int main(int argc, char** argv)
{
	const int border = 8;
	const int w = 256 + border;
	const int h = 192 + border;
	const int zoom = 3;

	Graphics* gr = new Graphics(w, h, zoom * w, zoom * h, false);
	gr->SetWindowTitle("PTM");
	Datafile* data = new Datafile();
	CommandShell* sh = new CommandShell(gr, data);
	sh->Run();
	delete sh;
	delete data;

	return 0;
}
