#pragma once

#define MAX_PIECE 6
#define DEFAULT_ORIENTATION 3

SDL_Color randomColor();

void rotate(int direction, Board *board);
void next_piece(Board *board, SDL_Point *floor);

void update(Board *board, SDL_Point *floor, bool force);

