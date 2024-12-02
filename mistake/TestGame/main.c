#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>  // _kbhit(), _getch() 함수 사용을 위한 헤더
#include <windows.h>

#include "ElfMath.h"

#define WIDTH 40
#define HEIGHT 24

// 화면 초기화 함수
void ClearScreen(char screenBuffer[]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH * 2; j++) {
            screenBuffer[i * (WIDTH * 2 + 1) + j] = ' ';  // 화면 초기화
        }
        screenBuffer[i * (WIDTH * 2 + 1) + WIDTH * 2] = '\n';  // 개행 문자
    }
    screenBuffer[(WIDTH * 2 + 1) * HEIGHT - 1] = '\0';  // 문자열이 끝났다고 알려줌!! 꼭 있어야됨
}

void Init() {
    system("cls");
    // 화면 버퍼 초기화
    char screenBuffer[(WIDTH * 2 + 1) * HEIGHT];
    ClearScreen(screenBuffer);

    // 커서 숨기기
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;  // 커서를 숨김
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// 화면을 그리는 함수
void Draw(char* screenBuffer) {
    // 0,0으로 이동
    COORD coord = { 0, 0 };  // 좌표 (0, 0)
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    // 스크린 버퍼 출력
    printf("%s", screenBuffer);  // 화면 출력
}

// 두 점을 연결하는 직선을 그리는 함수 (브레젠햄 알고리즘)
void DrawLine(float x1, float y1, float x2, float y2, char o, char* screenBuffer) {
    // float 인수를 int로 변환
    int ix1 = (int)(x1 + 0.5f); // 반올림 처리
    int iy1 = (int)(y1 + 0.5f);
    int ix2 = (int)(x2 + 0.5f);
    int iy2 = (int)(y2 + 0.5f);

    // 거리와 방향 계산
    int dx = abs(ix2 - ix1);
    int dy = abs(iy2 - iy1);
    int sx = (ix1 < ix2) ? 1 : -1;
    int sy = (iy1 < iy2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        // 화면 경계 내에 있는 경우만 그리기
        if (ix1 >= 0 && ix1 < WIDTH && iy1 >= 0 && iy1 < HEIGHT) {
            screenBuffer[iy1 * (WIDTH * 2 + 1) + ix1 * 2] = o;       // 첫 번째 char
            screenBuffer[iy1 * (WIDTH * 2 + 1) + ix1 * 2 + 1] = o;   // 두 번째 char
        }

        // 종료 조건
        if (ix1 == ix2 && iy1 == iy2) break;

        // 다음 점 계산
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
// 태양 화면 그리기 함수
void drawSun(char* screenBuffer, float angle) {

    Vector3 sun[3] = { // 3개의 값을 가진 배열 (태양 삼각형 모양)
        { 40.0f, 10.0f, 1.0f }, // 위 꼭짓점
        { 36.0f, 15.0f, 1.0f }, // 왼쪽 꼭짓점
        { 44.0f, 15.0f, 1.0f }, // 오른쪽 꼭짓점
    };

    // 회전 중심 (40, 12)
    float centerX = 40.0f;
    float centerY = 12.0f;

    // 회전 적용
    Matrix3x3 sunRotation = rotation_matrix(angle);
    Matrix3x3 sunScale = scale_matrix(1.0f, 1.0f); // Y축 비율을 줄여줌

    for (int i = 0; i < 3; i++) {
        // 회전 중심을 원점으로 이동
        Vector3 sunTranslated = { sun[i].x - centerX, sun[i].y - centerY, sun[i].z };

        // 회전 적용
        sunTranslated = multiply_matrix_vector(sunRotation, sunTranslated);

        // 스케일 적용 (회전 후에)
        sunTranslated = multiply_matrix_vector(sunScale, sunTranslated);

        // 회전 후 원래 위치로 되돌림
        sun[i].x = sunTranslated.x + centerX;
        sun[i].y = sunTranslated.y + centerY;
    }

    // sun 배열의 각 점을 이용해서 직선 그리기
    for (int i = 0; i < 3; i++) {
        int x1 = (int)sun[i].x;   // x 좌표를 int로 변환
        int y1 = (int)sun[i].y;   // y 좌표를 int로 변환
        int x2 = (int)sun[(i + 1) % 3].x;  // 다음 점의 x 좌표
        int y2 = (int)sun[(i + 1) % 3].y;  // 다음 점의 y 좌표

        DrawLine(x1, y1, x2, y2, 'S', screenBuffer);  // 변환된 좌표로 직선을 그린다
    }

    // 각 행에 대해 'S' 사이를 채우기
    for (int y = 0; y < HEIGHT; y++) {
        int firstHash = -1;
        for (int x = 0; x < WIDTH * 2; x++) {
            if (screenBuffer[y * (WIDTH + 1) + x] == 'S') {
                if (firstHash == -1) {
                    firstHash = x;
                }
                else {
                    // 'S' 사이에 있는 영역을 채움
                    for (int fillX = firstHash + 1; fillX < x; fillX++) {
                        screenBuffer[y * (WIDTH * 2 + 1) + fillX] = 'S';
                    }
                    firstHash = x; // 'S'의 마지막 위치로 업데이트
                }
            }
        }
    }

}
*/

void tri(char* screenBuffer) {
    Vector3 sun[3] = { // 3개의 값을 가진 배열 (태양 삼각형 모양)
        { 20.0f, 10.0f, 1.0f }, // 위 꼭짓점
        { 12.0f, 20.0f, 1.0f }, // 왼쪽 꼭짓점
        { 28.0f, 20.0f, 1.0f }, // 오른쪽 꼭짓점
    };


    // sun 배열의 각 점을 이용해서 직선 그리기
    for (int i = 0; i < 3; i++) {
        int x1 = (int)sun[i].x;   // x 좌표를 int로 변환
        int y1 = (int)sun[i].y;   // y 좌표를 int로 변환
        int x2 = (int)sun[(i + 1) % 3].x;  // 다음 점의 x 좌표
        int y2 = (int)sun[(i + 1) % 3].y;  // 다음 점의 y 좌표

        DrawLine(x1, y1, x2, y2, 'S', screenBuffer);  // 변환된 좌표로 직선을 그린다
    }

    // 위 꼭짓점에 "++" 출력
    int topX = (int)sun[0].x; // 위 꼭짓점 x 좌표
    int topY = (int)sun[0].y; // 위 꼭짓점 y 좌표
    if (topX >= 0 && topX < WIDTH && topY >= 0 && topY < HEIGHT) {
        screenBuffer[topY * (WIDTH * 2 + 1) + topX * 2] = '+';       // 첫 번째 '+'
        screenBuffer[topY * (WIDTH * 2 + 1) + topX * 2 + 1] = '+';   // 두 번째 '+'

    }
}


// 화면 업데이트 함수
void Update(char* screenBuffer) {
    ClearScreen(screenBuffer);

    tri(screenBuffer);
}

int main() {
    // 화면 버퍼 선언
    char screenBuffer[(WIDTH * 2 + 1) * HEIGHT];

    Init();

    // 프레임 타이머
    clock_t lastTime = clock();
    int fps = 30;
    double frameTime = 1000.0 / fps;

    while (1) {
        // 현재 시간 계산
        clock_t now = clock();
        double deltaTime = (double)(now - lastTime) / CLOCKS_PER_SEC * 1000.0;

        COORD coord = { 0, 0 };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

        Update(screenBuffer);   // 화면 업데이트

        Draw(screenBuffer);     // 화면 출력

        Sleep(frameTime - deltaTime); // CPU 가 쉴 수 있도록 하자!

        if (_kbhit()) {
            char ch2 = _getch();
            if (ch2 == 27) {  // ESC 키가 눌리면 게임 종료
                exit(0);
            }
        }
    }
}
