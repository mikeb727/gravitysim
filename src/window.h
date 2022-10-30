#ifndef WINDOW_H
#define WINDOW_H

#include "imageTools.h"

#include <iostream>
#include <string>

class Window {

public:

	Window(std::string, int, int);
	
	~Window();
	
	void update();
	void clear();

	SDL_Texture* loadImage(std::string);

	void drawImage(SDL_Texture*, int, int, int);
	
	SDL_Renderer* getRenderer() const {return ren;};

	void drawRectangle(SDL_Color color, int x, int y, int w, int h);
	void drawCircle(SDL_Color, int, int, int);
	void drawCircleGradient(SDL_Color outerColor, SDL_Color innerColor, int xPos, int yPos, int radius);
	void drawText(std::string, SDL_Color, int, int);
	
private:
	SDL_Renderer* ren;
	std::string name;
	int h, w;
	SDL_Window* win;
	TTF_Font* font;

};

#endif
