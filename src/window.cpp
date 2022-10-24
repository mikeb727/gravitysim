#include "window.h"

using namespace std;

Window::Window(string n, int width, int height) : name (n), w (width), h (height){

   	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1){
		cerr << SDL_GetError() << endl;
	}
	
	// Initialize window
	win = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
	if (win == NULL){
		cerr << SDL_GetError() << endl;
	}
	else {
		// Initialize renderer
		ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (ren == NULL){
			cerr << SDL_GetError() << endl;
		}
	}
	
	if (TTF_Init() != 0){
		cerr << SDL_GetError() << endl;
	}
	
	font = TTF_OpenFont("images/mainFont.ttf", ImageTools::fontSize);

}

Window::~Window(){
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
	
}

void Window::update(){
    SDL_RenderPresent(ren);
}

SDL_Texture* Window::loadImage(std::string fileName){

	return ImageTools::loadImage(fileName, ren);
}

void Window::drawImage(SDL_Texture* img, int x, int y, int alpha){

	ImageTools::drawImage(img, ren, x, y, alpha);
	
}

void Window::clear(){
	SDL_RenderClear(ren);
}

void Window::drawRectangle(SDL_Color color, int x, int y, int w, int h){
	ImageTools::drawRectangle(ren, color, x, y, w, h);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
}

void Window::drawCircle(SDL_Color color, int x, int y, int r){
	ImageTools::drawCircle(ren, color, x, y, r);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
}

void Window::drawCircleGradient(SDL_Color outer, SDL_Color inner, int x, int y, int r){
	ImageTools::drawCircleGradient(ren, outer, inner, x, y, r);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
}
void Window::drawText(std::string text, SDL_Color color, int x, int y){
	ImageTools::drawText(text, font, color, x, y, ren);
}
