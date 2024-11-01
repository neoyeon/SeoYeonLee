#include <stdio.h>
#include <math.h>
#include <windows.h>

#define WIDTH 40
#define HEIGHT 20

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
            if (i == j) {
                mat->m[i][j] = 1;
            }
            else {
                mat->m[i][j] = 0;
            }
        }
    }
}

// 회전 행렬 생성
Matrix3x3 createRotationMatrix(float angle) {
    Matrix3x3 mat;
    initIdentityMatrix(&mat);
    float radian = angle * M_PI / 180;
    mat.m[0][0] = cos(radian); // cos(각도)
    mat.m[0][1] = -sin(radian); // -sin(각도)
    mat.m[1][0] = sin(radian); // sin(각도)
    mat.m[1][1] = cos(radian); // cos(각도)
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

// 화면 그리기
void draw(float angle) {
    clearScreen();

    // P 위치
    int pX = WIDTH / 2;
    int pY = HEIGHT / 2;

    // O의 초기 위치 (P와의 거리)
    vec3 o = { 0.0f, 7.0f, 1.0f }; // O의 초기 위치 (P에서 7만큼 위)

    // O의 위치 계산 (P를 중심으로 회전)
    Matrix3x3 rotation = createRotationMatrix(angle);
    o = Mul(o, rotation); // O 점 회전

    // O 점을 P 점으로 이동
    float oX = 2 * o.x + pX; // 2를 곱하여 x 위치 조정
    float oY = o.y + pY;

    // . 점의 위치 계산 (O를 중심으로 회전)
    float dotRadius = 2.0f; // . 점과 O 점 사이의 거리
    vec3 dot = { dotRadius, 0.0f, 1.0f }; // O 점을 기준으로 한 상대 위치

    // dot 점의 회전 각도
    float dotAngle = angle * 3; // dot 점이 O 점 주위를 좀 더 빨리 돌도록 3배로 설정

    // . 점의 위치 변환
    Matrix3x3 dotRotation = createRotationMatrix(dotAngle);
    dot = Mul(dot, dotRotation); // . 점 회전

    // . 점을 O 점의 위치에 추가
    float dotX = dot.x + oX;
    float dotY = dot.y + oY;

    // 화면 그리기
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == pX && y == pY) {
                printf("P"); // P 출력
            }
            else if ((int)oX == x && (int)oY == y) {
                printf("O"); // O 출력
            }
            else if ((int)dotX == x && (int)dotY == y) {
                printf("."); // . 점 출력
            }
            else {
                printf(" "); // 빈 공간 출력
            }
        }
        printf("\n");
    }
}

int main() {
    for (float angle = 0; angle < 360; angle += 5) {
        draw(angle); // 회전 각도에 따라 그리기
        Sleep(100); // 0.1초 대기
    }
    return 0;
}
