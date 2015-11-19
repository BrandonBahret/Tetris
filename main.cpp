#include "main.h"
#include "Board.h"

#define LEFT 1
#define RIGHT -1

using namespace std;

void rotate(int *orientation, int direction, Board *board, int piece, int x_pos, int y_pos){
	int orig_orientation = *orientation;
	*orientation += direction;
	if (*orientation < 0) {
		*orientation = 3;
	} else if (*orientation > 3) {
		*orientation = 0;
	}
	if (!board->available_space(piece, *orientation, { x_pos, y_pos })) {
		*orientation = orig_orientation;
	}
}

int main(int argc, char *argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	TTF_Font* font = TTF_OpenFont("OpenSans.ttf", 25);

	SDL_Window *frame = SDL_CreateWindow("Tetris",
			SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
			640, 480, SDL_WINDOW_RESIZABLE);

	SDL_Renderer *painter = SDL_CreateRenderer(frame, -1, SDL_RENDERER_SOFTWARE);

	Board board(painter, frame, font);

	int piece = rand() % 7, next = rand() % 7;

	int y_pos = boardH;
	int x_pos = boardW * .4;
	int orientation = 3;

	SDL_Color pColor = { rand() % 255, rand() % 255, rand() % 255, 255 };
	SDL_Color nColor = { rand() % 255, rand() % 255, rand() % 255, 255 };

	bool collision  = false;
	bool fallfaster = false;
	bool gameover   = false;
	bool pause      = false;

	int score = 0;
	unsigned int lastTime = 0, currentTime;

	while (true) {
		SDL_Event e;

		if (SDL_PollEvent(&e)) {

			if (e.type == SDL_QUIT) {
				break;
			}
			if (e.type == SDL_KEYDOWN) {
				if(!pause){
					switch (e.key.keysym.sym) {
						case ('a'):
							if (board.available_space(piece, orientation, { x_pos - 1,y_pos }))
								x_pos -= 1;
							break;
						case ('d'):
							if (board.available_space(piece, orientation, { x_pos + 1,y_pos }))
								x_pos += 1;
							break;
						case ('s'):

							fallfaster = true;
							break;
						case ('k'):
							rotate(&orientation, LEFT, &board, piece, x_pos, y_pos);
							break;
						case ('l'):
							rotate(&orientation, RIGHT, &board, piece, x_pos, y_pos);
							break;
						case ('r'):
							if (gameover) {
								gameover = false;
								score = 0;
								for (int x = 0; x < boardW; x++) {
									for (int y = 0; y < boardH; y++) {
										board.playfield[x][y].isEmpty = true;
									}
								}
							}
							break;
					}
				}
				if(e.key.keysym.sym == 27 && !gameover){
					pause = !pause;
				}
			}
			if (e.type == SDL_KEYUP) {
				switch(e.key.keysym.sym) {
					case ('s'):
						fallfaster = false;
						break;
					}
			}
		}
		else{
			fallfaster = false;
		}

		SDL_SetRenderDrawColor(painter, 150, 150, 150, 255);
		SDL_RenderClear(painter);

		//insert a piece onto the board
		board.set_piece(piece, orientation, { x_pos, y_pos }, pColor, false);

		board.draw_board();
		board.draw_playfield();
		board.draw_next(next, nColor);
		board.draw_score(score);
		if (gameover) {
			board.RenderText(board.gameboard.x + board.gameboard.w + 37,board.gameboard.y + 165, "GAMEOVER",
					{ 150, 150, 150,255 }, { 255, 255, 255, 255 });
		}
		else if(pause){
			board.RenderText(board.gameboard.x + board.gameboard.w + 65,board.gameboard.y + 165, "PAUSE",
					{ 150, 150, 150,255 }, { 255, 255, 255, 255 });
		}

		SDL_RenderPresent(painter);

		//remove the piece
		board.set_piece(piece, orientation, { x_pos, y_pos }, { 0, 0, 0, 255 }, true);

		//Drop the piece
		currentTime = SDL_GetTicks();

		if (currentTime > lastTime + 185 - (fallfaster * 157) && !(gameover || pause)) {
			collision = !board.available_space(piece, orientation, { x_pos, y_pos - 1 });
			if (collision){
				board.set_piece(piece, orientation, { x_pos, y_pos }, pColor, false);
				score += board.clear_lines();
				y_pos = boardH;
				x_pos = boardW * .4;
				orientation = 3;
				pColor = nColor;
				nColor = {rand()%255,rand()%255,rand()%255, 255};
				piece = next;
				next = rand() % 7;
				if (!board.available_space(piece, orientation, { x_pos,y_pos - 2})) {
					gameover = true;
					pause = false;
				}
			}
			y_pos -= 1;
			lastTime = currentTime;
		}
	}

	SDL_Quit();
	SDL_DestroyRenderer(painter);
	SDL_DestroyWindow(frame);

	return EXIT_SUCCESS;
}
