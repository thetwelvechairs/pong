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
    bool right;
    bool left;
    bool up;
    bool down;
    bool spin;
} ball;

struct Player {
    int x;
    int y;
};

struct Player p1;
struct Player p2;


void init(){
    getmaxyx(stdscr, rows, columns);
    rows--;
    columns--;

    noecho();
    curs_set(0);
    box(stdscr, ACS_VLINE, ACS_HLINE);
    attrset(A_BOLD);
    char str[] = "   Player 1: 0  |  Player 2: 0   ";
    mvaddstr(0, 3, str);
    attroff(A_BOLD);
    char footer[] = "   E[x]it   ";
    mvaddstr(rows, columns-sizeof(footer), footer);
//    halfdelay(1);
    nodelay(stdscr, TRUE);

    ball.x = columns / 2;
    ball.y = rows / 2;
    ball.right = false;
    ball.left = false;
    ball.up = false;
    ball.down = false;
    ball.spin = false;

    p1.x = 6;
    p1.y = rows / 2;

    p2.x = columns - 6;
    p2.y = rows / 2;

}


void debug(int x, int integer){
    char buffer[] = {0, 0};
    char blank[] = {0, 0};

    sprintf(buffer, "%d", integer);
    attrset(A_BOLD);
    mvaddstr(0, x, blank);
    mvaddstr(0, x, buffer);
    attroff(A_BOLD);
}


void is_collision(){
    // X-axis border / goal
    if (ball.x == 1 || ball.x == columns - 1){
        if (ball.right){
            if (score_p1 < 9) {
                score_p1 += 1;
            }
        }
        else {
            if (score_p2 < 9) {
                score_p2 += 1;
            }
        }
        beep();
        debug(16, score_p1);
        debug(32, score_p2);

        ball.right = !ball.right;
        ball.left = !ball.left;
    }

    // Y-axis border
    else if (ball.y == 1 || ball.y == rows - 1) {
        ball.up = !ball.up;
        ball.down = !ball.down;
    }

    // Bat corner P2
    else if (((ball.right && ball.x == 5) || (ball.left && ball.x == 7))
             && (ball.y >= p1.y - 2 && ball.y <= p1.y + 2)) {
        ball.right = !ball.right;
        ball.left = !ball.left;
        ball.up = !ball.up;
        ball.down = !ball.down;
        ball.spin = true;
    }

    // Bat middle P2
    else if (((ball.right && ball.x == 5) || (ball.left && ball.x == 7))
             && ((ball.y >= p1.y ) || (ball.y >= p1.y - 1 && ball.y <= p1.y + 1))) {
        ball.right = !ball.right;
        ball.left = !ball.left;
        ball.up = false;
        ball.down = false;
        ball.spin = false;
    }

    // Bat corner P1
    else if (((ball.left && ball.x == (columns - 5)) || (ball.right && ball.x == (columns - 7)))
             && (ball.y >= p2.y - 2 && ball.y <= p2.y + 2)) {
        ball.right = !ball.right;
        ball.left = !ball.left;
        ball.spin = true;
    }

    // Bat middle P1
    else if (((ball.left && ball.x == (columns - 5)) || (ball.right && ball.x == (columns - 7)))
             && ((ball.y >= p2.y ) || (ball.y >= p2.y - 1 && ball.y <= p2.y + 1))) {
        ball.right = !ball.right;
        ball.left = !ball.left;
        ball.up = false;
        ball.down = false;
        ball.spin = false;
    }

}


void draw_ball(){
    is_collision();

    mvaddch(ball.y, ball.x, ' ');
    ball.x = ball.right ? ball.x + 1 : ball.x - 1;
    if (ball.spin){
        ball.y = ball.up ? ball.y + 1 : ball.y - 1;
    }
    mvaddch(ball.y, ball.x, 'o');
    napms(20);
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
            case 'Q':
                move_up(&p1);
                break;
            case 'a':
            case 'A':
                move_down(&p1);
                break;
            case 'p':
            case 'P':
                move_up(&p2);
                break;
            case 'l':
            case 'L':
                move_down(&p2);
                break;
            case 'x':
            case 'X':
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