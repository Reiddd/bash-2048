#include <stdlib.h>
#include <stdio.h>
#include <curses.h> // API intr: http://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html
#include <time.h>
#include <unistd.h>
#include <signal.h>

int lattices[4][4] = {0};   // the main interface, 4x4
int empty;                  // the number of empty lattices
int old_x, old_y;

/* S -------------- function declaration -------------- */
void draw();                            // draw the interface
void play();                            // the main body, read in [w a s d q], alter the lattices, and redraw.
void init();                            // initialization
void draw_one (int y     , int x);      // draw single number
void cnt_value(int* new_y, int* new_x); // capsule cnt_one
int  game_over();
int  cnt_one  (int y     , int x);      // generate the place of number
/* E -------------- function declaration -------------- */


/* S -------------- function implementation -------------- */
void init()
{   
    initscr();          // initialize the window
    cbreak();           // characters will be read in immediately(not waiting for user to press Enter)
    noecho();           // won't show user's input on screen
    curs_set(0);        // set curse to invisible(1: normal, 2: highly visible)

    empty = 15;
    srand(time(0));
    int y = rand() % 4;
    int x = rand() % 4;
    lattices[y][x] = 2; // randomly choose a lattice to fullfill a '2'
    draw();
}

void draw()
{
    char c[4] = {'0', '0', '0', '0'};
    clear();                        // clear the whole screen

    for (int r=0; r<9; r+=2) {      // draw horizontal lines
        for (int c=0; c<21; c++) {
            move(r, c); // preorder a curse move operation, it won't move immediately but waits for a refresh()
            addch('-');
            refresh();  // trigger the move operation
        } }

    for (int c=0; c<22; c+=5) {     // draw vertical lines
        for (int r=1; r<8; r++) {
            move(r, c);
            addch('|');
            refresh();
        } }

    for (int y=0; y<4; y++) {       // draw numbers
        for (int x=0; x<4; x++) {
            draw_one(y, x);
        } }
}

void draw_one(int y, int x) 
{
    int  i    = lattices[y][x];
    char c[5] = {0x00};

    int m = 0;
    while (i > 0) {             // build the number str
        int j  = i % 10;
        c[m++] = j + '0';
        i     /= 10;
    }

    m = 0;
    int k = (x + 1)*5 - 1;
    while (m < 5 && c[m] != 0x00) {      // print number
        move(2*y+1, k);         
        addch(c[m++]);
        refresh();
        k--;
    }
}

