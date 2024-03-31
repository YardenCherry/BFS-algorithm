
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <queue>
#include <vector>
#include "Cell.h"
#include <iostream>

using namespace std;
const int MSZ = 100;
const int SPACE = 0;
const int WALL = 1;
const int START = 2;
const int TARGET = 3;
const int GRAY = 4;
const int BLACK = 5;
const int PATH = 6;
const int YELLOW = 7;
const int TPATH = 8;
const int BROWN = 9;
const int MEET = 10;

int maze[MSZ][MSZ] = {0}; // space
bool runBFS = false, run1BiBFS = false, run2BiBFS = false;


queue <Cell*> grays; // queue of gray cells
queue <Cell*> yellows; // queue of yellows cells



void InitMaze();

void init()
{
	glClearColor(0.5,0.5,0.5,0);// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // set the coordinates system

	srand(time(0));

	InitMaze();
}

void InitMaze()
{
	int i, j;

	for (i = 0;i < MSZ;i++)
	{
		maze[0][i] = WALL;
		maze[MSZ-1][i] = WALL;
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
	}

	for(i=1;i<MSZ-1;i++)
		for (j = 1;j < MSZ - 1;j++)
		{
			if (i % 2 == 1) // mostly spaces
			{
				if (rand() % 10 < 2) // WALL
					maze[i][j] = WALL;
				else maze[i][j] = SPACE;
			}
			else // even lines
			{
				if (rand() % 10 < 4) // space
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
		}
	maze[MSZ / 2][MSZ / 2] = START;

	int r = rand() % MSZ;
	int c = rand() % MSZ;
	maze[r][c] = TARGET;
	Cell* tPointer = new Cell(r, c, nullptr);
	Cell* pc = new Cell(MSZ / 2, MSZ / 2,nullptr);  // pointer to a cell
	

	grays.push(pc); // insert first cell to grays
	yellows.push(tPointer); // insert first cell to grays

}

void DrawMaze()
{
	int i, j;

	for(i=0;i<MSZ;i++)
		for (j = 0;j < MSZ;j++)
		{
			switch (maze[i][j]) // set convinient color of a cell
			{
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case WALL:
				glColor3d(0, 0, 0); // black
				break;
			case START:
				glColor3d(0.5, 0.5, 1); // blue
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case GRAY:
				glColor3d(1, 0.7, 0); // orange
				break;
			case BLACK:
				glColor3d(0.8, 1, 0.8); // green
				break;
			case PATH:
				glColor3d(1, 0, 1); // magenta
				break;
			case YELLOW:
				glColor3d(1, 1, 0); //yellow
				break;
			case BROWN:
				glColor3d(0.6, 0.3, 0.15);
				break;

			case MEET:
				glColor3d(1, 0, 1);
				break;
			}// switch

			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i + 1);
			glVertex2d(j+1, i + 1);
			glVertex2d(j+1 , i );
			glEnd();
		}
}

void RestorePath(Cell* pc)
{
	while (pc != nullptr)
	{
		if(maze[pc->getRow()][pc->getCol()] !=START && maze[pc->getRow()][pc->getCol()] != TARGET)
			maze[pc->getRow()][pc->getCol()] = PATH;
		pc = pc->getParent();
	}
}

// BFS a cell on row,col can be either WHITE or TARGET
bool CheckToTargetNeighbour(int row, int col, Cell* sp,Cell* tp)
{
	Cell* pn = nullptr; 
	if (maze[row][col] == MEET || maze[row][col] == PATH) {
		run1BiBFS = false;
		maze[row][col] == PATH;
		RestorePath(sp);
		return false;
	}
	if (maze[row][col] == TARGET || maze[row][col] == YELLOW )
	{
		run1BiBFS = false;
		maze[row][col] = MEET;
		
		RestorePath(sp);
		return false;
	}

	
	

	else // must be "White Neighbour" - SPACE
	{
		pn = new Cell(row, col, sp); // create new Cell
		grays.push(pn);
		maze[row][col] = GRAY;
		return true; // go_on = true
	}
}
bool CheckToStartNeighbour(int row, int col, Cell* sp,Cell*tp)
{
	Cell* pn = nullptr;
	if (maze[row][col] == MEET || maze[row][col] == PATH) {
		run2BiBFS = false;
		maze[row][col] == PATH;
		RestorePath(tp);
		return false;
	}
	if (maze[row][col] == START || maze[row][col] == GRAY)
	{
		maze[row][col] = MEET;
		run2BiBFS = false;
		RestorePath(tp);
		return false;
	}

	
	else // must be "White Neighbour" - SPACE
	{
		pn = new Cell(row, col, tp); // create new Cell
		yellows.push(pn);
		maze[row][col] = YELLOW;
		return true; // go_on = true
	}
}



