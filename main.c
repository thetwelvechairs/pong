#define _XOPEN_SOURCE_EXTENDED 1

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <locale.h>

WINDOW *mainWindow;

int rows, columns;
int score_p1 = 0;
int score_p2 = 0;

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
    initscr();

    getmaxyx(stdscr, rows, columns);
    rows--;
    columns--;

    noecho();
    curs_set(0);
    box(stdscr, ACS_VLINE, ACS_HLINE);
    char str[] = "|  Player 1: 00  |  Player 2: 00  |";
    mvaddstr(0, 1, str);

//    halfdelay(1);
    nodelay(stdscr, TRUE);

    ball.x = columns / 2;
    ball.y = rows / 2;
    ball.next_x = 0;
    ball.next_y = 0;
    ball.up = true;
    ball.down = false;

    p1.x = 5;
    p1.y = rows / 2;

    p2.x = columns - 6;
    p2.y = rows / 2;

}


void debug(int x, int integer){
    char buffer[] = {0, 0, 0};
    char blank[] = {0, 0, 0};

    sprintf(buffer, "%d", integer);
    mvaddstr(0, x, blank);
    mvaddstr(0, x, buffer);
}


bool is_collision(){
    bool collision = false;

    if (ball.x == 1 || ball.x == columns - 1){
        if (ball.up){
            if (score_p1 < 99) {
                score_p1 += 1;
            }
            else {
                init();
            }
        }
        else {
            if (score_p2 < 99) {
                score_p2 += 1;
            }
            else {
                init();
            }
        }
        debug(14, score_p1);
        debug(31, score_p2);
        collision = true;
    }
    else if (((ball.up && ball.x == 4) && (ball.y >= p1.y - 2 && ball.y <= p1.y + 2)) ||
             ((ball.down && ball.x == 6) && (ball.y >= p1.y - 2 && ball.y <= p1.y + 2))) {
        collision = true;
    }

    else if (((ball.down && ball.x == (columns - 5)) && (ball.y >= p2.y - 2 && ball.y <= p2.y + 2)) ||
             ((ball.up && ball.x == (columns - 7)) && (ball.y >= p2.y - 2 && ball.y <= p2.y + 2))) {
        collision = true;
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

}


void draw_player(struct Player *player){
    mvaddch(player->y, player->x, ACS_BLOCK);
    for (int i = 1; i <= 2; i++) {
        if ((player->y + i) < rows){
            mvaddch(player->y + i, player->x, ACS_BLOCK);
        }
        if ((player->y - i) > 0) {
            mvaddch(player->y - i, player->x, ACS_BLOCK);
        }
    }
}


void move_up(struct Player *player){
    if (player->y - 2 != 1) {
        player->y -= 1;
        draw_player(player);
        mvaddch(player->y + 3, player->x, ' ');
    }
}


void move_down(struct Player *player){
    if (player->y + 2 < rows - 1) {
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