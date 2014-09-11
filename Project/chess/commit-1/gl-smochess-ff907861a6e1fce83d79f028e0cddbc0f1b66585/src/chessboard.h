#include <GLUT/glut.h>
#include "pawn.h"

#ifndef CHESSBOARD_h
#define CHESSBOARD_h

#define NUM_CELLS 8

#define R 0
#define G 1
#define B 2

typedef struct {
	GLdouble x; /* placeable */
	GLdouble y;
    GLdouble z;

	GLdouble cell_width;
	GLdouble cell_height;
	int ncells;
	GLdouble color_dark[3];
	GLdouble color_clear[3];
	GLdouble border;

	Pawn **board;
} Chessboard;

Chessboard *create_chessboard();
void destroy_chessboard(Chessboard *cboard);
void display_chessboard(Chessboard *cboard);

void chessboard_place_pawn(Chessboard *cboard, Pawn *pawn, int x, int y);

#endif
