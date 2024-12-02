#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>  // _kbhit(), _getch() �Լ� ����� ���� ���
#include <windows.h>

#include "ElfMath.h"

#define WIDTH 40
#define HEIGHT 24

// ȭ�� �ʱ�ȭ �Լ�
void ClearScreen(char screenBuffer[]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH * 2; j++) {
            screenBuffer[i * (WIDTH * 2 + 1) + j] = ' ';  // ȭ�� �ʱ�ȭ
        }
        screenBuffer[i * (WIDTH * 2 + 1) + WIDTH * 2] = '\n';  // ���� ����
    }
    screenBuffer[(WIDTH * 2 + 1) * HEIGHT - 1] = '\0';  // ���ڿ��� �����ٰ� �˷���!! �� �־�ߵ�
}

void Init() {
    system("cls");
    // ȭ�� ���� �ʱ�ȭ
    char screenBuffer[(WIDTH * 2 + 1) * HEIGHT];
    ClearScreen(screenBuffer);

    // Ŀ�� �����
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;  // Ŀ���� ����
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// ȭ���� �׸��� �Լ�
void Draw(char* screenBuffer) {
    // 0,0���� �̵�
    COORD coord = { 0, 0 };  // ��ǥ (0, 0)
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    // ��ũ�� ���� ���
    printf("%s", screenBuffer);  // ȭ�� ���
}

// �� ���� �����ϴ� ������ �׸��� �Լ� (�극���� �˰���)
void DrawLine(float x1, float y1, float x2, float y2, char o, char* screenBuffer) {
    // float �μ��� int�� ��ȯ
    int ix1 = (int)(x1 + 0.5f); // �ݿø� ó��
    int iy1 = (int)(y1 + 0.5f);
    int ix2 = (int)(x2 + 0.5f);
    int iy2 = (int)(y2 + 0.5f);

    // �Ÿ��� ���� ���
    int dx = abs(ix2 - ix1);
    int dy = abs(iy2 - iy1);
    int sx = (ix1 < ix2) ? 1 : -1;
    int sy = (iy1 < iy2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        // ȭ�� ��� ���� �ִ� ��츸 �׸���
        if (ix1 >= 0 && ix1 < WIDTH && iy1 >= 0 && iy1 < HEIGHT) {
            screenBuffer[iy1 * (WIDTH * 2 + 1) + ix1 * 2] = o;       // ù ��° char
            screenBuffer[iy1 * (WIDTH * 2 + 1) + ix1 * 2 + 1] = o;   // �� ��° char
        }

        // ���� ����
        if (ix1 == ix2 && iy1 == iy2) break;

        // ���� �� ���
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            ix1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            iy1 += sy;
        }
    }
}


// ȭ�� ������Ʈ �Լ�
void Update(char* screenBuffer) {
    ClearScreen(screenBuffer);

    DrawLine(0, 0, 40, 24, 'C', screenBuffer);
}

int main() {
    // ȭ�� ���� ����
    char screenBuffer[(WIDTH * 2 + 1) * HEIGHT];

    Init();

    // ������ Ÿ�̸�
    clock_t lastTime = clock();
    int fps = 30;
    double frameTime = 1000.0 / fps;

    while (1) {
        // ���� �ð� ���
        clock_t now = clock();
        double deltaTime = (double)(now - lastTime) / CLOCKS_PER_SEC * 1000.0;

        COORD coord = { 0, 0 };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

        Update(screenBuffer);   // ȭ�� ������Ʈ

        Draw(screenBuffer);     // ȭ�� ���

        Sleep(frameTime - deltaTime); // CPU �� �� �� �ֵ��� ����!

        if (_kbhit()) {
            char ch2 = _getch();
            if (ch2 == 27) {  // ESC Ű�� ������ ���� ����
                exit(0);
            }
        }
    }
}
