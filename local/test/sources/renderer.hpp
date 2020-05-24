#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "string"

struct SDL_Renderer;
struct _TTF_Font;

class Renderer {
public:
	Renderer(SDL_Renderer* pRenderer);
	enum Color {
		BLACK = 0x000000,
		WHITE = 0xffffff,
		RED = 0xff0000,
		GREEN = 0x00ff00,
		BLUE = 0x0000ff,
		YELLOW = 0xffff00,
		MAGENTA = 0xff00ff,
		CYAN = 0x00ffff,
		GREY = 0x808080,
		DARK_RED = 0x800000,
		DARK_GREEN = 0x008000,
		DARK_BLUE = 0x000080,
		DARK_YELLOW = 0x808000,
		DARK_MAGENTA = 0x800080,
		DARK_CYAN = 0x008080
	};

	void SetFont(_TTF_Font* inFont) { font = inFont; };

	void drawRect(float x, float y, float w, float h, Color c);
	void drawText(std::string, int x, int y, Color c);
private:
	SDL_Renderer* m_pRenderer;
	_TTF_Font* font;
};

#endif
