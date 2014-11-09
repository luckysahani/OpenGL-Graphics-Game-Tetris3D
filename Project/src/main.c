#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "base.h"
#include "block.h"
// #include "placer.h"
#include "viewer.h"
#include "glm.h"
#include <stdbool.h>
#include "AL/alut.h"
// #include <conio>
// #include <al/al.h>
#include <AL/alc.h>
// #include <al/alu.h>

#include <stdio.h>
#include <stdlib.h>  
#include <time.h>

#define HEIGHT 800
#define WIDTH 800

Tetris_board *tetris_board;
Block *block[8][8][6]; 
GLfloat ambientLightA[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat diffuseLightA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLfloat ambientLightB[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat diffuseLightB[4] = { 0.8f, 0.8f, 0.8f, 1.0f };

GLfloat specularLightA[4] = { 0.3f, 0.3f, 0.3f, 0.4f };
GLfloat specularLightB[4] = { 0.0f, 0.0f, 0.0f, 0.4f };

GLfloat lightPositionA[4] = { -100.0f, 100.0f, -100.0f, 1.0f };
GLfloat lightPositionB[4] = {  100.0f, 100.0f,  100.0f, 1.0f };

// GLfloat diffuseLightA[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
Viewer *viewer;

int time_status=0,isClicked_right,isClicked_left,flag=1,temp=0;
float height=0.8;				
// int x_temp,y_temp;
int color_block;
int board_status[8][8],view_status[8][8],created_status[8][8];
int music=1,is_ready_to_update_status_of_block=1,executed=1;
Block *current_block;
GLuint texture;
// BlockType current_type;
ALuint buffer, source;
int x[4],y[4],z[4]; 		

void loadSound(char* filename){		
	buffer = alutCreateBufferFromFile(filename);	
	alGenSources(1, &source);		
	alSourcei(source, AL_BUFFER, buffer);		
}			
void playSound(){		
	alSourcePlay(source);		
}


void init() {
/* the tetris_board and the table and the player (viewer)*/
	tetris_board = create_tetris_board();
	viewer = create_viewer((Placeable *)tetris_board);
	int i,j,k;
// itn k=0.0;
	for ( i = 0; i < 8; ++i)
	{
		for ( j = 0; j < 8; ++j)
		{
			board_status[i][j]=0;
			view_status[i][j]=0;
			for ( k = 0; k < 6; k++)
			{
				block[i][j][k] = create_block(ishape, 1);
			}
		}
	}
	tetris_board->score=0;
	glClearColor (0.8, 0.8, 1.0, 1.0);
// glClearColor (0.0f,0.2f,0.2f, 1.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
}

void end() {
	destroy_tetris_board(tetris_board);
	destroy_viewer(viewer);
}
int save_screenshot(char* filename, int w, int h)
{	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	int nSize = w*h*3;
	char* dataBuffer = (char*)malloc(nSize*sizeof(char));
	if (!dataBuffer) return 0;
	glReadPixels((GLint)0, (GLint)0,(GLint)w, (GLint)h,GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);
	FILE *filePtr = fopen(filename, "wb");
	if (!filePtr) return 0;
	unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
	unsigned char header[6] = { w%256,w/256,h%256,h/256,24,0};
	fwrite(TGAheader,	sizeof(unsigned char),	12,	filePtr);
	fwrite(header,	sizeof(unsigned char),	6,	filePtr);
	fwrite(dataBuffer,	sizeof(GLubyte),	nSize,	filePtr);
	fclose(filePtr);
	return 1;
}

void display() {

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// glEnable(GL_TEXTURE_2D);
	// glBindTexture(GL_TEXTURE_2D, texture);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffers
	glLoadIdentity();

	/* lighting */
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLightA);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLightA);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLightA);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPositionA);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLightB);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLightB);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLightB);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPositionB);

	observe_from_viewer(viewer);
	display_tetris_board(tetris_board,board_status,created_status,view_status);
	glFlush();
	glutSwapBuffers();
}

