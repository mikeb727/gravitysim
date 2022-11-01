#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

#include "imageTools.h"

namespace ImageTools {

	SDL_Texture* loadImage(std::string fileName, SDL_Renderer* renderer){

		SDL_Texture *tex = IMG_LoadTexture(renderer, fileName.c_str());

		if (tex == NULL){
			std::cout << SDL_GetError() << "\n";
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

	SDL_Color randomColor(){
		std::default_random_engine generator;
		generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_real_distribution<double> hueDist(0, 360);
		std::normal_distribution<double> satDist(0.8, 0.2);
		std::normal_distribution<double> valDist(0.5, 0.2);
		double hue = hueDist(generator);
		double sat = satDist(generator);
		double val = valDist(generator);
		std::cerr << "[imageTools] hue " << hue << " sat " << sat << " val " << val << "\n";
		rgb intermediate = hsv2rgb(ImageTools::hsv{hue,sat,val});
		return SDL_Color {(int)(intermediate.r * 255),
						  (int)(intermediate.g * 255),
						  (int)(intermediate.b * 255),
						  255};
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

	// see https://stackoverflow.com/questions/3018313/
	rgb hsv2rgb(hsv in)
	{
		double      hh, p, q, t, ff;
		long        i;
		rgb         out;

		if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
			out.r = in.v;
			out.g = in.v;
			out.b = in.v;
			return out;
		}
		hh = in.h;
		if(hh >= 360.0) hh = 0.0;
		hh /= 60.0;
		i = (long)hh;
		ff = hh - i;
		p = in.v * (1.0 - in.s);
		q = in.v * (1.0 - (in.s * ff));
		t = in.v * (1.0 - (in.s * (1.0 - ff)));

		switch(i) {
		case 0:
			out.r = in.v;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = in.v;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = in.v;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = in.v;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = in.v;
			break;
		case 5:
		default:
			out.r = in.v;
			out.g = p;
			out.b = q;
			break;
		}
		return out;     
	}

}
