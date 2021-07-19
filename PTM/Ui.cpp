#include "Ui.h"

#define CHAR_H 196
#define CHAR_V 179
#define CHAR_TL 218
#define CHAR_TR 191
#define CHAR_BL 192
#define CHAR_BR 217

void Ui::DrawWindow(Datafile* data, Graphics* gr, int x, int y, int w, int h, int fgcix, int bgcix)
{
	const int fgc = data->GetPalette()->Get(fgcix)->ToInteger();
	const int bgc = data->GetPalette()->Get(bgcix)->ToInteger();

	gr->PutChar(data->GetCharset(), CHAR_TL, x + 0, y + 0, fgc, bgc);
	gr->PutChar(data->GetCharset(), CHAR_TR, x + w + 1, y + 0, fgc, bgc);
	gr->PutChar(data->GetCharset(), CHAR_BL, x + 0, y + h + 1, fgc, bgc);
	gr->PutChar(data->GetCharset(), CHAR_BR, x + w + 1, y + h + 1, fgc, bgc);

	for (int px = 1; px <= w; px++) {
		gr->PutChar(data->GetCharset(), CHAR_H, x + px, y + 0, fgc, bgc);
		gr->PutChar(data->GetCharset(), CHAR_H, x + px, y + h + 1, fgc, bgc);
	}
	for (int py = 1; py <= h; py++) {
		gr->PutChar(data->GetCharset(), CHAR_V, x + 0, y + py, fgc, bgc);
		gr->PutChar(data->GetCharset(), CHAR_V, x + w + 1, y + py, fgc, bgc);
	}
}