void reshape (int w, int h) {
	glViewport (0, 0, w, h);
	// glViewport (0, 0, 1000, h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (60, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

// //This creates an Square shape Object
// void create_squareshape_block()
// {
// 	while(y_temp==0 && x_temp==0)
// 	{
// 		y_temp=rand()%8;
// 		x_temp=rand()%8;
// 	}
// 	int current_z=board_status[x_temp][y_temp];
// // Block *square=current_block;
// 	printf("SquareShape created at x_temp==%d,y_temp==%d,color=%d,cell value =%d and board_staus=%d\n",x_temp,y_temp,color_block, CELL(x_temp, y_temp,current_z),current_z );
// 	current_type=squareshape;
// 	current_block=set_block(squareshape, color_block,block[x_temp][y_temp][current_z]);
// // current_block=block[x_temp][y_temp][current_z];
// 	// glmScale(current_block->model,0.6);
// 	tetris_board_place_block(tetris_board, current_block, CELL(x_temp, y_temp,current_z),current_z);
// }

// //This creates an Ishape Object
// void create_ishape_block()
// {
// 	while(y_temp>5)
// 	{
// 		y_temp=rand()%8;
// 	}
// 	int current_z=board_status[x_temp][y_temp];
// 	current_type=ishape;
// 	printf("Ishape created at x_temp==%d,y_temp==%d,color=%d,cell value =%d and board_staus=%d\n",x_temp,y_temp,color_block, CELL(x_temp, y_temp,current_z),current_z );
// 	current_block= set_block(ishape, color_block,block[x_temp][y_temp][current_z]);
// // current_block=block[x_temp][y_temp][current_z];
// 	tetris_board_place_block(tetris_board, current_block, CELL(x_temp, y_temp,current_z),current_z);
// }

// //This creates an Cube Object
// void create_cube_block()
// {
// 	while(y_temp>6)
// 	{
// 		y_temp=rand()%8;
// 	}
// 	while(x_temp>6)
// 	{
// 		x_temp=rand()%8;
// 	}
// 	while(y_temp==0 && x_temp==0)
// 	{
// 		y_temp=rand()%8;
// 		x_temp=rand()%8;
// 	}
// 	int current_z=board_status[x_temp][y_temp];
// 	current_type=cube;
// 	printf("Cube created at x_temp==%d,y_temp==%d,color=%d,cell value =%d and board_staus=%d\n",x_temp,y_temp,color_block, CELL(x_temp, y_temp,current_z),current_z );
// 	current_block=set_block(cube, color_block,block[x_temp][y_temp][current_z]);
// 	// glmScale(current_block->model,0.6);
// 	tetris_board_place_block(tetris_board,current_block, CELL(x_temp, y_temp,current_z),current_z);
// }
void check_game_over()
{
	int i;
	for ( i = 0; i < 4; ++i)
	{
		if(board_status[x[i]][y[i]]>=8)
		{
			printf("Game Over\n");
			printf("\n\nYour total score is %d\n",tetris_board->score );
			exit(0);
		}
	}
}
void game_over()
{
	printf("Game Over\n");
	printf("\n\nYour total score is %d\n",tetris_board->score );
	exit(0);
}
bool collision()
{
	int i;
	for ( i = 0; i < 4; ++i)
	{
		if(board_status[x[i]][y[i]]==z[i])
		{
			return true;
		}
	}
	return false;
}
void update_created_status(int k)
{
	int i;
	for ( i = 0; i < 4; ++i)
	{
		created_status[x[i]][y[i]]=k;
	}
}
void create_new_shape(int type,int color_block,BlockType type_block)
{
	int i,current_z;
	if(type==1)
	{
		int temp_x=rand()%6;
		int temp_y=rand()%6;
		
		x[0]=x[1]=temp_x;
		x[2]=x[3]=temp_x+1;
		y[0]=y[2]=temp_y;
		y[1]=y[3]=temp_y+1;
		z[0]=z[1]=z[2]=z[3]=8;
		for ( i = 0; i < 4; ++i)
		{
			current_block=set_block(type_block, color_block,block[x[i]][y[i]][z[i]]);
			tetris_board_place_block(tetris_board,current_block, CELL(x[i], y[i],z[i]),z[i]);
		}
		// for (i = temp_x; i < temp_x+2; ++i)
		// {
		// 	for (j = temp_y; j < temp_y+2; ++j)
		// 	{
		// 		// current_z=board_status[i][j];
		// 		current_block=set_block(type_block, color_block,block[i][j][z[]]);
		// 		tetris_board_place_block(tetris_board,current_block, CELL(i, j,current_z),current_z);
		// 	}
		// }
	}
}
//To move the block by 0.1 units downward
void move_down()
{
	int i,j;
	check_game_over();
	printf("moved down 1 step\n");
	if(executed==1)
	{
		for ( i = 0; i < 4; ++i)
		{
			// current_z=board_status[x[i]][y[i]];
			current_block=tetris_board->board[CELL(x[i], y[i],z[i])];
			// update_created_status(0);
			tetris_board->board[CELL(x[i], y[i],z[i])] = NULL;
			// x_temp++;
			z[i]--;
			// current_block=set_block(type_block, color_block,block[x[i]][y[i]][z[i]]);
			tetris_board_place_block_at_boardvalue(tetris_board,current_block, CELL(x[i], y[i],z[i]),z[i]);
			// for ( j = 0; j < 4; ++j)
			// {
			// 	z[j]--;
			// 	current_block=set_block(type_block, color_block,block[x[i]][y[i]][z[i]]);
			// tetris_board_place_block(tetris_board,current_block, CELL(x[i], y[i],z[i]),z[i]);
			// }
			// update_created_status(1);
			// current_z=board_status[x_temp][y_temp];
				// tetris_board->board[CELL(x_temp,y_temp,current_z)] =current_block;
			// current_block=set_block(current_type, color_block,block[x_temp][y_temp][current_z]);
			// printf("count==%f\n",count );
			// tetris_board_place_block_at_boardvalue(tetris_board, current_block, CELL(x_temp, y_temp,current_z),(int)(count));
			// current_block->pos[1] -= 0.1;
		}
	}

}
void fix_block_at_z()
{
	int i; 
	for ( i = 0; i < 4; ++i)
	{
		// current_z=board_status[x[i]][y[i]];
		current_block=tetris_board->board[CELL(x[i], y[i],z[i])];
		tetris_board_place_block_at_boardvalue(tetris_board,current_block, CELL(x[i], y[i],z[i]),z[i]);
		printf("Block placed at x=%f,y=%f and z=%f\n",current_block->pos[0],current_block->pos[2],current_block->pos[1] );
	}
}
//The main code is implemented here
void update_game()
{
	int type,i;
	check_game_over();
	BlockType type_block=squareshape;
	if(flag==1)
	{ 
		printf("\n\n");
		flag=0;
		// count=height/0.1;
		type=rand()%1+ 1;
		color_block=rand()%3;
		printf("Created the blocks\n");
		create_new_shape(type,color_block,type_block);
		update_created_status(1);
	}
	// else{
	// 	count--;
	// }
	// move_down();
	// if(collision() && ())
	if(collision())
	{
		// current_z=board_status[x_temp][y_temp];
		// set_z_to_zero(tetris_board, current_block, CELL(x_temp,y_temp,current_z),(current_z));
		for ( i = 0; i < 4; ++i)
		{
			if(z[i]==8)
			{
				game_over();
			}
		}
		fix_block_at_z();
		increment_board_status(type); 
		update_created_status(0);
		tetris_board->score+=5;
		printf("\nScore==%d\n",tetris_board->score );
		check_game_over();
		flag=1;
	}
	else
	{
		move_down();
	}
}

//Update board status by 1 i.e now the blocks should come above the already placed block
void increment_board_status(int type)
{
// tetris_board_place_block_at_boardvalue(tetris_board, current_block, CELL(x_temp, y_temp,current_z),temp);
	printf("Updating board status\n");
	int i,j;
	if(type==1)
	{
		for ( i = 0; i < 4; ++i)
		{
			board_status[x[i]][y[i]]=z[i];
		}
	}

	// if(k==1){
	// 	board_status[x_temp][y_temp]++;
	// } 
	// else if(k==2){
	// 	board_status[x_temp][y_temp]=count+1;
	// 	board_status[x_temp][y_temp+1]=count+1;
	// 	board_status[x_temp][y_temp+2]=count+1;
	// }
	// else if(k==3){
	// 	for ( i = 0; i < 2; ++i)
	// 	{
	// 		for ( j = 0; j < 2; ++j)
	// 		{
	// 			board_status[x_temp+i][y_temp+j]=count;
	// 			printf("Updating board status for (%d,%d)\n",x_temp+i,y_temp+j );
	// 			// board_status[x_temp][y_temp]++;break;
	// 		}
	// 	}
	// }
	// view_status[x_temp][y_temp]++;
	// // board_status[x_temp][y_temp]++;
	// int current_z=board_status[x_temp][y_temp];
	printf("board_status with x_temp=%d,y_temp=%d ,ccell value==%d and board_status=%d\n\n",x_temp,y_temp, CELL(x_temp, y_temp,current_z),current_z);

}




// //Place the block at the bottom most possible and then increment the board status
// void place_block()
// {
// 	int current_z=board_status[x_temp][y_temp];
// 	set_z_to_zero(tetris_board, current_block, CELL(x_temp,y_temp,current_z),(current_z));
// 	increment_board_status();
// 	created_status[x_temp][y_temp]=0;
// 	tetris_board->score+=5;
// 	printf("\nScore==%d\n",tetris_board->score );
// 	// showText();
// }



void move_block_max_down()
{
	int current_z=board_status[x_temp][y_temp];
	while(!collision())
	{
		move_down();
	}
	// while(count>current_z)
	// {
	// 	count--;
	// 	move_block_down_by_one_step();
	// 	// current_z=board_status[x_temp][y_temp];
	// }
	// if(count==current_z){
	// 	place_block();
	// }
	// // current_z=board_status[x_temp][y_temp];
	// if(current_z==5){		
	// 	printf("Game over(exited in update game case 2)\n");
	// 	printf("\n\nYour total score is %d\n",tetris_board->score );
	// 	exit(0);
	// }
	flag=1;
}
void move_block_right()
{
	executed=0;
	int current_z=board_status[x_temp][y_temp];
	if(!(x_temp > 6) && (count > 0.0) && (board_status[x_temp+1][y_temp] < (int)(count)) )
	{
		created_status[x_temp][y_temp]=0;
		tetris_board->board[CELL(x_temp,y_temp,current_z)] = NULL;
		x_temp++;
		created_status[x_temp][y_temp]=1;
		current_z=board_status[x_temp][y_temp];
			// tetris_board->board[CELL(x_temp,y_temp,current_z)] =current_block;
		current_block=set_block(current_type, color_block,block[x_temp][y_temp][current_z]);
		printf("count==%f\n",count );
		tetris_board_place_block_at_boardvalue(tetris_board, current_block, CELL(x_temp, y_temp,current_z),(int)(count));
	}
	executed=1;
}
void move_block_left()
{
	executed=0;
	int current_z=board_status[x_temp][y_temp];
	if(!(x_temp < 1) && (count > 0.0) && (board_status[x_temp-1][y_temp] < (int)(count)))
	{
		created_status[x_temp][y_temp]=0;
		tetris_board->board[CELL(x_temp,y_temp,current_z)] = NULL;
		x_temp--;
		created_status[x_temp][y_temp]=1;
		current_z=board_status[x_temp][y_temp];
			// tetris_board->board[CELL(x_temp,y_temp,current_z)] =current_block;
		current_block=set_block(current_type, color_block,block[x_temp][y_temp][current_z]);
			// reset_coordinates(tetris_board, current_block, CELL(x_temp,y_temp,current_z));
		printf("count==%f\n",count );
		tetris_board_place_block_at_boardvalue(tetris_board, current_block, CELL(x_temp, y_temp,current_z),(int)(count));
	}
	executed=1;
}
void move_block_up()
{
	executed=0;
	int current_z=board_status[x_temp][y_temp];
	if(!(y_temp > 6) && (count > 0.0) && (board_status[x_temp][y_temp+1] < (int)(count)))
	{
		created_status[x_temp][y_temp]=0;
		tetris_board->board[CELL(x_temp,y_temp,current_z)] = NULL;
		y_temp++;
		created_status[x_temp][y_temp]=1;
		current_z=board_status[x_temp][y_temp];
			// tetris_board->board[CELL(x_temp,y_temp,current_z)] =current_block;
		current_block=set_block(current_type, color_block,block[x_temp][y_temp][current_z]);
			// reset_coordinates(tetris_board, current_block, CELL(x_temp,y_temp,current_z));
		printf("count==%f\n",count );
		tetris_board_place_block_at_boardvalue(tetris_board, current_block, CELL(x_temp, y_temp,current_z),(int)(count));
	}
	executed=1;
}
void move_block_down()
{
	executed=0;
	int current_z=board_status[x_temp][y_temp];
	if(!(y_temp < 1) && (count > 0.0) && (board_status[x_temp][y_temp-1] < (int)(count)))
	{
		created_status[x_temp][y_temp]=0;
		tetris_board->board[CELL(x_temp,y_temp,current_z)] = NULL;
		y_temp--;
		created_status[x_temp][y_temp]=1;
		current_z=board_status[x_temp][y_temp];
			// tetris_board->board[CELL(x_temp,y_temp,current_z)] =current_block;
		current_block=set_block(current_type, color_block,block[x_temp][y_temp][current_z]);
			// reset_coordinates(tetris_board, current_block, CELL(x_temp,y_temp,current_z));
		printf("count==%f\n",count );
		tetris_board_place_block_at_boardvalue(tetris_board, current_block, CELL(x_temp, y_temp,current_z),(int)(count));
	}
	executed=1;
}




void timer(int extra) {
	glutPostRedisplay();
	while(!executed);
	// while(is_ready_to_update_status_of_block==1)
	// {
	while(time_status>=40)
	{
			// is_ready_to_update_status_of_block=1;
		// tetris_board->score+=1;
		// printf("\nScore==%d\n",tetris_board->score );
		update_game();
			// time_status=0;
			// is_ready_to_update_status_of_block=0;
		time_status=0;
	}
	time_status++;
	// }

	
	glutTimerFunc(1, timer, 0);
	
// glutTimerFunc(200, update_game,0);	
}

void keypressed(unsigned char key, int x, int y) {
	if (key == 's') { viewer->pos[2]+=0.05; }
	if (key == 'w') { viewer->pos[2]-=0.05; }
	if (key == 'a') { viewer->pos[0]-=0.05; }
	if (key == 'd') { viewer->pos[0]+=0.05; }
	if (key == 'b') { viewer->pos[1]+=0.05; }
	if (key == 'n') { viewer->pos[1]-=0.05; }
	if (key == 'z') { save_screenshot("a.tga",WIDTH,HEIGHT); }
	if (key == 'x') 
	{ 
		printf("\n\nYour total score is %d\n",tetris_board->score );
		exit(0);
	}
	if( key == ' ')
	{
		move_block_max_down();
	}
	if (key=='m')
	{
		if(music==0){
			alSourceStop(source);
			music=1;
		}
		else
		{
			loadSound("./wav/theme.wav");
			alSourcePlay(source);
			music=0;
		}
	}

}
void keypressSpecial(int key, int x, int y){
	if (key == GLUT_KEY_UP) {
		loadSound("./wav/tick.wav"); playSound();

		move_block_up();
	}
	if (key== GLUT_KEY_DOWN){
		loadSound("./wav/tick.wav"); playSound();
		move_block_down();
	}
	if (key== GLUT_KEY_LEFT){
		loadSound("./wav/tick.wav"); playSound();
		move_block_left();
	}
	if (key== GLUT_KEY_RIGHT){
		loadSound("./wav/tick.wav"); playSound();
		move_block_right();
	}

}
void mouseMove(int x, int y) 
{ 	
	if (isClicked_left) { 
		viewer->pos[2]-=0.05;
	}
	else if (isClicked_right)
	{
		viewer->pos[2]+=0.05;
	}
}
void mouseButton(int button, int state, int x, int y) 
{
	if (button == GLUT_LEFT_BUTTON) 
	{
		if (state == GLUT_DOWN) { 
			viewer->pos[2]-=0.05;
			isClicked_left=1;
		}
		else  { 
			isClicked_left = 0; 
		}

	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			viewer->pos[2]+=0.05;
			isClicked_right=1;
		}
		else
		{
			isClicked_right=0;
		}

	}
	if ((button == 3) ) 
	{
		if (state == GLUT_UP) return; 
		viewer->pos[0]-=0.05;
	}
	else if(button == 4)
	{
		viewer->pos[0]+=0.05;
	}
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	alutInit (&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize (1200, 1000);
	glutInitWindowPosition (100,100);
	glutCreateWindow ("3D-Tetris");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keypressed);
	glutSpecialFunc(keypressSpecial);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	glutTimerFunc(0,timer,0);

	init();
	glutMainLoop();

	end();

	return 0;
}


