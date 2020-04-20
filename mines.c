#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h> 
#include <time.h>

#define QUIT   1
#define EMPTY  0
#define MINE  -1
#define REVEAL 2

/* Input function */
short input(const int i) {
    int y, x;
    getyx(stdscr, y, x);

    switch (i) {
        case 'h':
	move(y, --x);
	break;

	case 'j':
	move(++y, x);
	break;

	case 'k':
	move(--y, x);
	break;

	case 'l':
	move(y, ++x);
	break;

	case '\n':
	return REVEAL;	

	case KEY_BACKSPACE:
	return 1;
    }

    return 0;
}

#define top_left(B, X, Y)	((B[X - 1][Y - 1]) == -1 ? 1 : 0)
#define top(B, X, Y)	    	((B[X][Y - 1]) == -1 ? 1 : 0)
#define top_right(B, X, Y)  	((B[X + 1][Y - 1]) == -1 ? 1 : 0)
#define left(B, X, Y)		((B[X - 1][Y]) == -1 ? 1 : 0)
#define right(B, X, Y)		((B[X + 1][Y]) == -1 ? 1 : 0)
#define bottom_left(B, X, Y)	((B[X - 1][Y + 1]) == -1 ? 1 : 0)
#define bottom(B, X, Y)		((B[X][Y + 1]) == -1 ? 1 : 0)
#define bottom_right(B, X, Y)	((B[X + 1][Y + 1]) == -1 ? 1 : 0)

/* Main function */
int main() {

    // Game data ################### //

    const int BOARD_Y = 9;
    const int BOARD_X = 9; 
    const unsigned int mines = 10;
    int unsigned mineCount = 0;

    int i, j;

    // Board, set to EMPTY
    short board[BOARD_X][BOARD_Y];
    for (i = 0; i < BOARD_X; ++i)
	for (j = 0; j < BOARD_Y; ++j) 
	    board[i][j] = EMPTY;

    // Generate mines
    srand(time(NULL));
    while (mines > mineCount) {
	short *boardSpace = &board[rand() % BOARD_X][rand() % BOARD_Y];

	if (*boardSpace == EMPTY) {
	    *boardSpace = MINE;
	    mineCount++;
	} 
	    
    }

    // Create numbers (this is messy)
    for (i = 0; i < BOARD_X; ++i)
	for (j = 0; j < BOARD_Y; ++j) {
	    int adjMines = 0;

	    if (board[i][j] == -1) break;

	    // If search is on top-left
	    // | x = i | y = j |
	    if (i == 0 && j == 0) {
		adjMines += bottom(board, i, j) 
		+ bottom_right(board, i, j) 
		+ right(board, i, j);
	    }

	    // If search is on top
	    else if (j == 0) {
		adjMines += left(board, i, j)
		+ bottom_left(board, i, j)
		+ bottom(board, i, j)
		+ bottom_right(board, i, j)
		+ right(board, i, j);
	    }

	    // If search is on top-right
	    else if (i == BOARD_X - 1 && j == 0) {
		adjMines += left(board, i, j)
		+ bottom_left(board, i, j)
		+ bottom(board, i, j);
	    }

	    // If search is on left
	    else if (i == 0) {
		adjMines += top(board, i, j)			
		+ top_right(board, i, j)
		+ right(board, i, j)
		+ bottom_right(board, i, j)
		+ bottom(board, i, j);
	    }

	    // If search is on right
	    else if (i == BOARD_X - 1) {
		adjMines += top(board, i, j)
		+ top_left(board, i, j)
		+ left(board, i, j)
		+ bottom_left(board, i, j)
		+ bottom(board, i, j);
	    }

	    // If search is on bottom-left
	    else if (i == 0 && j == BOARD_Y - 1) {
		adjMines += top(board, i, j)
		+ top_right(board, i, j)
		+ right(board, i, j);
	    }

	    // If search is on bottom
	    else if (j == BOARD_Y - 1) {
		adjMines += left(board, i, j)
		+ top_left(board, i, j)
		+ top(board, i, j)
		+ top_right(board, i, j)
		+ right(board, i, j);
	    }

	    // If search is on bottom-right
	    else if (i == BOARD_X - 1 && j == BOARD_Y - 1) {
		adjMines += left(board, i, j)
		+ top_left(board, i, j)
		+ top(board, i, j);
	    }

	    // Default
	    else {
		adjMines +=

		// Top
		  top_left(board, i, j)
		+ top(board, i, j)
		+ top_right(board, i, j)

		// Middle
		+ left(board, i, j)
		+ right(board, i, j)

		// Bottom
		+ bottom_left(board, i, j)
		+ bottom(board, i, j)
		+ bottom_right(board, i, j);
	    }

	    // Finally... add value to block
	    board[i][j] = adjMines;

	}

    for (i = 0; i < BOARD_X; ++i) {
	for (j = 0; j < BOARD_Y; ++j)
	    printf("%2d ", board[i][j]);
	printf("\n");
    }

    // ############################# //

    // Initialize ncurses ################## //

    initscr();

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();

    // Colors
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);

    // Redefine coordinates
    // #define STARTY 1
    // #define STARTX 1

    // #define ENDY (LINES - 1)
    // #define ENDX (COLS  - 1)

    #define MIDDLEY (LINES / 2)
    #define MIDDLEX (COLS / 2)

    // ##################################### //

    // Starting info: print ################### //

    // Border
    attron(COLOR_PAIR(2));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(2));

    // Title
    attron(COLOR_PAIR(1));
    mvprintw(0, COLS / 2 - 6, " Minesweeper ");
    attroff(COLOR_PAIR(1));

    // Print spaces, uses coordinates like ncurses (y, x)
    const int start_placey = MIDDLEY - BOARD_Y; 
    const int start_placex = (MIDDLEX - BOARD_X) / 2; 
    const int end_placey = start_placey + BOARD_Y; 
    const int end_placex = (start_placex + BOARD_X) * 2; 

    printw(" Here ==> %d\n", end_placey - start_placey);

    for (i = start_placey; i < end_placey; ++i)
	for (j = start_placex; j < end_placex; j += 2) {
	   mvaddch(i, j, '#');
	}

    move(start_placey, start_placex);
    refresh();

    // ####################################### //
    
    // Preparing quit, then quit //
    short term, y, x;
    while (term != QUIT) {
	term = input(getch());
	switch (term) {
	    case QUIT:
	    endwin();
	    break;

	    case REVEAL:
	    getyx(stdscr, y, x);
	    if (y >= start_placey && y < end_placey && x >= start_placex && x < end_placex) {
		const unsigned int xAdj = x - start_placex + 1, yAdj = y - start_placey;
		if (board[xAdj][yAdj] == -1) {
		    printw("You failed!");
		    term = QUIT;
		    getch();
		    endwin();
		    break;
		} else {
		    addch('0' + board[xAdj][yAdj]);
		}
	    }
	    break;
	}
    }
    
    return 0;

    // ######################### //
}