void play()
{
    while (1) {
        int  move      = 0;             // in case of no move can be done when user gives an instruction
        int  old_empty = empty;         // for later check of whether a move happened.
        char ch        = getchar();     // read in the instruction

        switch (ch) {
            case 'a':
            case 'A':                                                           // left arrow
                for (int y = 0; y < 4; y++) {
                    for (int x = 0; x < 4; ) {
                        if (lattices[y][x] == 0) { // empty lattice
                            x++;
                            continue;
                        } 
                        else {
                            int i;
                            for (i = x + 1; i < 4; i++) {
                                if (lattices[y][i] == 0) {
                                    continue;
                                }
                                else {
                                    if (lattices[y][x] == lattices[y][i]) { // merge two lattices who have the same number
                                        lattices[y][x] += lattices[y][i];
                                        lattices[y][i]  = 0;
                                        empty++;
                                        break;
                                    }
                                    else break;
                                }
                            }
                            x = i;
                        }
                    } }

                for (int y = 0; y < 4; y++) {
                    for (int x = 0; x < 4; x++) {
                        if (lattices[y][x] == 0) {
                            continue;
                        } 
                        else {
                            int i;
                            while (i > 0 && lattices[y][i-1] == 0) {
                                lattices[y][i-1] = lattices[y][i];
                                lattices[y][i]   = 0;
                                move             = 1;
                                i--;
                            }
                        }
                    } }

                break;

            case 'd':
            case 'D':                                                           // right arrow
                for (int y=0; y<4; y++) {
                    for (int x=3; x>=0; ) {
                        if (lattices[y][x] == 0) {
                            x--;
                            continue;
                        }
                        else {
                            int i;
                            for (i=x-1; i>=0; i--) {
                                if (lattices[y][i] == 0){
                                    continue;
                                }
                                else if (lattices[y][x] == lattices[y][i]) {
                                    lattices[y][x] += lattices[y][i];
                                    lattices[y][i]  = 0;
                                    empty++;
                                    break;
                                }
                                else break;
                            }
                            x = i;
                        }
                    } }
                
                for (int y=0; y<4; y++) {
                    for (int x=3; x>=0; x--) {
                        if (lattices[y][x] == 0) {
                            continue;
                        }
                        else {
                            int i = x;
                            while (i < 3 && lattices[y][i+1] == 0) {
                                lattices[y][i+1] = lattices[y][i];
                                lattices[y][i]   = 0;
                                move = 1;
                                i++;
                            }
                        }
                    } }

                break;

            case 'w': 
            case 'W':                                                           // up arrow
                for (int x=0; x<4; x++) {
                    for (int y=0; y<4; ) {
                        if (lattices[y][x] == 0) {
                            y++;
                            continue;
                        }
                        else {
                            int i;
                            for (i=y+1; i<4; i++) {
                                if (lattices[i][x] == 0) {
                                    continue;
                                }
                                else if (lattices[y][x] == lattices[i][x]) {
                                    lattices[y][x] += lattices[i][x];
                                    lattices[i][x]  = 0;
                                    empty++;
                                    break;
                                }
                                else break;
                            }
                            y = i;
                        }
                    } }

                for (int x=0; x<4; x++) {
                    for (int y=0; y<4; y++) {
                        if (lattices[y][x] == 0) {
                            continue;
                        }
                        else {
                            int i = y;
                            while (i > 0 && lattices[i-1][x] == 0) {
                                lattices[i-1][x] = lattices[i][x];
                                lattices[i][x]   = 0;
                                move             = 1;
                                i--;
                            }
                        }
                    } }

                break;
            
            case 's': 
            case 'S':                                                           // down arrow
                for (int x = 0; x < 4; x++) {
                    for (int y = 3; y >= 0; ) {
                        if (lattices[y][x] == 0) {
                            y--;
                            continue;
                        } 
                        else {
                            int i;
                            for (i = y - 1; i >= 0; i--) {
                                if (lattices[i][x] == 0) {
                                    continue;
                                } 
                                else if (lattices[y][x] == lattices[i][x]) {
                                    lattices[y][x] += lattices[i][x];
                                    lattices[i][x]  = 0;
                                    empty++;
                                    break;
                                } 
                                else break;
                            }
                            y = i;
                        }
                    } }
                for (int x = 0; x < 4; x++) {
                    for (int y = 3; y >= 0; y--) {
                        if (lattices[y][x] == 0) {
                            continue;
                        } 
                        else {
                            int i = y;
                            while (i < 3 && lattices[i+1][x] == 0) {
                                lattices[i+1][x] = lattices[i][x];
                                lattices[i][x]   = 0;
                                move             = 1;
                                i++;
                            }
                        }
                    } }

                break;

            case 'q': 
            case 'Q': game_over(); break;                                       // quit
            default : continue   ; break;
        }

        if (empty <= 0) {                           // no empty lattices, lose
            game_over();
        }

        if (empty != old_empty || move == 1) {      // check for a redraw
            int new_x, new_y;
            int temp = 0;
            do {
                new_x = rand() % 4;
                new_y = rand() % 4;
            } while (lattices[new_y][new_x] != 0);

            cnt_value(&new_y, &new_x);

            do {
                temp = rand() % 4;
            } while (temp == 0 || temp == 2);

            lattices[new_y][new_x] = temp + 1;
            empty--;
        }

        draw();
    }
}

int cnt_one(int y, int x)   // find the best place to draw the number
{
    int value = 0;

    if( y - 1 > 0)                lattices[y-1][x]   ? 0 : value++;
    if (y + 1 < 4)                lattices[y+1][x]   ? 0 : value++;
    if (x - 1 >= 0)               lattices[y][x-1]   ? 0 : value++;
    if (x + 1 < 4)                lattices[y][x+1]   ? 0 : value++;
    if (y - 1 >= 0 && x - 1 >= 0) lattices[y-1][x-1] ? 0 : value++;
    if (y - 1 >= 0 && x + 1 < 4)  lattices[y-1][x+1] ? 0 : value++;
    if (y + 1 < 4 && x - 1 >= 0)  lattices[y+1][x-1] ? 0 : value++;
    if( y + 1 < 4 && x + 1 < 4)   lattices[y+1][x+1] ? 0 : value++;

    return value;
}

void cnt_value(int* new_y, int* new_x)  // capsule cnt_one for all lattices
{
    int max_x, max_y, value;
    int max = 0;

    max = cnt_one(*new_y, *new_x);

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (!lattices[y][x]) {
                value = cnt_one(y, x);
                if (value > max && old_y != y && old_x != x) {  
                    *new_y = y;
                    *new_x = x;
                    old_x  = x;
                    old_y  = y;
                    break;
                }
            }
        } }
}

int game_over()
{
    sleep(1);
    endwin();
    exit(0);
}
/* E -------------- function implementation -------------- */

int main() 
{
    init();
    play();
    endwin();   // close the window

    return 0;
}