#define _XOPEN_SOURCE_EXTENDED 1

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <locale.h>

WINDOW *mainWindow;

struct Ball {
    int x;
    int y;
    int next_x;
    int next_y;
    bool up;
    bool down;
} ball;

struct Player {
    int x;
    int y;
};

struct Player p1;
struct Player p2;


void init(){
    int max_x = getmaxx(mainWindow);
    int max_y = getmaxy(mainWindow);

    ball.x = max_x / 2;
    ball.y = max_y / 2;
    ball.next_x = 0;
    ball.next_y = 0;
    ball.up = false;
    ball.down = false;

    p1.x = 5;
    p1.y = max_y / 2;

    p2.x = max_x - 6;
    p2.y = max_y / 2;
}


void debug(char title, int x, int integer){
    char buffer[] = {'0', '0', '0'};
    char blank[] = {'0', '0', '0'};

    sprintf(buffer, "%d", integer);
    mvaddch(getmaxy(mainWindow) - 1, x, title);
    mvaddstr(getmaxy(mainWindow) - 1, x + 1, blank);
    mvaddstr(getmaxy(mainWindow) - 1, x + 1, buffer);
}


bool is_collision(){
    bool collision = false;
    int hello = 0;

    if ((ball.down) && (ball.x == 6) && (p1.y-2 <= ball.y <= p1.y+2)){
        hello = 1;
        collision = true;
        debug('C', 20, 1);
    }

    else if ((ball.up) && (ball.x == (getmaxx(mainWindow) - 7)) && (p2.y - 2 <= ball.y <= p2.y + 2)){
        hello = 2;
        collision = true;
        debug('C', 20, 2);
    }

    else if (ball.down && (ball.x == 1 || ball.y == 1)){
        hello = 3;
        collision = true;
        debug('C', 20, 3);
    }

    else if (ball.up && (ball.x == getmaxx(mainWindow) - 1 || ball.y == getmaxy(mainWindow) - 1)){
        hello = 4;
        collision = true;
        debug('C', 20, 4);
    }
    else {
        hello = 5;
    }

    return collision;
}


void draw_ball(){
    if (is_collision()) {
        ball.up = !ball.up;
        ball.down = !ball.down;
    }
    mvaddch(ball.y, ball.x, ' ');
    ball.x = ball.up ? ball.x + 1 : ball.x - 1;
    mvaddch(ball.y, ball.x, 'o');
    debug('X', 40, ball.x);
    debug('Y', 44, ball.y);
}


void draw_player(struct Player *player){
    mvaddch(player->y, player->x, ACS_BLOCK);
    for (int i = 1; i <= 2; i++) {
        if ((player->y + i) < getmaxy(mainWindow)){
            mvaddch(player->y + i, player->x, ACS_BLOCK);
        }
        if ((player->y - i) > 0) {
            mvaddch(player->y - i, player->x, ACS_BLOCK);
        }
    }
    debug('X', 4, player->x);
    debug('Y', 8, player->y);
}


void move_up(struct Player *player){
    if (player->y - 2 != 1) {
        player->y -= 1;
        draw_player(player);
        mvaddch(player->y + 3, player->x, ' ');
    }
}


void move_down(struct Player *player){
    if (player->y + 2 < getmaxy(mainWindow) - 2) {
        player->y += 1;
        draw_player(player);
        mvaddch(player->y - 3, player->x, ' ');
    }
}


int main(void) {

    if ((mainWindow = initscr() ) == NULL ) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    init();
    noecho();
    curs_set(0);
    box(stdscr, ACS_VLINE, ACS_HLINE);

    //    halfdelay(1);
    nodelay(stdscr, TRUE);

    draw_player(&p1);
    draw_player(&p2);

    while (true){

        char exit = getch();
        switch (exit) {
            case 'q':
                move_up(&p1);
                break;
            case 'a':
                move_down(&p1);
                break;
            case 'p':
                move_up(&p2);
                break;
            case 'l':
                move_down(&p2);
                break;
            case 'x':
                delwin(mainWindow);
                endwin();
                return EXIT_SUCCESS;
            default:
                break;
        }

        draw_ball();
        refresh();
    }
}