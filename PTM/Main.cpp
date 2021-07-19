#include <SDL.h>
#include <TBRLGPT.h>
#include "Machine.h"
#include "Datafile.h"
#include "EditorBase.h"
#include "CommandShell.h"
using namespace TBRLGPT;

int main(int argc, char** argv)
{
	if (!File::ExistsFolder("files"))
		File::CreateFolder("files");

	const int border = 8;
	const int w = 256 + border;
	const int h = 192 + border;
	const int zoom = 3;

	Graphics* gr = new Graphics(w, h, zoom * w, zoom * h, false);
	gr->SetWindowTitle("PTM");
	Datafile* data = new Datafile();
	EditorBase* sh = new CommandShell(gr, data);
	sh->Run();
	delete sh;
	delete data;

	return 0;
}
