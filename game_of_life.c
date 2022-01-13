#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#define WIDTH 80
#define HEIGHT 25
#define SPEED_UP 'x'
#define SPEED_DOWN 'z'
#define END 'e'

void draw_state();
void game_of_life();
int r_search(int x, int y);
void life(int i, int j);
void dead(int i, int j);
void input();
void init_cells_console();
void init_cells_file();
int round_go_x(int i, int go);
int round_go_y(int j, int go);
void keybord_handler();
void all_dead_or_no_move();
void print_end();
void instruction();

int prev_cells[HEIGHT][WIDTH];
int new_cells[HEIGHT][WIDTH];
double speed = 1000000;
double c_speed = 1;
int end_game = 0;

int main() {
    instruction();
    input();
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(0);
    draw_state();
    refresh();
    while (end_game == 0) {
        nodelay(stdscr, 1);
        keybord_handler();
        usleep(speed);
        game_of_life();
        draw_state();
        refresh();
        print_end();
    }
    getchar();
    endwin();
    return 0;
}

void draw_state() {
    erase();
    mvvline(0, WIDTH + 1, ACS_VLINE, HEIGHT + 2);
    mvvline(0, 0, ACS_VLINE, HEIGHT + 2);
    mvhline(0, 0, ACS_HLINE, WIDTH + 2);
    mvhline(HEIGHT + 1, 0, ACS_HLINE, WIDTH + 2);
    for (int i = 1; i < HEIGHT + 1; i++) {
        for (int j = 1; j < WIDTH + 1; j++) {
            if (prev_cells[i-1][j-1] == 1) {
                mvprintw(i, j, "x");
            }
        }
    }
    mvprintw(HEIGHT + 1, 0, "speed %.0f ", c_speed);
}

void game_of_life() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (prev_cells[i][j] == 1) {
                life(i, j);
            } else {
                dead(i, j);
            }
        }
    }
    all_dead_or_no_move();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            prev_cells[i][j] = new_cells[i][j];
        }
    }
}

void life(int i, int j) {
    if (r_search(i, j) == 2 || r_search(i, j) == 3) {
        new_cells[i][j] = 1;
    } else {
        new_cells[i][j] = 0;
    }
}

void dead(int i, int j) {
    if (r_search(i, j) == 3) {
        new_cells[i][j] = 1;
    } else {
        new_cells[i][j] = 0;
    }
}

int r_search(int x, int y) {
    int res = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (prev_cells[round_go_x(x, i)][round_go_y(y, j)] == 1) {
                if (i != 0 || j != 0) {
                    res++;
                }
            }
        }
    }
    return res;
}

int round_go_x(int i, int go) {
    int move = i + go;
    if (move > HEIGHT - 1) {
        move = move - HEIGHT;
    }
    if (move < 0) {
        move = move + HEIGHT;
    }
    return move;
}

int round_go_y(int j, int go) {
    int move = j + go;
    if (move > WIDTH - 1) {
        move = move - WIDTH;
    }
    if (move < 0) {
        move = move + WIDTH;
    }
    return move;
}
void input() {
    int n = 0;
    if (scanf("%d", &n) == 1 && (n == 0 || n == 1)) {
        if (n == 0) {
            init_cells_console();
        } else {
            init_cells_file();
        }
    }
}

void init_cells_console() {
    int n, x, y;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            prev_cells[i][j] = 0;
            new_cells[i][j] = 0;
        }
    }
    if (scanf("%d", &n) == 1) {
        for (int i = 0; i < n; i++) {
            if (scanf("%d%d", &x, &y) == 2) {
                if (x < HEIGHT + 1 && y < WIDTH + 1 && x > 0 && y > 0) {
                    prev_cells[x][y] = 1;
                }
            }
        }
    }
}

void init_cells_file() {
    FILE *F = NULL;
    int f = 0;
    if (scanf("%d", &f) == 1 && f < 8 && f > 0) {
        switch (f) {
            case 1:
                F = fopen("Glider.txt", "r");
                break;
            case 2:
                F = fopen("Flower.txt", "r");
                break;
            case 3:
                F = fopen("Gospers_Gun.txt", "r");
                break;
            case 4:
                F = fopen("Rocket.txt", "r");
                break;
            case 5:
                F = fopen("Gospers_Gun+Eater.txt", "r");
                break;
            case 6:
                F = fopen("Monogram.txt", "r");
                break;
            case 7:
                F = fopen("Prepulsar.txt", "r");
                break;
            default:
                break;
        }
        int n, x, y;
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                prev_cells[i][j] = 0;
                new_cells[i][j] = 0;
            }
        }
        if (fscanf(F, "%d", &n) == 1) {
            for (int i = 0; i < n; i++) {
                if (fscanf(F, "%d%d", &x, &y) == 2) {
                    if (x < HEIGHT + 1 && y < WIDTH + 1 && x > 0 && y > 0) {
                        prev_cells[x][y] = 1;
                    }
                } else {
                    fclose(F);
                }
            }
        } else {
            fclose(F);
        }
        fclose(F);
    }
}

void keybord_handler() {
    switch (getch()) {
        case SPEED_UP:
            if (speed > 0) {
                speed = speed / 1.5;
                c_speed++;
            }
            break;
        case SPEED_DOWN:
            if (c_speed > 1) {
                speed = speed * 1.5;
                c_speed--;
            }
            break;
        case END:
            end_game = 1;
            break;
    }
}

void all_dead_or_no_move() {
    int k = 0, c = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (prev_cells[i][j] == 1) {
                k = 1;
            }
            if (prev_cells[i][j] != new_cells[i][j]) {
                c = 1;
            }
        }
    }
    if (k == 0 || c == 0) {
        end_game = 1;
    }
}

void print_end() {
    if (end_game == 1) {
        mvprintw(HEIGHT / 2, (WIDTH / 2) - 3, "END GAME");
        refresh();
        usleep(5000000);
    }
}

void instruction() {
    printf("For keyboard input enter 0. To use patterns 1.\n");
    printf("You can change the speed by pressing z (for down) or x (for up)\n");
    printf("e for exit\n");
    printf("Available patterns:\n");
    printf("1. Glider\n");
    printf("2. Flower\n");
    printf("3. Gospers_Gun\n");
    printf("4. Rocket\n");
    printf("5. Gospers_Gun+Eater\n");
    printf("6. Monogram\n");
    printf("7. Prepulsar\n");
}
