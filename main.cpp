#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Board.h"
#include "main.h"
#include "Random.h"

#include <iostream>
using namespace std;

//Two constants for use with the "rotate" function
#define LEFT   1
#define RIGHT -1

/***********INPUT*************/
const SDL_Keycode MOVE_LEFT    = 'a';
const SDL_Keycode MOVE_RIGHT   = 'd';
const SDL_Keycode FALL_FASTER  = 's';
const SDL_Keycode ROTATE_LEFT  = 'k';
const SDL_Keycode ROTATE_RIGHT = 'l';
const SDL_Keycode DROP_PIECE   = 'j';
const SDL_Keycode PAUSE_GAME   = 27;
const SDL_Keycode RESET_GAME   = 'r';
/*****************************/

/*******************GLOBAL VARIABLES**************************************/
Random random;
//The indices of both the current and next piece
int piece  = random.GetInteger(0, MAX_PIECE);
int nPiece = random.GetInteger(0, MAX_PIECE);

//Variables to describe the location and orientation of the current piece.
int x_pos = boardW * .4, y_pos = boardH;
int orientation = DEFAULT_ORIENTATION;

//Variables to describe the color of the current and next piece
SDL_Color pColor = randomColor();
SDL_Color nColor = randomColor();

//The initial speed at which pieces fall
int init_speed = 150;
int speed = init_speed;

//Keeps track of the number of lines that have been cleared.
int score = 0;

bool fallfaster = false;
bool collision  = false;
bool gameover   = false;
bool pause      = false;

//Two variables to time actions in the game logic
unsigned int lastTime = 0, currentTime;

/*************************************************************************/

void rotate(int direction, Board *board){
	int orig_orientation = orientation;
	orientation += direction;
	if (orientation < 0) {
		orientation = DEFAULT_ORIENTATION;
	} else if (orientation > 3) {
		orientation = 0;
	}
	if (!board->availableSpace(piece, orientation, { x_pos, y_pos })) {
		orientation = orig_orientation;
	}
}

void next_piece(Board *board, SDL_Point *floor){
	if(collision == false)
		board->setPiece(piece, orientation, *floor, pColor, false);
	else
		board->setPiece(piece, orientation, {x_pos, y_pos}, pColor, false);
	score += board->clearLines();

	//pieces increase speed with more lines
	speed = init_speed - (score/10 * 20);
	if(speed < 50)
		speed = 50;

	y_pos = boardH;
	x_pos = boardW * .4;
	orientation = DEFAULT_ORIENTATION;

	pColor = nColor;
	nColor = randomColor();
	piece  = nPiece;
	nPiece = random.GetInteger(0, MAX_PIECE);
	*floor = board->findFloor(piece, orientation, {x_pos, y_pos});
}

void update(Board *board, SDL_Point *floor, bool force = false){
	//If enough time has passed since the last update and the game isn't over or paused, run update
	if ( (currentTime > lastTime + speed - (fallfaster * speed * .85) && !(gameover || pause)) || force) {
		collision = !board->availableSpace(piece, orientation, { x_pos, y_pos - 1 });
		if (collision){
			next_piece(board, floor);

			if (!board->availableSpace(piece, orientation, { x_pos,y_pos - 2})) {
				gameover = true;
				pause    = false;
			}

			*floor = board->findFloor(piece, orientation, {x_pos, y_pos});
		}
		y_pos -= 1;
		lastTime = currentTime;
	}
}

SDL_Color randomColor(){
	Uint8 r = random.GetInteger(0, 255);
	Uint8 g = random.GetInteger(0, 255);
	Uint8 b = random.GetInteger(0, 255);
	return {r,g,b,255};
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	TTF_Font* font = TTF_OpenFont("OpenSans.ttf", 25);

	SDL_Window *frame = SDL_CreateWindow("Tetris",
			SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
			640, 480, SDL_WINDOW_RESIZABLE);

	SDL_Renderer *painter = SDL_CreateRenderer(frame, -1, SDL_RENDERER_SOFTWARE);

	Board board(painter, frame, {50, 50, 50, 255}, font);

	SDL_Color backgroundColor = {150, 150, 150, 255};
	SDL_Color white = {255, 255, 255, 255};
	SDL_Color black = {0, 0, 0, 255};

	SDL_Point floor = board.findFloor(piece, orientation, {x_pos, y_pos});

	while (true) {
		SDL_Event e;

		if (SDL_PollEvent(&e)) {

			if (e.type == SDL_QUIT) {
				break;
			}

			if (e.type == SDL_KEYDOWN) {
				if(!pause && !gameover){
					switch (e.key.keysym.sym) {
						case (MOVE_LEFT):
							if (board.availableSpace(piece, orientation, { x_pos - 1,y_pos }))
								x_pos -= 1;
							break;
						case (MOVE_RIGHT):
							if (board.availableSpace(piece, orientation, { x_pos + 1,y_pos }))
								x_pos += 1;
							break;
						case (FALL_FASTER):
							fallfaster = true;
							break;
						case (ROTATE_LEFT):
							rotate(LEFT, &board);
							break;
						case (ROTATE_RIGHT):
							rotate(RIGHT, &board);
							break;
						case (DROP_PIECE):
							next_piece(&board, &floor);
							break;
					}
					floor = board.findFloor(piece, orientation, {x_pos, y_pos});
				}
				if(e.key.keysym.sym == PAUSE_GAME && !gameover){
					pause = !pause;
				}
				else if(e.key.keysym.sym == RESET_GAME && gameover){
					gameover = false;
					score = 0;
					for (int x = 0; x < boardW; x++) {
						for (int y = 0; y < boardH; y++) {
							board.playfield[x][y].isEmpty = true;
						}
					}
					piece  = random.GetInteger(0, MAX_PIECE);
					nPiece = random.GetInteger(0, MAX_PIECE);
					speed  = init_speed;
					x_pos  = boardW * .4;
					y_pos  = boardH;
					orientation = DEFAULT_ORIENTATION;
					floor = {x_pos, 0};
				}
			}
			if (e.type == SDL_KEYUP) {
				switch(e.key.keysym.sym) {
					case (FALL_FASTER):
						fallfaster = false;
						break;
					}
			}
		}
		else{
			fallfaster = false;
		}
		SDL_SetRenderDrawColor(painter, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
		SDL_RenderClear(painter);

		//insert a piece onto the board
		board.setPiece(piece, orientation, { x_pos, y_pos }, pColor, false);

		board.drawBoard();
		board.drawPiece(piece, orientation, floor, {board.bgColor.r, board.bgColor.g, board.bgColor.b, board.bgColor.a}, {35,35,35,255});
		board.drawPlayfield();
		board.drawNext(nPiece, nColor);
		board.drawScore(score);
		if (gameover) {
			board.renderText(board.gameboard.x + board.gameboard.w + 37,board.gameboard.y + 165, "GAMEOVER",
					backgroundColor, white);
		}
		else if(pause){
			board.renderText(board.gameboard.x + board.gameboard.w + 65,board.gameboard.y + 165, "PAUSE",
					backgroundColor, white);
		}

		SDL_RenderPresent(painter);

		//remove the piece
		board.setPiece(piece, orientation, { x_pos, y_pos }, black, true);

		//Drop the piece
		currentTime = SDL_GetTicks();

		update(&board, &floor);
	}

	SDL_Quit();
	SDL_DestroyRenderer(painter);
	SDL_DestroyWindow(frame);

	return EXIT_SUCCESS;
}
