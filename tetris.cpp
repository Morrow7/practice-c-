#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define W 12
#define H 22

int board[H][W];
int px, py, cur, rot;
int next, nx, ny;
int score, level, lines;

const char shapes[7][4][4][4] = {
    { {0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0} },
    { {1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0} },
    { {0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0} },
    { {0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0} },
    { {1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0} },
    { {1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0} },
    { {0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0} }
};

void gotoxy(int x, int y) {
    COORD c;
    c.X = x; c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

int canplace(int x, int y, int r, int t) {
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (shapes[t][r][i][j]) {
                int nx = x + j, ny = y + i;
                if (nx < 1 || nx > W - 2 || ny >= H) return 0;
                if (ny >= 0 && board[ny][nx]) return 0;
            }
    return 1;
}

void place(int x, int y, int r, int t, int v) {
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (shapes[t][r][i][j])
                if (y + i >= 0) board[y + i][x + j] = v;
}

void drawblock(int x, int y, int c) {
    gotoxy(x * 2, y);
    if (c == 0) printf("  ");
    else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
        printf("[]");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    }
}

void draw() {
    int i, j;
    for (i = 0; i < H; i++)
        for (j = 0; j < W; j++) {
            if (i == 0 || j == 0 || j == W - 1) drawblock(j, i + 1, 8);
            else if (board[i][j]) drawblock(j, i + 1, board[i][j]);
            else drawblock(j, i + 1, 0);
        }
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (shapes[cur][rot][i][j])
                if (py + i >= 0) drawblock(px + j, py + i + 1, cur + 9);
}

void shownext() {
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (shapes[next][0][i][j])
                drawblock(16 + j, 3 + i, next + 9);
}

void newpiece() {
    cur = next;
    next = rand() % 7;
    rot = 0;
    px = 5;
    py = -1;
    if (!canplace(px, py, rot, cur)) {
        gotoxy(0, H + 3);
        printf("GAME OVER\n");
        system("pause");
        exit(0);
    }
}

void clearlines() {
    int i, j, k, n = 0;
    for (i = H - 1; i >= 0; i--) {
        int full = 1;
        for (j = 1; j < W - 1; j++)
            if (!board[i][j]) { full = 0; break; }
        if (full) {
            n++;
            for (k = i; k > 0; k--)
                for (j = 1; j < W - 1; j++)
                    board[k][j] = board[k - 1][j];
            i++;
        }
    }
    if (n) {
        lines += n;
        score += n * n * 10;
        level = lines / 10 + 1;
    }
}

void move(int dx) {
    if (canplace(px + dx, py, rot, cur)) px += dx;
}

void drop() {
    while (canplace(px, py + 1, rot, cur)) py++;
}

void lock() {
    place(px, py, rot, cur, cur + 1);
    clearlines();
    newpiece();
}

int main() {
    srand((unsigned)time(NULL));
    system("cls");
    next = rand() % 7;
    score = 0; level = 1; lines = 0;
    memset(board, 0, sizeof(board));
    newpiece();

    int tick = 0, speed = 20;
    while (1) {
        draw();
        shownext();
        gotoxy(26, 2); printf("SCORE:%d", score);
        gotoxy(26, 3); printf("LINES:%d", lines);
        gotoxy(26, 4); printf("LEVEL:%d", level);

        if (_kbhit()) {
            char c = _getch();
            if (c == 'a' || c == 'A') move(-1);
            if (c == 'd' || c == 'D') move(1);
            if (c == 'w' || c == 'W') { if (canplace(px, py, (rot + 1) % 4, cur)) rot = (rot + 1) % 4; }
            if (c == 's' || c == 'S') drop();
            if (c == 'q' || c == 'Q') break;
            if (c == ' ') { drop(); lock(); continue; }
        }

        tick++;
        if (tick >= speed - level) {
            tick = 0;
            if (canplace(px, py + 1, rot, cur)) py++;
            else lock();
        }
        Sleep(20);
    }
    return 0;
}
