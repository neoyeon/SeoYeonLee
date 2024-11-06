#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <string.h>

#define WIDTH 40   // 가로 크기
#define HEIGHT 20  // 세로 크기

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float m[3][3]; // 3x3 행렬
} Matrix3x3;

// 3x3 행렬 초기화
void initIdentityMatrix(Matrix3x3* mat) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat->m[i][j] = (i == j) ? 1 : 0;
        }
    }
}

// 회전 행렬 생성
Matrix3x3 createRotationMatrix(float angle) {
    Matrix3x3 mat;
    initIdentityMatrix(&mat);
    float radian = angle * M_PI / 180;
    mat.m[0][0] = cos(radian);
    mat.m[0][1] = -sin(radian);
    mat.m[1][0] = sin(radian);
    mat.m[1][1] = cos(radian);
    return mat;
}

// 비율 보정 행렬 생성
Matrix3x3 createScaleMatrix(float scaleX, float scaleY) {
    Matrix3x3 mat;
    initIdentityMatrix(&mat);
    mat.m[0][0] = scaleX;
    mat.m[1][1] = scaleY;
    return mat;
}

// vec3 구조체 정의
typedef struct {
    float x, y, w; // w는 동차좌표계
} vec3;

// vec3와 Matrix3x3의 곱셈
vec3 Mul(vec3 a, Matrix3x3 b) {
    vec3 result;
    result.x = a.x * b.m[0][0] + a.y * b.m[0][1] + a.w * b.m[0][2];
    result.y = a.x * b.m[1][0] + a.y * b.m[1][1] + a.w * b.m[1][2];
    result.w = a.x * b.m[2][0] + a.y * b.m[2][1] + a.w * b.m[2][2];
    return result;
}

// 콘솔 화면 지우기
void clearScreen() {
    system("cls");
}

// 두 점을 연결하여 '#'로 채우기
void drawLine(char grid[HEIGHT][WIDTH], int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xIncrement = (float)dx / (float)steps;
    float yIncrement = (float)dy / (float)steps;

    float x = x0;
    float y = y0;

    for (int i = 0; i <= steps; i++) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            grid[(int)y][(int)x] = '#';
        }
        x += xIncrement;
        y += yIncrement;
    }
}

// 화면 그리기
void draw(float angle) {
    clearScreen();

    // P 위치 (콘솔 중앙)
    int pX = WIDTH / 2;
    int pY = HEIGHT / 2;

    // 직사각형의 4개 점 (중심 기준, 변의 길이 11, 5)
    vec3 rect[4] = {
        { -5.5f, -2.5f, 1.0f }, // 왼쪽 위
        { -5.5f,  2.5f, 1.0f }, // 왼쪽 아래
        {  5.5f,  2.5f, 1.0f }, // 오른쪽 아래
        {  5.5f, -2.5f, 1.0f }  // 오른쪽 위
    };

    // 회전 및 비율 보정 적용
    Matrix3x3 rotation = createRotationMatrix(angle);
    Matrix3x3 scale = createScaleMatrix(1.0f, 0.5f); // Y축 비율을 줄여줌

    for (int i = 0; i < 4; i++) {
        rect[i] = Mul(rect[i], rotation); // 회전 적용
        rect[i] = Mul(rect[i], scale);    // 비율 보정 적용
    }

    // 화면 그리기 위한 그리드 초기화
    char grid[HEIGHT][WIDTH];
    memset(grid, ' ', sizeof(grid)); // 그리드를 빈 공백으로 초기화

    // P 출력
    grid[pY][pX] = 'P'; // P 출력

    // 직사각형의 점 표시 및 사이를 채우기
    for (int i = 0; i < 4; i++) {
        int x0 = (int)(rect[i].x + pX);
        int y0 = (int)(-rect[i].y + pY);
        int x1 = (int)(rect[(i + 1) % 4].x + pX);
        int y1 = (int)(-rect[(i + 1) % 4].y + pY);

        // 두 점을 연결하여 채우기
        drawLine(grid, x0, y0, x1, y1);
    }

    // 각 행에 대해 '#' 사이를 채우기
    for (int y = 0; y < HEIGHT; y++) {
        int firstHash = -1;
        for (int x = 0; x < WIDTH; x++) {
            if (grid[y][x] == '#') {
                if (firstHash == -1) {
                    firstHash = x;
                }
                else {
                    for (int fillX = firstHash + 1; fillX < x; fillX++) {
                        grid[y][fillX] = '#';
                    }
                    firstHash = x; // 업데이트
                }
            }
        }
    }

    // 그리드 출력
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            putchar(grid[y][x]);
        }
        putchar('\n');
    }
}

int main() {
    float totalTime = 10.0f; // 전체 실행 시간
    float rotationSpeed = 360.0f / 3.0f; // 3초에 한 바퀴 (각도)
    float angle = 0.0f;

    for (float elapsedTime = 0; elapsedTime < totalTime; elapsedTime += 0.1f) {
        draw(angle); // 회전 각도에 따라 그리기
        angle += rotationSpeed * 0.1f; // 0.1초마다 각도 증가
        Sleep(100); // 0.1초 대기
    }

    return 0;
}
