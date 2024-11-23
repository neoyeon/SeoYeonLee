
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>  // _kbhit(), _getch() 함수 사용을 위한 헤더
#include <windows.h>

#include "ElfMath.h"


#define WIDTH 80
#define HEIGHT 24

// 전역 변수로 스크린 버퍼 선언
char screenBuffer[(WIDTH + 1) * HEIGHT]; // 개행문자
int screenWidth = WIDTH;
int screenHeight = HEIGHT;

// 화면 초기화 함수
void ClearScreen() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            screenBuffer[i * (WIDTH + 1) + j] = ' ';  // 화면 초기화
        }
        screenBuffer[i * (WIDTH + 1) + WIDTH] = '\n';  // 개행 문자
    }
}


// 게임 초기화 함수
void Init() {

    system("cls");
    ClearScreen();

    //커서 숨기기
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;  // 커서를 숨김
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    

}

// 두 점을 연결하는 직선을 그리는 함수 (브레젠햄 알고리즘)
void DrawLine(int x1, int y1, int x2, int y2, char o) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        if (x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
            screenBuffer[y1 * (WIDTH + 1) + x1] = o;  // 화면에 점을 찍음
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


// 화면을 그리는 함수
void Draw(char* Buffer) {
    //0,0으로 이동
    COORD coord = { 0, 0 };  // 좌표 (0, 0)
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    // 스크린 버퍼 출력
    printf("%s", screenBuffer);  // 화면 출력
}

// 태양 화면 그리기 함수
void drawSun(float angle) {

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
    Matrix3x3 sunScale = scale_matrix(1.0f, 0.8f); // Y축 비율을 줄여줌

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

        DrawLine(x1, y1, x2, y2, 'S');  // 변환된 좌표로 직선을 그린다
    }

    // 각 행에 대해 'S' 사이를 채우기
    for (int y = 0; y < HEIGHT; y++) {
        int firstHash = -1;
        for (int x = 0; x < WIDTH; x++) {
            if (screenBuffer[y * (WIDTH + 1) + x] == 'S') {
                if (firstHash == -1) {
                    firstHash = x;
                }
                else {
                    // 'S' 사이에 있는 영역을 채움
                    for (int fillX = firstHash + 1; fillX < x; fillX++) {
                        screenBuffer[y * (WIDTH + 1) + fillX] = 'S';
                    }
                    firstHash = x; // 'S'의 마지막 위치로 업데이트
                }
            }
        }
    }

}

// 지구의 중심을 전역 변수로 선언. 달 함수에서도 사용하기 위해
int globalEarthPosX = 0;
int globalEarthPosY = 0;

