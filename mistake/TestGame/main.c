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

/*
// �¾� ȭ�� �׸��� �Լ�
void drawSun(char* screenBuffer, float angle) {

    Vector3 sun[3] = { // 3���� ���� ���� �迭 (�¾� �ﰢ�� ���)
        { 40.0f, 10.0f, 1.0f }, // �� ������
        { 36.0f, 15.0f, 1.0f }, // ���� ������
        { 44.0f, 15.0f, 1.0f }, // ������ ������
    };

    // ȸ�� �߽� (40, 12)
    float centerX = 40.0f;
    float centerY = 12.0f;

    // ȸ�� ����
    Matrix3x3 sunRotation = rotation_matrix(angle);
    Matrix3x3 sunScale = scale_matrix(1.0f, 1.0f); // Y�� ������ �ٿ���

    for (int i = 0; i < 3; i++) {
        // ȸ�� �߽��� �������� �̵�
        Vector3 sunTranslated = { sun[i].x - centerX, sun[i].y - centerY, sun[i].z };

        // ȸ�� ����
        sunTranslated = multiply_matrix_vector(sunRotation, sunTranslated);

        // ������ ���� (ȸ�� �Ŀ�)
        sunTranslated = multiply_matrix_vector(sunScale, sunTranslated);

        // ȸ�� �� ���� ��ġ�� �ǵ���
        sun[i].x = sunTranslated.x + centerX;
        sun[i].y = sunTranslated.y + centerY;
    }

    // sun �迭�� �� ���� �̿��ؼ� ���� �׸���
    for (int i = 0; i < 3; i++) {
        int x1 = (int)sun[i].x;   // x ��ǥ�� int�� ��ȯ
        int y1 = (int)sun[i].y;   // y ��ǥ�� int�� ��ȯ
        int x2 = (int)sun[(i + 1) % 3].x;  // ���� ���� x ��ǥ
        int y2 = (int)sun[(i + 1) % 3].y;  // ���� ���� y ��ǥ

        DrawLine(x1, y1, x2, y2, 'S', screenBuffer);  // ��ȯ�� ��ǥ�� ������ �׸���
    }

    // �� �࿡ ���� 'S' ���̸� ä���
    for (int y = 0; y < HEIGHT; y++) {
        int firstHash = -1;
        for (int x = 0; x < WIDTH * 2; x++) {
            if (screenBuffer[y * (WIDTH + 1) + x] == 'S') {
                if (firstHash == -1) {
                    firstHash = x;
                }
                else {
                    // 'S' ���̿� �ִ� ������ ä��
                    for (int fillX = firstHash + 1; fillX < x; fillX++) {
                        screenBuffer[y * (WIDTH * 2 + 1) + fillX] = 'S';
                    }
                    firstHash = x; // 'S'�� ������ ��ġ�� ������Ʈ
                }
            }
        }
    }

}
*/

void tri(char* screenBuffer) {
    Vector3 sun[3] = { // 3���� ���� ���� �迭 (�¾� �ﰢ�� ���)
        { 20.0f, 10.0f, 1.0f }, // �� ������
        { 12.0f, 20.0f, 1.0f }, // ���� ������
        { 28.0f, 20.0f, 1.0f }, // ������ ������
    };


    // sun �迭�� �� ���� �̿��ؼ� ���� �׸���
    for (int i = 0; i < 3; i++) {
        int x1 = (int)sun[i].x;   // x ��ǥ�� int�� ��ȯ
        int y1 = (int)sun[i].y;   // y ��ǥ�� int�� ��ȯ
        int x2 = (int)sun[(i + 1) % 3].x;  // ���� ���� x ��ǥ
        int y2 = (int)sun[(i + 1) % 3].y;  // ���� ���� y ��ǥ

        DrawLine(x1, y1, x2, y2, 'S', screenBuffer);  // ��ȯ�� ��ǥ�� ������ �׸���
    }

    // �� �������� "++" ���
    int topX = (int)sun[0].x; // �� ������ x ��ǥ
    int topY = (int)sun[0].y; // �� ������ y ��ǥ
    if (topX >= 0 && topX < WIDTH && topY >= 0 && topY < HEIGHT) {
        screenBuffer[topY * (WIDTH * 2 + 1) + topX * 2] = '+';       // ù ��° '+'
        screenBuffer[topY * (WIDTH * 2 + 1) + topX * 2 + 1] = '+';   // �� ��° '+'

    }
}


// ȭ�� ������Ʈ �Լ�
void Update(char* screenBuffer) {
    ClearScreen(screenBuffer);

    tri(screenBuffer);
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
