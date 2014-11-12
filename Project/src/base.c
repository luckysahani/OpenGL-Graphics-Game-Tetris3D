#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "base.h" 
#include <stdbool.h>

void destroy_tetris_board(Tetris_board *cboard)
{
	free(cboard->board); cboard->board = NULL;
	free(cboard);        cboard = NULL;
}

Tetris_board * create_tetris_board()
{
	Tetris_board *cboard = malloc(sizeof(Tetris_board));
	cboard->pos[0] = 0;
	cboard->pos[1] = 0;
	cboard->pos[2] = 0;

	cboard->cell_height = 1.0f / NUM_CELLS;
	cboard->cell_width  = 1.0f / NUM_CELLS;
	
	/* colors */
	cboard->color_dark[R] = 0.3f; cboard->color_dark[G] = 0.3f;
	cboard->color_dark[B] = 0.4f; cboard->color_dark[A] = 1.0f;

	cboard->color_clear[R] = 1.0f; cboard->color_clear[G] = 1.0f;
	cboard->color_clear[B] = 1.0f; cboard->color_clear[A] = 1.0f;

	cboard->color_selected[R] = 0.0f; cboard->color_selected[G] = 0.0f;
	cboard->color_selected[B] = 1.0f; cboard->color_selected[A] = 0.2f;
	
	cboard->color_specular[R] = 1.0f; cboard->color_specular[G] = 1.0f;
	cboard->color_specular[B] = 1.0f; cboard->color_specular[A] = 1.0f;

	cboard->player_turn = WHITE;

    /* select no cell */
	cboard->cell_highlighted = CELL_NONE;
	cboard->cell_selected =  CELL_NONE;

	/* logical cells for the block */
	cboard->board = malloc(sizeof(Block*) * NUM_CELLS * NUM_CELLS);
	memset(cboard->board, 0, sizeof(Block*) * NUM_CELLS * NUM_CELLS);

	return cboard;
}
void tetris_board_place_block_at_boardvalue(Tetris_board *cboard, Block *p, int cell ,int k)
{
	p->pos[0] = ((GLdouble)CELLX(cell)/NUM_CELLS) - 0.5f + cboard->cell_width/2;
	p->pos[1] = (float)((float)(k)/10);//((double) rand() / (RAND_MAX))/8;
	p->pos[2] = ((GLdouble)(NUM_CELLS-CELLY(cell)-1)/NUM_CELLS) - 0.5f + cboard->cell_height/2;
	// printf("Block placed at x=%f,y=%f and z=%f\n",p->pos[0],p->pos[2],p->pos[1] );

	cboard->board[cell] = p;
}
void tetris_board_place_block(Tetris_board *cboard, Block *p, int cell,int z ) {
	/* invert the position of the pieces along the y-axis */
	p->pos[0] = ((GLdouble)CELLX(cell)/NUM_CELLS) - 0.5f + cboard->cell_width/2;
	p->pos[1] = 0.9;//((double) rand() / (RAND_MAX))/8;
	p->pos[2] = ((GLdouble)(NUM_CELLS-CELLY(cell)-1)/NUM_CELLS) - 0.5f + cboard->cell_height/2;
	// printf("Block placed at x=%f,y=%f and z=%f\n",p->pos[0],p->pos[2],p->pos[1] );
// 
	cboard->board[cell] = p;
}

// void reduce_z_regularly(Tetris_board *cboard, Block *p, int cell)
// {
// 	if(p->pos[1]>0.1)
// 	{
// 		p->pos[1] -= 0.1; 
// 	}
// }
// void set_z_to_zero(Tetris_board *cboard, Block *p, int cell,int k)
// {
// 	float temp=k;
// 	temp=temp/10;
// 	printf("setting p->pos[1] to %d and temp =%f\n",k ,temp);
// 	p->pos[1]=temp;
// 	printf("Block placed at x=%f,y=%f and z=%f\n",p->pos[0],p->pos[2],p->pos[1] );
// }
void reset_coordinates(Tetris_board *cboard, Block *p, int cell)
{
	p->pos[0] = ((GLdouble)CELLX(cell)/NUM_CELLS) - 0.5f + cboard->cell_width/2;
	// p->pos[1] = 0.5;//((double) rand() / (RAND_MAX))/8;
	p->pos[2] = ((GLdouble)(NUM_CELLS-CELLY(cell)-1)/NUM_CELLS) - 0.5f + cboard->cell_height/2;
	// cboard->board[cell] = p;
	printf("Resetting position p->pos[0] to %f and p->pos[2] to %f \n",p->pos[0],p->pos[2]);
	// p->pos[1]=temp;
	// printf("Block placed at x=%f,y=%f and z=%f\n",p->pos[0],p->pos[2],p->pos[1] );
}

