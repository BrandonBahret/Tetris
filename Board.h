#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define boardW 10
#define boardH 24

struct Block{
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
		SDL_Color bgColor;
		Block playfield[boardW][boardH+2];
		Board(SDL_Renderer *renderer, SDL_Window *window, SDL_Color backgroundColor, TTF_Font *fnt);
		virtual ~Board();

		bool availableSpace(int piece, int orientation, SDL_Point coord);
		bool inbounds(int piece, int orientation, SDL_Point coord);
		SDL_Point findFloor(int piece, int orientation, SDL_Point coord);

		void drawScore(int score);
		void drawBoard();
		void drawBlock(Block *b, SDL_Color outline);
		void drawPiece(int piece, int orientation, SDL_Point coord, SDL_Color color, SDL_Color outline);
		void drawPlayfield();
		void drawNext(int next, SDL_Color nColor);

		void renderText(int x,int y, const char* message, SDL_Color bg,SDL_Color fg);

		void setBlock(Block b);
		void setPiece(int piece, int orientation, SDL_Point coord, SDL_Color color, bool isEmpty);

		int clearLines();
};
