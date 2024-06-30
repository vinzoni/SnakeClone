#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <iostream>

using namespace std;

const int ROWS = 23;
const int COLUMNS = 80;
const char *PAUSE_MSG = "Hit SPACE key to pause the game.";
const char *RESTORE_MSG = "Hit any key to restore the game.";

struct Point {
    int x;
    int y;
};

vector<Point> snake;

int x_direction = 1;
int y_direction = 0;

bool snake_edge_collision(vector<Point> &snake);
bool bonus_catch(Point snake_head, Point bonus);
void grow_snake(vector<Point> &snake);
void snake_move(vector<Point> &snake);
Point seed_bonus();
bool is_snake_here(Point p, vector<Point> snake);
bool snake_auto_collision(vector<Point> snake);
void bye();
void message_to_user(const char* msg);
void keyboard_poll();

void keyboard_poll() {

    int ch = getch();

    switch(ch) {
        case KEY_LEFT:
            x_direction = -1;
            y_direction = 0;
            break;
        case KEY_RIGHT:
            x_direction = 1;
            y_direction = 0;
            break;
        case KEY_UP:
            x_direction = 0;
            y_direction = -1;
            break;
        case KEY_DOWN:
            x_direction = 0;
            y_direction = 1;
            break;
        case ' ':
            timeout(-1); // blocking getch()
            message_to_user(RESTORE_MSG);
            getch();
            timeout(0); // blocking getch()
            break;
        }
}

void message_to_user(const char* msg) {
    move(23, 1);
    printw(msg);
    refresh();
}

void bye() {
    move(12, 30);
    printw("G A M E    O V E R");
    refresh();
    napms(3000);
}

bool snake_edge_collision(vector<Point> &snake) {
    Point head = snake[0];
    return head.x == -1 || head.y == -1 || head.x == COLUMNS || head.y == ROWS;
}

bool bonus_catch(Point snake_head, Point bonus) {
    return snake_head.x == bonus.x && snake_head.y == bonus.y;
}

void grow_snake(vector<Point> &snake) {
    Point head = snake[0];
    Point new_head;
    new_head.x = head.x + x_direction;
    new_head.y = head.x + y_direction;
    snake.push_back(new_head);
}

void snake_move(vector<Point> &snake) {
   for (unsigned int i=snake.size()-1; i>0; i--) {
       snake[i].x = snake[i-1].x;
       snake[i].y = snake[i-1].y;
   }
   snake[0].x = snake[0].x + x_direction;
   snake[0].y = snake[0].y + y_direction;
}

Point seed_bonus() {
   Point bonus;

   do {
       bonus.x = rand() % COLUMNS;
       bonus.y = rand() % ROWS;
   } while (is_snake_here(bonus, snake));

   return bonus;
}

bool is_snake_here(Point p, vector<Point> snake) {
    for (Point ps: snake) {
        if (p.x == ps.x && p.y == ps.y) {
            return true;
        }
    }
    return false;
}

bool snake_auto_collision(vector<Point> snake) {
    Point head = snake[0];

    for (unsigned int i=1; i<snake.size(); i++) {
        if ((head.x == snake[i].x) && (head.y == snake[i].y))
            return true;
    }

    return false;
}

int main()
{
    srand(time(0));

    // make a snake
    Point snake_head;
    snake_head.x = 0;
    snake_head.y = 10;
    snake.push_back(snake_head);

    Point bonus = seed_bonus();

    // initializes the screen
    // sets up memory and clears the screen
    initscr();

    curs_set(0); // invisible cursor

    timeout(0); // for non blocking getch()
    keypad(stdscr, true); // so thatn arrow keys returns a single value

    while (true)
    {
        Point head = snake[0];
        if (snake_edge_collision(snake)) {
            refresh();
            bye();
            break;
        }

        if (snake_auto_collision(snake)) {
            refresh();
            bye();
            break;
        }

        clear();

        message_to_user(PAUSE_MSG);

        move (bonus.y, bonus.x);
        addch('+');

        for (Point p: snake) {
            move(p.y, p.x);
            addch('*');
        }

        if (bonus_catch(head, bonus)) {
            grow_snake(snake);
            move (bonus.x, bonus.y);
            addch(' ');
            bonus = seed_bonus();
        }

        refresh();

        napms(150); // sleep x milliseconds

        keyboard_poll();
        snake_move(snake);

    }
    curs_set(1); // visible cursor

    // waits for user input, returns int value of that key
    getch();

    // deallocates memory and ends ncurses
    endwin();

    return 0;
}
