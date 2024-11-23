
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>  // _kbhit(), _getch() �Լ� ����� ���� ���
#include <windows.h>

#include "ElfMath.h"


#define WIDTH 80
#define HEIGHT 24

// ���� ������ ��ũ�� ���� ����
char screenBuffer[(WIDTH + 1) * HEIGHT]; // ���๮��
int screenWidth = WIDTH;
int screenHeight = HEIGHT;

// ȭ�� �ʱ�ȭ �Լ�
void ClearScreen() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            screenBuffer[i * (WIDTH + 1) + j] = ' ';  // ȭ�� �ʱ�ȭ
        }
        screenBuffer[i * (WIDTH + 1) + WIDTH] = '\n';  // ���� ����
    }
}


// ���� �ʱ�ȭ �Լ�
void Init() {

    system("cls");
    ClearScreen();

    //Ŀ�� �����
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;  // Ŀ���� ����
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    

}

// �� ���� �����ϴ� ������ �׸��� �Լ� (�극���� �˰���)
void DrawLine(int x1, int y1, int x2, int y2, char o) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        if (x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
            screenBuffer[y1 * (WIDTH + 1) + x1] = o;  // ȭ�鿡 ���� ����
            screenBuffer[y1 * (WIDTH + 1) + (x1 + 1)] = o;
        }

        if (x1 == x2 && y1 == y2) break;
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}


// ȭ���� �׸��� �Լ�
void Draw(char* Buffer) {
    //0,0���� �̵�
    COORD coord = { 0, 0 };  // ��ǥ (0, 0)
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    // ��ũ�� ���� ���
    printf("%s", screenBuffer);  // ȭ�� ���
}

// �¾� ȭ�� �׸��� �Լ�
void drawSun(float angle) {

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
    Matrix3x3 sunScale = scale_matrix(1.0f, 0.8f); // Y�� ������ �ٿ���

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

        DrawLine(x1, y1, x2, y2, 'S');  // ��ȯ�� ��ǥ�� ������ �׸���
    }

    // �� �࿡ ���� 'S' ���̸� ä���
    for (int y = 0; y < HEIGHT; y++) {
        int firstHash = -1;
        for (int x = 0; x < WIDTH; x++) {
            if (screenBuffer[y * (WIDTH + 1) + x] == 'S') {
                if (firstHash == -1) {
                    firstHash = x;
                }
                else {
                    // 'S' ���̿� �ִ� ������ ä��
                    for (int fillX = firstHash + 1; fillX < x; fillX++) {
                        screenBuffer[y * (WIDTH + 1) + fillX] = 'S';
                    }
                    firstHash = x; // 'S'�� ������ ��ġ�� ������Ʈ
                }
            }
        }
    }

}

// ������ �߽��� ���� ������ ����. �� �Լ������� ����ϱ� ����
int globalEarthPosX = 0;
int globalEarthPosY = 0;

