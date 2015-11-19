#include "Board.h"
#include "pieces.h"
#include <iostream>
//Play field is 10 cells wide and at least 22 cells high, with the topmost two hidden.

Board::Board(SDL_Renderer *renderer, SDL_Window *window, TTF_Font *fnt) {
	painter = renderer;
	frame   = window;
	font    = fnt;
	SDL_GetWindowSize(frame, &winW, &winH);
	gameboard = {winW / 3, 0, winW/3, winH};
	blocklength = gameboard.w / boardW ;
}

Board::~Board() {

}

void Board::draw_board(){
	//Draws the grid for the playfield
	SDL_Color orig_color;
	SDL_GetRenderDrawColor(painter, &orig_color.r, &orig_color.g, &orig_color.b, &orig_color.a);

	SDL_SetRenderDrawColor(painter, 50,50,50,255);
	SDL_RenderFillRect(painter, &gameboard);

	SDL_SetRenderDrawColor(painter, 255,255,255,255);
	SDL_RenderDrawRect(painter, &gameboard);

	for(int c=0; c<boardW; c++){
		SDL_RenderDrawLine(painter,
				blocklength * c + gameboard.x, 0,
				blocklength * c + gameboard.x, gameboard.h);
	}

	for(int r=0; r<boardH; r++){
		SDL_RenderDrawLine(painter,
				gameboard.x, blocklength * r,
				gameboard.x + gameboard.w, blocklength * r);
	}

	SDL_SetRenderDrawColor(painter, orig_color.r, orig_color.g, orig_color.b, orig_color.a);
}

void Board::draw_next(int next, SDL_Color nColor){
	SDL_Color orig_color;
	SDL_GetRenderDrawColor(painter, &orig_color.r, &orig_color.g, &orig_color.b, &orig_color.a);

	SDL_Rect container = {gameboard.x + gameboard.w + 50, 50 , blocklength * 5, blocklength * 5};

	SDL_SetRenderDrawColor(painter, 50,50,50,255);
	SDL_RenderFillRect(painter, &container);

	SDL_SetRenderDrawColor(painter, 255,255,255,255);
	SDL_RenderDrawRect(painter, &container);

	for(int i=0; i<4; i++){
		int x = container.x + ((1 + pieces[next][3][i].x) * blocklength) - (blocklength / 2);
		int y = container.y + ((3 - pieces[next][3][i].y) * blocklength);
		SDL_Rect blk = {x, y, blocklength, blocklength};

		SDL_SetRenderDrawColor(painter, nColor.r, nColor.g, nColor.b, nColor.a);
		SDL_RenderFillRect(painter, &blk);
		SDL_SetRenderDrawColor(painter, 0,0,0,255);
		SDL_RenderDrawRect(painter, &blk);
	}

	RenderText(container.x + 17, container.y + 5, "NEXT", {50,50,50,255}, {255,255,255,255});

	SDL_SetRenderDrawColor(painter, orig_color.r, orig_color.g, orig_color.b, orig_color.a);
}

void Board::draw_score(int score){
	char *scr = (char*) malloc(20);
	sprintf(scr, "Lines: %d", score);
	RenderText(gameboard.x + gameboard.w + 52, 10, scr, {150, 150, 150, 255}, {255,255,255,255});
	free(scr);
}

void Board::RenderText(int x,int y, char* message, SDL_Color bg,SDL_Color fg){
	SDL_Texture* text;
	SDL_Surface* surface;
	SDL_Rect textrect;
	surface=TTF_RenderText_Shaded(font,message,fg,bg);
	text=SDL_CreateTextureFromSurface(painter,surface);
	SDL_QueryTexture(text,NULL,NULL,&textrect.w,&textrect.h);
	textrect.x=x;
	textrect.y=y;
	SDL_FreeSurface(surface);
	SDL_SetRenderDrawColor(painter, fg.r, fg.g, fg.b, fg.a);
	SDL_RenderCopy(painter,text,NULL,&textrect);
	SDL_SetRenderDrawColor(painter, bg.r, bg.g, bg.b, bg.a);
	SDL_DestroyTexture(text);
}

void Board::set_block(block b){
	playfield[b.coord.x][b.coord.y] = b;
}

bool Board::available_space(int piece, int orientation, SDL_Point coord){
	int x,y;
	bool available = inbounds(piece, orientation, coord);
	if(available){
		for(int i=0; i<4; i++){
			y = pieces[piece][orientation][i].y + coord.y;
			x = pieces[piece][orientation][i].x + coord.x;

			if(playfield[x][y].isEmpty == false){
				return false;
			}
		}
	}
	return available;
}

bool Board::inbounds(int piece, int orientation, SDL_Point coord){
	int x,y;
	for(int i=0; i<4; i++){
		y = pieces[piece][orientation][i].y + coord.y;
		x = pieces[piece][orientation][i].x + coord.x;
		if( !((x>=0 && x<boardW) && (y>=0)) ){
			return false;
		}
	}
	return true;
}

void Board::set_piece(int piece, int orientation, SDL_Point coord, SDL_Color color, bool isEmpty){
	if(inbounds(piece, orientation, coord)){
		for(int i=0; i<4; i++){
			int x = pieces[piece][orientation][i].x + coord.x;
			int y = pieces[piece][orientation][i].y + coord.y;
			set_block({ {x, y}, color, isEmpty});
		}
	}
}

void Board::draw_block(block *b){
	if(b->isEmpty == false){
		SDL_Color orig_color;
		SDL_GetRenderDrawColor(painter, &orig_color.r, &orig_color.g, &orig_color.b, &orig_color.a);

		int x = b->coord.x;
		int y = b->coord.y;
		SDL_Color color = b->color;

		SDL_Rect blk = {
				gameboard.x + (x * blocklength),
				gameboard.y + (((boardH - 2) - y) * blocklength),
				blocklength, blocklength
		};

		SDL_SetRenderDrawColor(painter, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(painter, &blk);

		SDL_SetRenderDrawColor(painter, 0,0,0,255);
		SDL_RenderDrawRect(painter,&blk);

		SDL_SetRenderDrawColor(painter, orig_color.r, orig_color.g, orig_color.b, orig_color.a);
	}
}

void Board::draw_playfield(){
	for(int x=0; x<boardW; x++){
		for(int y=0; y<boardH; y++){
			draw_block(&playfield[x][y]);
		}
	}
}

int Board::clear_lines(){
	int line_count  = 0;
	int empty_count = 0;
	for(int y=0; y<boardH; y++){
		empty_count = 0;
		for(int x=0; x<boardW; x++){
			empty_count += playfield[x][y].isEmpty;
		}
		if(empty_count == 0){
			line_count++;
			for(int x=0; x<boardW; x++){
				playfield[x][y].isEmpty = true;
			}
			for(int row = y; row<boardH - 1; row++){
				for(int col=0; col<boardW; col++){
					set_block({{col, row}, playfield[col][row+1].color, playfield[col][row+1].isEmpty});
				}
			}
			y=-1;
		}
	}
	return line_count;
}

