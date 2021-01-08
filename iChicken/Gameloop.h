#pragma once
#include "globals.h"
#include "AssetPipeline.h"
#include "Multimedia.h"

namespace Gameloop {
	
	struct GameInput {
		bool up;
		bool down;
		bool left;
		bool right;
		bool jump;   // Genesis B, XBOX X, PS □, SNES/DS/Switch Y, NES A, Keyboard Z
		bool run;    // Genesis C, XBOX A, PS X, SNES/DS/Switch B, NES B, Keyboard X
	};

	class BaseGameLoop {
	public:
		virtual void gameLoop(int deltaMS);
		GameInput (*inputFunction)();
	};

	class FrameCappedLoop : public BaseGameLoop {
	protected:
		int deltaTally = 0;
		int deltaMaximum = 1000 * (1.0f / 60);
	public:
		void gameLoop(int deltaMS);
		void (*gameStep)();
	};

}