/* Functions for using images in SDL. Includes
    the SDL headers, so they don't have to be
    included in main. */

#ifndef IMAGES_H
#define IMAGES_H

#include <string>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

namespace Colors {
	const SDL_Color black = {0, 0, 0, 255};
	const SDL_Color white = {255, 255, 255, 255};
	const SDL_Color red = {255, 0, 0, 255};
	const SDL_Color blue = {0, 0, 255, 255};
}

namespace ImageTools {

	const int fontSize = 18;

	/* Returns a texture containing the image from the file specified. */
	SDL_Texture* loadImage(std::string, SDL_Renderer*);

	/* Applies the image (texture) specified to the renderer (window) specified at the position given. */
	void drawImage(SDL_Texture*, SDL_Renderer*, int, int, int);
	
	
	void drawRectangle(SDL_Renderer*, SDL_Color, int, int, int, int);

	/* Applies a circle with the given color and diameter to the renderer with the given center. */
	void drawCircle(SDL_Renderer*, SDL_Color, int, int, int);

	void drawCircleGradient(SDL_Renderer*, SDL_Color, SDL_Color, int, int, int);

	/* Applies a text object with the font and color given to the renderer specified at the position given. */
	void drawText(std::string, TTF_Font*, SDL_Color, int, int, SDL_Renderer*);
	
	SDL_Color blend(SDL_Color, double, SDL_Color, double);

}

#endif