void RunBiBFSIteration()
{
	Cell* sp;
	Cell* tp;
	int sRow, sCol, tRow,tCol;
	bool s_go_on = true;
	bool t_go_on = true;


	if (grays.empty())
	{
		run1BiBFS = false;
		run2BiBFS = false;
		cout << "Grays are empty. There is no solution\n";
		return;
	}
	if (yellows.empty())
	{
		run1BiBFS = false;
		run2BiBFS = false;
		cout << "yellows are empty. There is no solution\n";
		return;
	}
	else // grays/yellows is not empty
	{
		// pick the first Cell from grays
		sp = grays.front();
		grays.pop();
		tp = yellows.front();
		yellows.pop();
		// paint it black
		if (maze[sp->getRow()][sp->getCol()] != START && maze[sp->getRow()][sp->getCol()] != WALL) maze[sp->getRow()][sp->getCol()] = BLACK;
		if (maze[tp->getRow()][tp->getCol()] != TARGET && maze[tp->getRow()][tp->getCol()] != WALL) maze[tp->getRow()][tp->getCol()] = BROWN;

		sRow = sp->getRow();
		sCol = sp->getCol();

		tRow = tp->getRow();
		tCol = tp->getCol();
		// now check all neighbors of pc and if they are white add them to grays
		// but if one of them is target then stop BFS because target has been found


		

		// ----------------Target to Start---------------
		// 
		if (run2BiBFS) {
			//up
			if (maze[tRow + 1][tCol] == SPACE || maze[tRow + 1][tCol] == GRAY || maze[tRow + 1][tCol] == START || maze[tRow + 1][tCol] == PATH || maze[tRow + 1][tCol] == MEET) {
				t_go_on = CheckToStartNeighbour(tRow + 1, tCol, sp, tp);

			}
			//down
			if (t_go_on && (maze[tRow - 1][tCol] == SPACE || maze[tRow - 1][tCol] == START || maze[tRow - 1][tCol] == GRAY || maze[tRow - 1][tCol] == PATH || maze[tRow - 1][tCol] == MEET )) {
				t_go_on = CheckToStartNeighbour(tRow - 1, tCol, sp, tp);

			}
			// left
			if (t_go_on && (maze[tRow][tCol - 1] == SPACE || maze[tRow][tCol - 1] == START || maze[tRow][tCol - 1] == GRAY || maze[tRow][tCol - 1] == PATH || maze[tRow][tCol - 1] == MEET )) {
				t_go_on = CheckToStartNeighbour(tRow, tCol - 1, sp, tp);

			}

			// right
			if (t_go_on && (maze[tRow][tCol + 1] == SPACE || maze[tRow][tCol + 1] == START || maze[tRow][tCol + 1] == GRAY || maze[tRow][tCol + 1] == PATH || maze[tRow][tCol + 1] == MEET)) {
				t_go_on = CheckToStartNeighbour(tRow, tCol + 1, sp, tp);

			}

		}

		if (run1BiBFS) {
			// up
			if (maze[sRow + 1][sCol] == SPACE || maze[sRow + 1][sCol] == YELLOW || maze[sRow + 1][sCol] == TARGET || maze[sRow + 1][sCol] == PATH || maze[sRow + 1][sCol] == MEET) {
				s_go_on = CheckToTargetNeighbour(sRow + 1, sCol, sp, tp);

			}
			// down
			if (s_go_on && ((maze[sRow - 1][sCol] == SPACE || maze[sRow - 1][sCol] == TARGET || maze[sRow - 1][sCol] == YELLOW || maze[sRow - 1][sCol] == PATH || maze[sRow - 1][sCol] == MEET)))
			{
				s_go_on = CheckToTargetNeighbour(sRow - 1, sCol, sp, tp);

			}
			// left
			if (s_go_on && ((maze[sRow][sCol - 1] == SPACE || maze[sRow][sCol - 1] == TARGET || maze[sRow][sCol - 1] == YELLOW || maze[sRow][sCol - 1] == PATH || maze[sRow][sCol - 1] == MEET))) {
				s_go_on = CheckToTargetNeighbour(sRow, sCol - 1, sp, tp);

			}
			// right
			if (s_go_on && ((maze[sRow][sCol + 1] == SPACE || maze[sRow][sCol + 1] == TARGET || maze[sRow][sCol + 1] == YELLOW || maze[sRow][sCol + 1] == PATH || maze[sRow][sCol + 1] == MEET))) {
				s_go_on = CheckToTargetNeighbour(sRow, sCol + 1, sp, tp);

			}

		}
		
	}
}


// drawings are here
void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer with background color

	DrawMaze();

	glutSwapBuffers(); // show all
}

void idle() 
{

	if (runBFS) {
		//RunBFSIteration();
	}
	if (run1BiBFS||run2BiBFS)
		RunBiBFSIteration();

	glutPostRedisplay(); // call to display indirectly
}

void menu(int choice)
{
	if (choice == 1) // BFS
		runBFS = true;
	if (choice == 2) { // BiBFS
		run1BiBFS = true;
		run2BiBFS = true;
	}
}

void main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(900, 900);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Maze Solver");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle);  // updates function
	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("run BFS", 1);
	glutAddMenuEntry("run BiBFS", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}