#include "savestate.h"

#include <Commctrl.h>

#include <stdio.h>
#include <string>

static auto hStatusWnd = (HWND*)0x4D81e0;
static auto CurrentSave = (char*)0x4D8080;
static auto RomName = (char*)0x4af1f8;

void SetCurrentSaveState(int state)
{
	if (strlen(RomName) == 0) { return; }

	sprintf_s(CurrentSave, 256, "%s.pj%d", RomName, state);
	auto string = std::string("LINK's state: ") + CurrentSave;

	auto hwnd = *hStatusWnd;
	SendMessage(*hStatusWnd, SB_SETTEXT, 0, (LPARAM)string.c_str());
}

void Savestate::handleKey(WPARAM wParam, LPARAM lParam)
{
    SetCurrentSaveState(wParam);
}
