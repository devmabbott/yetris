#include <Flow/StateManager.hpp>
#include <Flow/GameStateGame.hpp>
#include <Flow/GameStateMainMenu.hpp>
#include <Flow/GameStateFirstTime.hpp>
#include <Misc/Utils.hpp>
#include <Game/Profile.hpp>
#include <Config/Globals.hpp>

StateManager::StateManager():
	currentState(nullptr),
	sharedInfo(0)
{
	// First we'll load the default profile.
	if (! Profile::load())
	{
		// Couldn't find any profiles - first time!

		if (! Globals::Profiles::default_name.empty())
			Globals::Profiles::default_name = "";

		// Let's ask for the user
		this->currentState = new GameStateFirstTime();
		this->currentState->load();
	}
	else
	{
		// Alright, let's load it!
		if (Globals::Profiles::default_name.empty())
			Globals::Profiles::default_name = Profile::profiles.front();

		Globals::Profiles::current = new Profile(Globals::Profiles::default_name);
		Globals::Profiles::current->loadSettings();

		// The first state, Hardcoded
		this->currentState = new GameStateMainMenu();
		this->currentState->load();
	}
}
StateManager::~StateManager()
{
	if (this->currentState)
		this->currentState->unload();

	SAFE_DELETE(this->currentState);
}
void StateManager::run()
{
	bool letsQuit = false;

	while (!letsQuit)
	{
		// Updating the whole state.
		// This value is returned from it tell us if
		// we need to switch from the current state.
		GameState::StateCode whatToDoNow;

		whatToDoNow = this->currentState->update();

		switch (whatToDoNow)
		{
		case GameState::CONTINUE:
			// Just continue on the current state.
			break;

		case GameState::QUIT:
			this->currentState->unload();
			delete this->currentState;
			this->currentState = NULL;

			letsQuit = true;
			break;

		case GameState::GAME_START:
		{
			this->currentState->unload();
			delete this->currentState;

			this->currentState = new GameStateGame();
			this->currentState->load();
			break;
		}

		case GameState::MAIN_MENU:
		{
			this->currentState->unload();
			delete this->currentState;

			this->currentState = new GameStateMainMenu();
			this->currentState->load();
			break;
		}

		default:
			break;
		}

		if (this->currentState)
			this->currentState->draw();
	}

	// Right before quitting...
	Globals::Profiles::current->saveSettings();
}

