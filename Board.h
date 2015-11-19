#ifndef BOARD_H_
#define BOARD_H_

#include "main.h"

#define boardW 10
#define boardH 24

struct block{
	SDL_Point coord;
	SDL_Color color = {0,0,0,255};
	bool isEmpty = true;
};

class Board {
	private:
		SDL_Renderer *painter;
		SDL_Window *frame;
		TTF_Font* font;
		float blocklength;
		int winW, winH;

	public:
		SDL_Rect gameboard;
		block playfield[boardW][boardH+1];
		Board(SDL_Renderer *renderer, SDL_Window *window, TTF_Font *fnt);
		virtual ~Board();

		bool available_space(int piece, int orientation, SDL_Point coord);
		bool inbounds(int piece, int orientation, SDL_Point coord);

		void draw_score(int score);
		void draw_board();
		void draw_block(block *b);
		void draw_playfield();
		void draw_next(int next, SDL_Color nColor);

		void RenderText(int x,int y, char* message, SDL_Color bg,SDL_Color fg);

		void set_block(block b);
		void set_piece(int piece, int orientation, SDL_Point coord, SDL_Color color, bool isEmpty);

		int clear_lines();
};

#endif /* BOARD_H_ */