void drawEarth(float angle) {

    // �¾��� �߽� ��ġ
    int sunX = WIDTH / 2;
    int sunY = HEIGHT / 2;

    // ������ �ʱ� ��� ��ġ (�¾� ����)
    Vector3 earthPoint = { 0.0f, 12.0f, 1.0f }; // �߽ɿ��� 9��ŭ ������

    // ���� ���� (�¾� �߽� ����)
    Matrix3x3 orbitRotation = rotation_matrix(angle);
    earthPoint = multiply_matrix_vector(orbitRotation, earthPoint);

    // ������ ���� �߽� ��ǥ ���
    float earthX = earthPoint.x + sunX;
    float earthY = earthPoint.y + sunY;

    // ���� ��ǥ�� ��ȯ
    globalEarthPosX = (int)earthX; // ���� ������ ����
    globalEarthPosY = (int)earthY; // ���� ������ ����

    // ȭ�� ���� ������ Ȯ���ϰ� �߽� ���
    if (globalEarthPosX >= 0 && globalEarthPosX < WIDTH && globalEarthPosY >= 0 && globalEarthPosY < HEIGHT) {
        screenBuffer[globalEarthPosY * (WIDTH + 1) + globalEarthPosX] = 'E'; // �߽ɿ� 'E' ���
    }

    // ���簢���� 4�� �� (���� �߽� ����)
    Vector3 earth[4] = {
        { -2.0f, -2.0f, 1.0f }, // ���� ��
        { -2.0f,  2.0f, 1.0f }, // ���� �Ʒ�
        {  2.0f,  2.0f, 1.0f }, // ������ �Ʒ�
        {  2.0f, -2.0f, 1.0f }  // ������ ��
    };

    // ���� ���� �� ������ ���� (���� �߽� ����)
    Matrix3x3 earthRotation = rotation_matrix(angle);   // ���� ȸ�� ���
    Matrix3x3 earthScale = scale_matrix(1.0f, 0.8f);    // Y�� ������ ���̴� ������ ���

    for (int i = 0; i < 4; i++) {
        // ���� �߽� �������� �������� �̵�
        Vector3 translatedPoint = {
            earth[i].x,
            earth[i].y,
            earth[i].z
        };

        // ���� ����
        translatedPoint = multiply_matrix_vector(earthRotation, translatedPoint);

        // ������ ����
        translatedPoint = multiply_matrix_vector(earthScale, translatedPoint);

        // �ٽ� ���� �߽� ��ǥ�� �̵�
        earth[i].x = translatedPoint.x + globalEarthPosX;
        earth[i].y = translatedPoint.y + globalEarthPosY;
    }

    // ���簢�� �ܰ����� �׸���
    for (int i = 0; i < 4; i++) {
        int x1 = (int)earth[i].x;   // ���� ���� x ��ǥ
        int y1 = (int)earth[i].y;   // ���� ���� y ��ǥ
        int x2 = (int)earth[(i + 1) % 4].x; // ���� ���� x ��ǥ
        int y2 = (int)earth[(i + 1) % 4].y; // ���� ���� y ��ǥ

        DrawLine(x1, y1, x2, y2, 'E'); // �������� �ܰ��� �׸���
    }

    // �� �࿡ ���� 'E' ���̸� ä���
    for (int y = 0; y < HEIGHT; y++) {
        int firstHash = -1;
        for (int x = 0; x < WIDTH; x++) {
            if (screenBuffer[y * (WIDTH + 1) + x] == 'E') {
                if (firstHash == -1) {
                    firstHash = x;
                }
                else {
                    // 'E' ���̿� �ִ� ������ ä��
                    for (int fillX = firstHash + 1; fillX < x; fillX++) {
                        screenBuffer[y * (WIDTH + 1) + fillX] = 'E';
                    }
                    firstHash = x; // 'E'�� ������ ��ġ�� ������Ʈ
                }
            }
        }
    }
}

void drawMoon(float angle) {
    // ���� �ʱ� ��� ��ġ (���� �߽� ����)
    Vector3 moonPoint = { -1.5f, -4.0f, 1.0f }; // ���� �߽ɿ��� 6��ŭ ������

    // �� ���� (���� �߽� ����)
    Matrix3x3 moonOrbitRotation = rotation_matrix(angle); // ���� ȸ�� ���
    moonPoint = multiply_matrix_vector(moonOrbitRotation, moonPoint);

    // ���� ���� �߽� ��ǥ ��� (���� �߽� ��ǥ ����)
    float moonX = moonPoint.x + globalEarthPosX; // ���� �߽ɿ��� �̵�
    float moonY = moonPoint.y + globalEarthPosY;

    // ���� ��ǥ�� ��ȯ
    int moonPosX = (int)moonX;
    int moonPosY = (int)moonY;

    // ȭ�� ���� ������ Ȯ���ϰ� �� �߽� ���
    if (moonPosX >= 0 && moonPosX < WIDTH && moonPosY >= 0 && moonPosY < HEIGHT) {
        screenBuffer[moonPosY * (WIDTH + 1) + moonPosX] = 'M'; // �߽ɿ� 'M' ���
    }

    // ���� �����ϴ� ���� (moon ����)
    Vector3 moon[6] = {
        {  1.0f, -1.0f, 1.0f }, // ù ��° ��: 0, -1
        {  3.0f, -1.0f, 1.0f }, // ù ��° ��: 3, -1

        { 0.0f,  0.0f, 1.0f }, // �� ��° ��: -1, 0
        {  4.0f,  0.0f, 1.0f }, // �� ��° ��: 4, 0

        {  1.0f,  1.0f, 1.0f }, // �� ��° ��: 0, 1
        {  3.0f,  1.0f, 1.0f }  // �� ��° ��: 3, 1
    };

    // �� ���� �� ������ ���� (�� �߽� ����)
    Matrix3x3 moonRotation = rotation_matrix(angle);   // ���� ȸ�� ���
    Matrix3x3 moonScale = scale_matrix(1.0f, 1.0f);    // Y�� ������ ���̴� ������ ���

    for (int i = 0; i < 6; i++) {
        // �� �߽� �������� �������� �̵�
        Vector3 translatedPoint = {
            moon[i].x,
            moon[i].y,
            moon[i].z
        };

        // ���� ����
        translatedPoint = multiply_matrix_vector(moonRotation, translatedPoint);

        // ������ ����
        translatedPoint = multiply_matrix_vector(moonScale, translatedPoint);

        // �ٽ� �� �߽� ��ǥ�� �̵�
        moon[i].x = translatedPoint.x + moonPosX;
        moon[i].y = translatedPoint.y + moonPosY;
    }

    // ���簢�� �ܰ����� �׸���
    for (int i = 0; i < 6; i++) {
        int x1 = (int)moon[i].x;   // ���� ���� x ��ǥ
        int y1 = (int)moon[i].y;   // ���� ���� y ��ǥ
        int x2 = (int)moon[(i + 1) % 6].x; // ���� ���� x ��ǥ
        int y2 = (int)moon[(i + 1) % 6].y; // ���� ���� y ��ǥ

        DrawLine(x1, y1, x2, y2, 'M'); // �������� �ܰ��� �׸���
    }

    // �� �࿡ ���� 'M' ���̸� ä���
    for (int y = 0; y < HEIGHT; y++) {
        int firstHash = -1;
        for (int x = 0; x < WIDTH; x++) {
            if (screenBuffer[y * (WIDTH + 1) + x] == 'M') {
                if (firstHash == -1) {
                    firstHash = x;
                }
                else {
                    // 'M' ���̿� �ִ� ������ ä��
                    for (int fillX = firstHash + 1; fillX < x; fillX++) {
                        screenBuffer[y * (WIDTH + 1) + fillX] = 'M';
                    }
                    firstHash = x; // 'M'�� ������ ��ġ�� ������Ʈ
                }
            }
        }
    }
}



