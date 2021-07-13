#include "GameData.h"

GameData::GameData(Charset* chars, Palette* pal)
{
	Chars = new Charset();
	Pal = new Palette();

	for (int i = 0; i < Charset::Size; i++) {
		byte* ch = chars->Get(i);
		Chars->SetChar(i, ch[0], ch[1], ch[2], ch[3], ch[4], ch[5], ch[6], ch[7]);
	}

	for (int i = 0; i < Palette::Size; i++) {
		Color* color = pal->Get(i);
		Pal->Set(i, color->ToInteger());
	}
}

GameData::~GameData()
{
	delete Chars;
	delete Pal;
}