Block *get_block(Tetris_board* c, int cell) {
	// printf("Asking for block\n");
	if(c->board[cell])
	{
		printf("Block exists at cell %d\n",cell );
	}
	else
	{
		// printf("Block does not exists at cell %d\n",cell );
	}
	return c->board[cell];
}

void blockdrawer(Tetris_board *cboard,int board_status[8][8],int created_status[8][8],int view_status[8][8][12],int placed_status[8][8][12]) {
	GLdouble x, y;
	// glPushMatrix();		
	// glTranslatef(cboard->pos[0], cboard->pos[1], cboard->pos[2]);
	int color = 0;
	int xcell = -1;
	int ycell;
	int zcell=0;
	int i;
	GLdouble step = cboard->cell_width;
	for (x=-0.5f; x<0.5f; x+=step){
		xcell++;
		ycell = NUM_CELLS;

		color = 1 - color;
		for (y=-0.5f; y<0.5f; y+=step){
			ycell--;
    		/* flip color */
			color = 1 - color;
    		/* choose material color */
			if (cboard->cell_highlighted == CELL(xcell, ycell,zcell)) {
				printf("\n\nshould not enter\n");
				glMaterialfv(GL_FRONT, GL_AMBIENT, cboard->color_selected);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, cboard->color_selected);
				glMaterialfv(GL_FRONT, GL_SPECULAR, cboard->color_specular);
				glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);
			}
			else {
				if (color) { 
					glMaterialfv(GL_FRONT, GL_AMBIENT, cboard->color_dark);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, cboard->color_dark);
					glMaterialfv(GL_FRONT, GL_SPECULAR, cboard->color_specular);
					glMaterialf(GL_FRONT, GL_SHININESS, 120.0f);
				}
				else {
					glMaterialfv(GL_FRONT, GL_AMBIENT, cboard->color_clear);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, cboard->color_clear);
					glMaterialfv(GL_FRONT, GL_SPECULAR, cboard->color_specular);
					glMaterialf(GL_FRONT, GL_SHININESS, 120.0f);
				}
			}
    		/* draw cell */
			// glBegin(GL_QUADS);
			// glNormal3f(0.0,0.0,1.0);
			// glVertex3d(x, 0, y);
			// glVertex3d(x+step, 0, y);
			// glVertex3d(x+step, 0, y+step);
			// glVertex3d(x, 0, y+step);
			// glEnd();
			/* draw block at cell */
			for ( i = 0; i < 10; i++)
			{
				if ((view_status[xcell][ycell][i]==1)) {
					Block *block = cboard->board[CELL(xcell, ycell,i)];
					if(!(cboard->board[CELL(xcell, ycell,i)]))
					{
						printf("Entered ohh really at cell =%d,x=%d,y=%d,z=%d\n",CELL(xcell, ycell,i),xcell,ycell,i);
						printf("view status=%d\n", view_status[xcell][ycell][i]);
						// printf("%s\n", );
						printf("no cell how can this be possible\n");
					}
					else
					{
						view_status[xcell][ycell][i]=1;
						display_block(block, CELL(xcell, ycell,i));
					}
				}
				else
				{
					// if(cboard->board[CELL(xcell, ycell,i)])
					// {
					// 	printf("impossible\n");
					// }
				}
			}
		}
	}
		// printf("\n");
		xcell=-1;
		int check=1;
		int count=0;
		bool check_2=false;
		Block *block;
		for (x=-0.5f; x<0.5f; x+=step)
		{
			xcell++;
			ycell = NUM_CELLS;
			// count=0;
			for (y=-0.5f; y<0.5f; y+=step)
			{
				ycell--;
				// if(board_status[xcell][ycell]>0)
				// {
					if (view_status[xcell][ycell][0]==1)
					{
						count++;
					}
					else
					{
						check=0;
					}
			}	

		}	
		if(check==1)
		{
			printf("\n\nLooks like its fully occupied: Well done dude\n\n");
			cboard->score+=100;
			xcell=-1;
			for (x=-0.5f; x<0.5f; x+=step)
			{
				xcell++;
				ycell = NUM_CELLS;
				for (y=-0.5f; y<0.5f; y+=step)
				{
					ycell--;
					for ( i = 0; i < 10; ++i)
					{
						if(view_status[xcell][ycell][i]==1)
						{
							check_2=true;
						}
					}
					if(check_2==true)
					{
						board_status[xcell][ycell]--;
					}
					for ( i = 0; i < 10; ++i)
					{
						if((view_status[xcell][ycell][i]==1) && (cboard->board[CELL(xcell, ycell,i)]))
						{
							if(i==0)
							{
								// cboard->board[CELL(xcell, ycell,i)]=NULL;
								view_status[xcell][ycell][0]=0;
								// board_status[xcell][ycell]--;
							}
							else
							{
								printf("xcell=%d,ycell=%d,z_old=%d and z_new=%d\n",xcell,ycell,i,(i-1) );
								block=cboard->board[CELL(xcell, ycell,i)];
								view_status[xcell][ycell][i]=0;
								view_status[xcell][ycell][(i-1)]=1;
								tetris_board_place_block_at_boardvalue(cboard,block, CELL(xcell, ycell,(i-1)),(i-1));
								cboard->board[CELL(xcell, ycell,(i-1))]=block;
								printf("Cell_prev=%d,Cell_new =%d,x=%d,y=%d,z=%d\n",CELL(xcell, ycell,i),CELL(xcell, ycell,(i-1)),xcell,ycell,(i-1));
							}
						}
					}
					
				}
			}
			printf("exited\n");
		}
		// glPopMatrix();
}