float angleS = 0; // �¾� ����

float angleE1 = 0; // ���� ����
float angleE2 = 0; // ���� ����

float angleM1 = 0; // �� ����
float angleM2 = 0; // �� ����

// ȭ�� ������Ʈ �Լ�
void Update() {
    ClearScreen();
    drawSun(angleS);
    drawEarth(angleE1);
    drawMoon(angleS);

    angleS -= 120.0f * (1.0f / 60.0f);  // �ݽð���� 3�ʴ� 1����
    angleE1 += 120.0f * (1.0f / 60.0f); // �ð���� 3�ʴ� 1����
    angleE2 += 180.0f * (1.0f / 60.0f); // �ð���� 2�ʴ� 1����
    angleM1 -= 180.0f * (1.0f / 60.0f);  // �ݽð���� 2�ʴ� 1����
    angleM2 += 360.0f * (1.0f / 60.0f);  // �ð���� 1�ʴ� 1����
}

void firstScreen() { //�й��� �̸� ���
    COORD coord = { WIDTH / 2, HEIGHT / 2 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("20231364 �̼���");
}

void secondScreen() {

    while(1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) {  // ESC Ű�� ������ ���� ����
                exit(0);
            }
            else if (ch == ' ') {
                
                ClearScreen();
              
                COORD coord = { 0, 0 };
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
                
                drawSun(0);
                drawEarth(0);
                drawMoon(0);
                Draw(screenBuffer);


                break;
            }
        }
    }
}

void thirdScreen() {
    // ������ Ÿ�̸�
    clock_t lastTime = clock();
    int fps = 60;
    double frameTime = 1000.0 / fps;


    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) {  // ESC Ű�� ������ ���� ����
                exit(0);
            }
            else if (ch == ' ') {

                while (1) {

                    // ���� �ð� ���
                    clock_t now = clock();
                    int fps = 60;
                    double frameTime = 1000.0 / fps;

                    double deltaTime = (double)(now - lastTime) / CLOCKS_PER_SEC * 1000.0;

                    // ������ ����
                    if (deltaTime >= frameTime) {
                        lastTime = now;
  
                        COORD coord = { 0, 0 };
                        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

                        Update();

                        
                        Draw(screenBuffer);
                        
                    }

                    if (_kbhit()) {
                        char ch2 = _getch();
                        if (ch2 == 27) {  // ESC Ű�� ������ ���� ����
                            exit(0);
                        }
                    }

                }

                
            }
        }
    }
}


// ���� ����
int main() {
    Init(); // ȭ�� �ʱ�ȭ

    firstScreen();
    secondScreen();
    thirdScreen();

    return 0;
}