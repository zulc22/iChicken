#include "globals.h"
#include "AssetPipeline.h"
#include "Multimedia.h"
#include "Gameloop.h"

namespace Gameloop {
	
	void FrameCappedLoop::gameLoop(int deltaMS) {
		deltaTally += deltaMS;
		while (deltaTally > deltaMaximum) {
			gameStep();
			deltaTally -= deltaMaximum;
		}
	}

}