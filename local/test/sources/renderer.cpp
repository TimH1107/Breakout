#include "renderer.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

Renderer::Renderer(SDL_Renderer* pRenderer) {
	m_pRenderer = pRenderer;
	font = nullptr;
}

void Renderer::drawRect(float x, float y, float w, float h, Color c) {
	SDL_SetRenderDrawColor( m_pRenderer, ( Uint8 )( ( c >> 16 ) & 255 ), ( Uint8 )( ( c >> 8 ) & 255 ), ( Uint8 )( c & 255), ( Uint8 )255 );
	SDL_Rect rect;
	rect.x = ( int )x;
	rect.y = ( int )y;
	rect.w = ( int )w;
	rect.h = ( int )h;
	SDL_RenderFillRect(m_pRenderer, &rect);
}

void Renderer::drawText(std::string inText, int x, int y, Color c)
{
	if (font == nullptr) return;
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, inText.c_str(), { (Uint8)((c >> 16) & 255), (Uint8)((c >> 8) & 255), (Uint8)(c & 255), (Uint8)255 });
	SDL_Texture* text = SDL_CreateTextureFromSurface(m_pRenderer, textSurface);
	SDL_Rect textRect;
	textRect.x = x;
	textRect.y = y;
	SDL_QueryTexture(text, NULL, NULL, &textRect.w, &textRect.h);
	SDL_FreeSurface(textSurface);
	textSurface = nullptr;
	SDL_RenderCopy(m_pRenderer, text, NULL, &textRect);
	SDL_DestroyTexture(text);
}