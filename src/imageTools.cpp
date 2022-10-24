#include <iostream>
#include <cmath>

#include "imageTools.h"

using namespace std;

namespace ImageTools {

	SDL_Texture* loadImage(std::string fileName, SDL_Renderer* renderer){

		SDL_Texture *tex = IMG_LoadTexture(renderer, fileName.c_str());

		if (tex == NULL){
			std::cout << SDL_GetError() << std::endl;
		}

		return tex;
	}

	void drawImage(SDL_Texture* img, SDL_Renderer* renderer, int x, int y, int alpha){

		SDL_Rect pos;
		pos.x = x; pos.y = y;

		SDL_QueryTexture(img, NULL, NULL, &pos.w, &pos.h);
		SDL_SetTextureBlendMode(img, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(img, alpha);
		SDL_RenderCopy(renderer, img, NULL, &pos);

	}
	
	void drawRectangle(SDL_Renderer* renderer, SDL_Color color, int x, int y, int w, int h){
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_Rect target;
		target.x = x;
		target.y = y;
		target.w = w;
		target.h = h;
		SDL_RenderFillRect(renderer, &target);
	}

	void drawCircle(SDL_Renderer* renderer, SDL_Color color, int x, int y, int r){

		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

		for (int _y = y - r; _y <= y + r; _y++){
		    int w = sqrt((r * r) - ((_y - y) * (_y - y)) - r);
		    SDL_RenderDrawLine(renderer, x - w, _y, x + w, _y);
		}

	}

	void drawCircleGradient(SDL_Renderer* renderer, SDL_Color outer, SDL_Color inner, int x, int y, int r){

		for (int _y = y - r; _y <= y + r; _y++){
			int w = sqrt(pow(r, 2) - pow(_y - y, 2) - r);
			for (int _x = x - w; _x <= x + w; _x++){
				int d = sqrt(pow(abs(x - _x), 2) + pow(abs(y - _y), 2));
				SDL_Color pointColor = blend(outer, d, inner, r - d);
				SDL_SetRenderDrawColor(renderer,
						               pointColor.r,
						               pointColor.g,
						               pointColor.b,
						               pointColor.a);
				SDL_RenderDrawPoint(renderer, _x, _y);
		    }
		}

	}
	
	SDL_Color blend(SDL_Color c1, double w1, SDL_Color c2, double w2){
		double sumWeights = w1 + w2;
		SDL_Color result = {(int)((w1 * c1.r + w2 * c2.r) / sumWeights),
		                    (int)((w1 * c1.g + w2 * c2.g) / sumWeights),
		                    (int)((w1 * c1.b + w2 * c2.b) / sumWeights),
		                    (int)((w1 * c1.a + w2 * c2.a) / sumWeights)};
		return result;
	}


	void drawText(std::string text, TTF_Font* font, SDL_Color color, int x, int y, SDL_Renderer* renderer){
		//We need to first render to a surface as that's what TTF_RenderText returns, then
		//load that surface into a texture
		SDL_Surface *surf = TTF_RenderText_Blended(font, text.c_str(), color);
		if (surf == NULL){
		}
		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
		if (texture == NULL){
		}

		SDL_Rect pos;
		pos.x = x; pos.y = y;
		SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);

		SDL_RenderCopy(renderer, texture, NULL, &pos);

		//Clean up the surface and font
		SDL_FreeSurface(surf);
	}

}