void drawFloor() {
	glDisable(GL_LIGHTING);
	// glColor3f(0.4, 0.4 , 0.4);
	GLdouble x,y,step;
	step = 0.125;
	for (x=-0.5f; x<0.5f; x+=step){
		for (y = -0.5f; y < 0.5f; y += step){
			glBegin(GL_QUADS);
				glNormal3f(0.0,0.0,1.0);
				glVertex3d(x, 0, y);
				glVertex3d(x+step, 0, y);
				glVertex3d(x+step, 0, y+step);
				glVertex3d(x, 0, y+step);
			glEnd();
		}
	}
	glEnable(GL_LIGHTING);
}
void display_tetris_board(Tetris_board *cboard,int board_status[8][8],int created_status[8][8],int view_status[8][8][12],int placed_status[8][8][12]) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glPushMatrix();
	glColor3f(1.0,1.0,1.0);
	glTranslatef(cboard->pos[0], cboard->pos[1], cboard->pos[2]);

	//----------------------------
	// Reflection Tests going on here
	/* Don't update color or depth. */
      glDisable(GL_DEPTH_TEST);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);

      /* Draw 1 into the stencil buffer. */
      glEnable(GL_STENCIL_TEST);
      glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
      glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

      /* Now render floor; floor pixels just get their stencil set to 1. */
	drawFloor();

	/* Re-enable update of color and depth. */ 
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glEnable(GL_DEPTH_TEST);

      /* Now, only render where stencil is set to 1. */
      glStencilFunc(GL_EQUAL, 1, 0xffffffff);  /* draw if ==1 */
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	//----------------------------
    glPushMatrix();
	    glScalef(1.0, -1.0, 1.0);
	    // glEnable(GL_NORMALIZE);
	    // glCullFace(GL_FRONT);
	    blockdrawer(cboard,board_status,created_status,view_status,placed_status);
	    // glDisable(GL_NORMALIZE);
	    // glCullFace(GL_BACK);
	glPopMatrix();
	glDisable(GL_STENCIL_TEST);

	/* Draw "top" of floor.  Use blending to blend in reflection. */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.7, 0.7, 0.7, 0.7);
    drawFloor();
    glDisable(GL_BLEND);

    /* Draw "bottom" of floor in blue. */
    glFrontFace(GL_CW);  /* Switch face orientation. */
    drawFloor();
    glFrontFace(GL_CCW);
    blockdrawer(cboard,board_status,created_status,view_status,placed_status);
  	glPopMatrix();
  	// glutSwapBuffers();
}

void set_turn(Tetris_board *cboard, PlayerType player) {
	cboard->player_turn = player;
	printf("Player turn changed\n");
}
void flip_turn(Tetris_board *cboard) {
	// cboard->player_turn = 
		// cboard->player_turn == WHITE ? BLACK: WHITE;
	printf("Flipped\n");
}

void tetris_board_clear_cell(Tetris_board *cboard, int cell)
{
	// cboard->board[cell] = NULL;
	printf("Clearing\n");
}
