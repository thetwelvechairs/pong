#define _XOPEN_SOURCE_EXTENDED 1

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>


WINDOW *mainWindow;

int rows, columns;
int score_p1 = 0;
int score_p2 = 0;
bool sound = true;

//struct json_object *new_obj;

struct Ball {
    int x;
    int y;
    bool left;
    bool right;
    bool up;
    bool down;
    bool spin;
    bool sharp;
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
//    wbkgd(mainWindow, ACS_CKBOARD);
    attrset(A_BOLD);
    char header[] = "   Player 1: 0  |  Player 2: 0   ";
    mvaddstr(0, 3, header);
    attroff(A_BOLD);
    char footer1[] = "   S[o]und On/Off   ";
    mvaddstr(rows, 3, footer1);
    char footer2[] = "   E[x]it   ";
    mvaddstr(rows, columns - sizeof(footer2), footer2);
//    halfdelay(1);
    nodelay(stdscr, TRUE);

    ball.x = columns / 2;
    ball.y = rows / 2;
    ball.left = false;
    ball.right = false;
    ball.up = false;
    ball.down = false;
    ball.spin = false;
    ball.sharp = false;

    p1.x = 6;
    p1.y = rows / 2;

    p2.x = columns - 6;
    p2.y = rows / 2;

}


void update_score(int x, int integer){
    char buffer[] = {0, 0};
    char blank[] = {0, 0};

    sprintf(buffer, "%d", integer);
    attrset(A_BOLD);
    mvaddstr(0, x, blank);
    mvaddstr(0, x, buffer);
    attroff(A_BOLD);
}


void check_collision(){
    // Y-axis border / goal
    if (ball.x == 1 || ball.x == columns-1 || ball.x == columns || ball.x == columns + 1){
        if (ball.right){
            if (score_p1 < 9) {
                score_p1 += 1;
                update_score(16, score_p1);
            }
        }
        else {
            if (score_p2 < 9) {
                score_p2 += 1;
                update_score(32, score_p2);
            }
        }

        ball.left = !ball.left;
        ball.right = !ball.right;

        if (sound){
            beep();
        }
    }

    // X-axis border
    else if (ball.y == 1 || ball.y == rows - 1) {
        ball.up = !ball.up;
        ball.down = !ball.down;
    }

    // P1
    else if ((ball.right && ball.x == 5) || (ball.left && ball.x == 7)){
        // Bat corner
        if ((ball.y == p1.y - 2) || (ball.y == p1.y + 2)) {
            ball.left = !ball.left;
            ball.right = !ball.right;
            ball.spin = true;
            ball.sharp = true;
        }
        // Bat middle
        else if ((ball.y == p1.y) || (ball.y == p1.y - 1) || (ball.y == p1.y + 1)) {
            ball.left = !ball.left;
            ball.right = !ball.right;
            ball.spin = false;
            ball.sharp = false;
        }
    }

    // P2
    else if ((ball.left && ball.x == (columns - 5)) || (ball.right && ball.x == (columns - 7))){
        // Bat corner
        if ((ball.y == p2.y - 2) || (ball.y == p2.y + 2)) {
            ball.left = !ball.left;
            ball.right = !ball.right;
            ball.spin = true;
            ball.sharp = true;
        }
        // Bat middle
        else if ((ball.y == p2.y) || (ball.y == p2.y - 1) || (ball.y == p2.y + 1)) {
            ball.left = !ball.left;
            ball.right = !ball.right;
            ball.spin = false;
            ball.sharp = false;
        }
    }
}


void draw_ball(){
    check_collision();

    mvaddch(ball.y, ball.x, ' ');

    ball.x = ball.right ? ball.x + 1 : ball.x - 1;

    if (ball.spin && ball.sharp){
        int angle = 1;
        if (ball.y + 2 > rows){
            angle = 2;
        }
        ball.y = ball.down ? ball.y + angle : ball.y - 1;
    }

    if (ball.spin && !ball.sharp){
        ball.y = ball.down ? ball.y + 1 : ball.y - 1;
    }

    mvaddch(ball.y, ball.x, 'O');
    refresh();
}


void draw_player(struct Player *player){
    mvaddch(player->y, player->x, ACS_CKBOARD);
    for (int i = 1; i <= 2; i++) {
        if ((player->y + i) < rows){
            mvaddch(player->y + i, player->x, ACS_CKBOARD);
        }
        if ((player->y - i) > 0) {
            mvaddch(player->y - i, player->x, ACS_CKBOARD);
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
//    new_obj = json_tokener_parse("/* more difficult test case */ { \"glossary\": { \"title\": \"example glossary\", \"pageCount\": 100, \"GlossDiv\": { \"title\": \"S\", \"GlossList\": [ { \"ID\": \"SGML\", \"SortAs\": \"SGML\", \"GlossTerm\": \"Standard Generalized Markup Language\", \"Acronym\": \"SGML\", \"Abbrev\": \"ISO 8879:1986\", \"GlossDef\": \"A meta-markup language, used to create markup languages such as DocBook.\", \"GlossSeeAlso\": [\"GML\", \"XML\", \"markup\"] } ] } } }");
//    printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));

    if ((mainWindow = initscr() ) == NULL ) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    init();

    draw_player(&p1);
    draw_player(&p2);

    while (true){

        char keystroke = getch();
        napms(20);
        draw_ball();

        switch (keystroke) {
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
            case 'o':
            case 'O':
                sound = !sound;
                break;
            case 'x':
            case 'X':
                delwin(mainWindow);
                endwin();
                return EXIT_SUCCESS;
            default:
                break;
        }

        refresh();
    }
}