void drawEarth(float angle) {

    // 태양의 중심 위치
    int sunX = WIDTH / 2;
    int sunY = HEIGHT / 2;

    // 지구의 초기 상대 위치 (태양 기준)
    Vector3 earthPoint = { 0.0f, 12.0f, 1.0f }; // 중심에서 9만큼 떨어짐

    // 지구 공전 (태양 중심 기준)
    Matrix3x3 orbitRotation = rotation_matrix(angle);
    earthPoint = multiply_matrix_vector(orbitRotation, earthPoint);

    // 지구의 실제 중심 좌표 계산
    float earthX = earthPoint.x + sunX;
    float earthY = earthPoint.y + sunY;

    // 정수 좌표로 변환
    globalEarthPosX = (int)earthX; // 전역 변수에 저장
    globalEarthPosY = (int)earthY; // 전역 변수에 저장

    // 화면 범위 내인지 확인하고 중심 출력
    if (globalEarthPosX >= 0 && globalEarthPosX < WIDTH && globalEarthPosY >= 0 && globalEarthPosY < HEIGHT) {
        screenBuffer[globalEarthPosY * (WIDTH + 1) + globalEarthPosX] = 'E'; // 중심에 'E' 출력
    }

    // 직사각형의 4개 점 (지구 중심 기준)
    Vector3 earth[4] = {
        { -2.0f, -2.0f, 1.0f }, // 왼쪽 위
        { -2.0f,  2.0f, 1.0f }, // 왼쪽 아래
        {  2.0f,  2.0f, 1.0f }, // 오른쪽 아래
        {  2.0f, -2.0f, 1.0f }  // 오른쪽 위
    };

    // 지구 자전 및 스케일 적용 (지구 중심 기준)
    Matrix3x3 earthRotation = rotation_matrix(angle);   // 자전 회전 행렬
    Matrix3x3 earthScale = scale_matrix(1.0f, 0.8f);    // Y축 비율을 줄이는 스케일 행렬

    for (int i = 0; i < 4; i++) {
        // 지구 중심 기준으로 원점으로 이동
        Vector3 translatedPoint = {
            earth[i].x,
            earth[i].y,
            earth[i].z
        };

        // 자전 적용
        translatedPoint = multiply_matrix_vector(earthRotation, translatedPoint);

        // 스케일 적용
        translatedPoint = multiply_matrix_vector(earthScale, translatedPoint);

        // 다시 지구 중심 좌표로 이동
        earth[i].x = translatedPoint.x + globalEarthPosX;
        earth[i].y = translatedPoint.y + globalEarthPosY;
    }

    // 직사각형 외곽선을 그리기
    for (int i = 0; i < 4; i++) {
        int x1 = (int)earth[i].x;   // 현재 점의 x 좌표
        int y1 = (int)earth[i].y;   // 현재 점의 y 좌표
        int x2 = (int)earth[(i + 1) % 4].x; // 다음 점의 x 좌표
        int y2 = (int)earth[(i + 1) % 4].y; // 다음 점의 y 좌표

        DrawLine(x1, y1, x2, y2, 'E'); // 직선으로 외곽선 그리기
    }

    // 각 행에 대해 'E' 사이를 채우기
    for (int y = 0; y < HEIGHT; y++) {
        int firstHash = -1;
        for (int x = 0; x < WIDTH; x++) {
            if (screenBuffer[y * (WIDTH + 1) + x] == 'E') {
                if (firstHash == -1) {
                    firstHash = x;
                }
                else {
                    // 'E' 사이에 있는 영역을 채움
                    for (int fillX = firstHash + 1; fillX < x; fillX++) {
                        screenBuffer[y * (WIDTH + 1) + fillX] = 'E';
                    }
                    firstHash = x; // 'E'의 마지막 위치로 업데이트
                }
            }
        }
    }
}

void drawMoon(float angle) {
    // 달의 초기 상대 위치 (지구 중심 기준)
    Vector3 moonPoint = { -1.5f, -4.0f, 1.0f }; // 지구 중심에서 6만큼 떨어짐

    // 달 공전 (지구 중심 기준)
    Matrix3x3 moonOrbitRotation = rotation_matrix(angle); // 공전 회전 행렬
    moonPoint = multiply_matrix_vector(moonOrbitRotation, moonPoint);

    // 달의 실제 중심 좌표 계산 (지구 중심 좌표 기준)
    float moonX = moonPoint.x + globalEarthPosX; // 지구 중심에서 이동
    float moonY = moonPoint.y + globalEarthPosY;

    // 정수 좌표로 변환
    int moonPosX = (int)moonX;
    int moonPosY = (int)moonY;

    // 화면 범위 내인지 확인하고 달 중심 출력
    if (moonPosX >= 0 && moonPosX < WIDTH && moonPosY >= 0 && moonPosY < HEIGHT) {
        screenBuffer[moonPosY * (WIDTH + 1) + moonPosX] = 'M'; // 중심에 'M' 출력
    }

    // 달을 구성하는 점들 (moon 벡터)
    Vector3 moon[6] = {
        {  1.0f, -1.0f, 1.0f }, // 첫 번째 줄: 0, -1
        {  3.0f, -1.0f, 1.0f }, // 첫 번째 줄: 3, -1

        { 0.0f,  0.0f, 1.0f }, // 두 번째 줄: -1, 0
        {  4.0f,  0.0f, 1.0f }, // 두 번째 줄: 4, 0

        {  1.0f,  1.0f, 1.0f }, // 세 번째 줄: 0, 1
        {  3.0f,  1.0f, 1.0f }  // 세 번째 줄: 3, 1
    };

    // 달 자전 및 스케일 적용 (달 중심 기준)
    Matrix3x3 moonRotation = rotation_matrix(angle);   // 자전 회전 행렬
    Matrix3x3 moonScale = scale_matrix(1.0f, 1.0f);    // Y축 비율을 줄이는 스케일 행렬

    for (int i = 0; i < 6; i++) {
        // 달 중심 기준으로 원점으로 이동
        Vector3 translatedPoint = {
            moon[i].x,
            moon[i].y,
            moon[i].z
        };

        // 자전 적용
        translatedPoint = multiply_matrix_vector(moonRotation, translatedPoint);

        // 스케일 적용
        translatedPoint = multiply_matrix_vector(moonScale, translatedPoint);

        // 다시 달 중심 좌표로 이동
        moon[i].x = translatedPoint.x + moonPosX;
        moon[i].y = translatedPoint.y + moonPosY;
    }

    // 직사각형 외곽선을 그리기
    for (int i = 0; i < 6; i++) {
        int x1 = (int)moon[i].x;   // 현재 점의 x 좌표
        int y1 = (int)moon[i].y;   // 현재 점의 y 좌표
        int x2 = (int)moon[(i + 1) % 6].x; // 다음 점의 x 좌표
        int y2 = (int)moon[(i + 1) % 6].y; // 다음 점의 y 좌표

        DrawLine(x1, y1, x2, y2, 'M'); // 직선으로 외곽선 그리기
    }

    // 각 행에 대해 'M' 사이를 채우기
    for (int y = 0; y < HEIGHT; y++) {
        int firstHash = -1;
        for (int x = 0; x < WIDTH; x++) {
            if (screenBuffer[y * (WIDTH + 1) + x] == 'M') {
                if (firstHash == -1) {
                    firstHash = x;
                }
                else {
                    // 'M' 사이에 있는 영역을 채움
                    for (int fillX = firstHash + 1; fillX < x; fillX++) {
                        screenBuffer[y * (WIDTH + 1) + fillX] = 'M';
                    }
                    firstHash = x; // 'M'의 마지막 위치로 업데이트
                }
            }
        }
    }
}



