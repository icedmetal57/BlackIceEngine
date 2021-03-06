#include "Engine\Main.h"

void ScreenManager::CleanUp()
{
	while ( !screenStack.empty() )
	{
		RemoveScreen();
	}
}

void ScreenManager::AddScreen( Screen* screen )
{
	screenStack.push( screen );
}

void ScreenManager::RemoveScreen()
{
	Screen* screen = screenStack.top();
	delete screen;
	screenStack.pop();
}

void ScreenManager::Update( int ticks )
{
	if ( !screenStack.empty() )
	{
		Screen* screen = screenStack.top();

		screen->Update( ticks );

		if ( screen->IsFinished() )
		{
			if ( screen->GetNextScreen() != NULL )
			{
				AddScreen( screen->GetNextScreen() );
			}
			else
			{
				do
				{
					RemoveScreen();
				} while ( screenStack.top()->IsDisposable() );
			}
		}
	}
}

void ScreenManager::Render()
{
	if ( !screenStack.empty() )
	{
		Screen* screen = screenStack.top();
		if ( screen != NULL && !screen->IsFinished() )
		{
			screen->Render();
		}
	}
}