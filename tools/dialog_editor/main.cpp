
//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "Base/Surface.h"
#include "Base/EditorLoop.h"
#include <string>
#include <iostream>

int main(int argc, char* args[]) {
	if (argc != 2) {
		std::cout << "Usage: " << args[0] << " <dialog.xml>\n";
		return -1;
	}
	EditorLoop loop;
	loop.start(args[1]);
	return 0;
}
