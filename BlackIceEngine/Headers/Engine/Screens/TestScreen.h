#pragma once

#include "Engine\GUI\GUI_Button.h"

class TestScreen : public Screen
{
private:
	Texture buttonSelection;
	Texture playButton;
	Texture quitButton;
	Texture background;	
	int selectionIndex;
	static const int BUTTON_COUNT = 2;
	GUI_Skin* guiSkin;
	GUI_Button* testButton;
public:
	TestScreen();
	~TestScreen();
	void Update( int ticks );
	void Render();
	void QuitGame();
};