float angleS = 0; // 태양 자전

float angleE1 = 0; // 지구 공전
float angleE2 = 0; // 지구 자전

float angleM1 = 0; // 달 공전
float angleM2 = 0; // 달 자전

// 화면 업데이트 함수
void Update() {
    ClearScreen();
    drawSun(angleS);
    drawEarth(angleE1);
    drawMoon(angleS);

    angleS -= 120.0f * (1.0f / 60.0f);  // 반시계방향 3초당 1바퀴
    angleE1 += 120.0f * (1.0f / 60.0f); // 시계방향 3초당 1바퀴
    angleE2 += 180.0f * (1.0f / 60.0f); // 시계방향 2초당 1바퀴
    angleM1 -= 180.0f * (1.0f / 60.0f);  // 반시계방향 2초당 1바퀴
    angleM2 += 360.0f * (1.0f / 60.0f);  // 시계방향 1초당 1바퀴
}

void firstScreen() { //학번과 이름 출력
    COORD coord = { WIDTH / 2, HEIGHT / 2 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("20231364 이서연");
}

void secondScreen() {

    while(1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) {  // ESC 키가 눌리면 게임 종료
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
    // 프레임 타이머
    clock_t lastTime = clock();
    int fps = 60;
    double frameTime = 1000.0 / fps;


    while (1) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) {  // ESC 키가 눌리면 게임 종료
                exit(0);
            }
            else if (ch == ' ') {

                while (1) {

                    // 현재 시간 계산
                    clock_t now = clock();
                    int fps = 60;
                    double frameTime = 1000.0 / fps;

                    double deltaTime = (double)(now - lastTime) / CLOCKS_PER_SEC * 1000.0;

                    // 프레임 제한
                    if (deltaTime >= frameTime) {
                        lastTime = now;
  
                        COORD coord = { 0, 0 };
                        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

                        Update();

                        
                        Draw(screenBuffer);
                        
                    }

                    if (_kbhit()) {
                        char ch2 = _getch();
                        if (ch2 == 27) {  // ESC 키가 눌리면 게임 종료
                            exit(0);
                        }
                    }

                }

                
            }
        }
    }
}


// 게임 루프
int main() {
    Init(); // 화면 초기화

    firstScreen();
    secondScreen();
    thirdScreen();

    return 0;
}