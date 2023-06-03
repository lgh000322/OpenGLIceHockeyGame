#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "gl/glut.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

float ball_rad;  // 공의 반지름
float ball_x, ball_y; //공의 초기 x좌표, 적구 초기 y좌표
float  ball_moveX, ball_moveY;  //공의 초기 x축 이동 속도, 적구 초기 y축 이동 속도
float p1_x, p1_y, p1_radius;//플레이어1 초기 설정 변수
float p1_moveX, p1_moveY;
float p2_moveX, p2_moveY;
float p2_x, p2_y, p2_radius;//플레이어2 초기 설정 변수
bool p2_A, p2_S, p2_D, p2_W;
int p1_score, p2_score;
float windowWidth = 1.0f;
float windowHeight = 2.0f;
bool isConnecetd = false;
WSADATA wsaData;
sockaddr_in serverAddress;
SOCKET clientSocket;

void initValue() {      // 초기화 함수
    ball_rad = 0.05f;
    p1_x = 0.0f;
    p1_y = -1.5f;
    p1_radius = 0.1f;
    p2_x = 0.0f;
    p2_y = 1.5f;
    p2_radius = 0.1f;
    p1_moveX = 0.08f;
    p1_moveY = 0.08f;
    p2_moveX = 0.08f;
    p2_moveY = 0.08f;
    p2_A = false; p2_D = false; p2_S = false; p2_W = false;
}

void ConnectServer() {

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "Failed to initialize winsock" << std::endl;
    }

    // 서버 주소 설정
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);  // 서버 포트 번호
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");  // 서버 IP 주소(루프백 주소)

    if (serverAddress.sin_addr.s_addr == INADDR_NONE)
    {
        std::cout << "Invalid server address" << std::endl;
        WSACleanup();
    }

    // 클라이언트 소켓 생성
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "Failed to create socket" << std::endl;
        WSACleanup();
    }

    // 서버에 연결
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cout << "Failed to connect to server" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
    }

    std::cout << "Connected to server" << std::endl;
}

void DisconnectServer() {
    closesocket(clientSocket);
    WSACleanup();
}

//서버로 데이터를 주고받는 함수
void SendAndReceive() {
    recv(clientSocket, (char*)&ball_x, sizeof(float), 0);
    recv(clientSocket, (char*)&ball_y, sizeof(float), 0);
    send(clientSocket, (char*)&p2_x, sizeof(float), 0);
    send(clientSocket, (char*)&p2_y, sizeof(float), 0);

    recv(clientSocket, (char*)&p1_x, sizeof(float), 0);
    recv(clientSocket, (char*)&p1_y, sizeof(float), 0);
    recv(clientSocket, (char*)&p1_score, sizeof(float), 0);
    recv(clientSocket, (char*)&p2_score, sizeof(float), 0);

}

void MyTimer(int Value) {
    SendAndReceive();
    glutPostRedisplay();
    glutTimerFunc(10, MyTimer, 1);      // 공위치 수정을 몇초마다 할건지를 정하는 함수. (현재 10ms로 설정.)
}

void keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        p2_W = false;
        break;
    case 'a':
        p2_A = false;
        break;
    case 's':
        p2_S = false;
        break;
    case 'd':
        p2_D = false;
        break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
        p2_W = true;
        break;
    case 'a':
        p2_A = true;
        break;
    case 's':
        p2_S = true;
        break;
    case 'd':
        p2_D = true;
        break;
    }
}

void drawTable() {
    // 테이블 상판
    glColor3f(1.0, 1.0, 1);
    glBegin(GL_QUADS);
    glVertex3f(-1, -2, 0);
    glVertex3f(1, -2, 0);
    glVertex3f(1, 2, 0);
    glVertex3f(-1, 2, 0);
    glEnd();

    glColor3f(1.0, 0.0, 0); //아래 골대
    glBegin(GL_QUADS);
    glVertex3f(-0.5, -2, 0.0001);
    glVertex3f(0.5, -2, 0.0001);
    glVertex3f(0.5, -1.9, 0.0001);
    glVertex3f(-0.5, -1.9, 0.0001);
    glEnd();

    glColor3f(1.0, 0.0, 0); //위 골대
    glBegin(GL_QUADS);
    glVertex3f(-0.5, 2, 0.0001);
    glVertex3f(0.5, 2, 0.0001);
    glVertex3f(0.5, 1.9, 0.0001);
    glVertex3f(-0.5, 1.9, 0.0001);
    glEnd();

    glColor3f(0.0, 0.0, 0); // 중앙선
    glBegin(GL_QUADS);
    glVertex3f(-1, -0.03, 0.0001);
    glVertex3f(1, -0.03, 0.0001);
    glVertex3f(1, 0.03, 0.0001);
    glVertex3f(-1, 0.03, 0.0001);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0001f); // 중심점

    for (int i = 0; i <= 36; ++i) {
        float angle = 2.0f * 3.14f * static_cast<float>(i) / static_cast<float>(36);
        float x = 0.1 * cos(angle);
        float y = 0.1 * sin(angle);
        glVertex3f(x, y, 0.0001f);
    }

    glEnd();

    //왼쪽 벽면
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glVertex3f(-1, -2, 0.01);
    glVertex3f(-1, -2, 0.11);
    glVertex3f(-1, 2, 0.11);
    glVertex3f(-1, 2, 0.01);
    glEnd();

    //오른쪽벽면
    glColor3f(1.0, 1.0, 1);
    glBegin(GL_QUADS);
    glVertex3f(1, -2, 0.01);
    glVertex3f(1, -2, 0.11);
    glVertex3f(1, 2, 0.11);
    glVertex3f(1, 2, 0.01);
    glEnd();

    glColor3f(0.75, 0.5, 0.25); //테이블 선
    glBegin(GL_LINE_LOOP);
    glVertex3f(-1, -2, 0);
    glVertex3f(1, -2, 0);
    glVertex3f(1, 2, 0);
    glVertex3f(-1, 2, 0);
    glEnd();
    if (p2_score == 5) {
        glColor4f(1.0, 1.0, 0, 0.5);
        glBegin(GL_QUADS);
        glVertex3f(-1, 0, 0.00001);
        glVertex3f(1, 0, 0.00001);
        glVertex3f(1, 2, 0.00001);
        glVertex3f(-1, 2, 0.00001);
        glEnd();
    }
    for (int i = 0; i < p1_score; i++) {
        glColor3f(1.0, 0.5, 1);
        glBegin(GL_QUADS);
        glVertex3f(0.5 + 0.105 * i, -2.3, 0.3);
        glVertex3f(0.5 + 0.105 * i, -2.3, 0.4);
        glVertex3f(0.6 + 0.105 * i, -2.3, 0.3);
        glVertex3f(0.6 + 0.105 * i, -2.3, 0.4);
        glEnd();
    }

    for (int i = 0; i < p2_score; i++) {
        glColor3f(1.0, 0.5, 1);
        glBegin(GL_QUADS);
        glVertex3f(0.5 + 0.105 * i, 2.1, 0.3);
        glVertex3f(0.5 + 0.105 * i, 2.1, 0.4);
        glVertex3f(0.6 + 0.105 * i, 2.1, 0.3);
        glVertex3f(0.6 + 0.105 * i, 2.1, 0.4);
        glEnd();
    }

  
}

void drawCircle(float radius, int numSegments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f); // 중심점

    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * 3.14f * static_cast<float>(i) / static_cast<float>(numSegments);
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        glVertex3f(x, y, 0.0f);
    }

    glEnd();
}

//공의 위치 ball_x,ball_y
void drawBall() {
    // 조명을 활성화합니다.
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // 빛의 위치와 색상을 설정합니다.
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };  // 빛의 위치 (x, y, z, w)
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // 빛의 색상 (R, G, B, A)
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    // 공의 재질을 설정합니다.
    GLfloat mat_diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };  // 재질의 색상 (R, G, B, A)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    glPushMatrix(); // 공의 기둥
    glTranslatef(ball_x, ball_y, 0.0f);
    GLUquadricObj* ball = gluNewQuadric();
    gluCylinder(ball, ball_rad, ball_rad, 0.05f, 36, 10);
    gluDeleteQuadric(ball); // quadric 객체 ball을 삭제해서 메모리 누수 방지
    glPopMatrix();

    glPushMatrix(); // 공의 뚜껑
    glTranslatef(ball_x, ball_y, 0.05f);
    drawCircle(ball_rad, 36);
    glPopMatrix();

    // 조명을 비활성화합니다.
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

}

void drawPlayer() {

    // 조명을 활성화합니다.
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // 빛의 위치와 색상을 설정합니다.
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };  // 빛의 위치 (x, y, z, w)
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // 빛의 색상 (R, G, B, A)
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    // 공의 재질을 설정합니다.
    GLfloat mat_diffuse_p1[] = { 0.0f, 1.0f, 0.0f, 1.0f };  // 플레이어 1의 재질 색상 (R, G, B, A)
    GLfloat mat_diffuse_p2[] = { 0.0f, 0.0f, 1.0f, 1.0f };  // 플레이어 2의 재질 색상 (R, G, B, A)

    // 플레이어 1 설정
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_p1);

    glPushMatrix(); // 플레이어 1 기둥
    glTranslatef(p1_x, p1_y, 0.0f);
    GLUquadricObj* player1 = gluNewQuadric();
    gluCylinder(player1, p1_radius, p1_radius, 0.05f, 36, 10);
    gluDeleteQuadric(player1);
    glPopMatrix();

    glPushMatrix(); // 플레이어 1 뚜껑
    glTranslatef(p1_x, p1_y, 0.05f);
    drawCircle(p1_radius, 36);
    glPopMatrix();

    // 플레이어 2 설정
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_p2);

    glPushMatrix(); // 플레이어 2 기둥
    glTranslatef(p2_x, p2_y, 0.0f);
    GLUquadricObj* player2 = gluNewQuadric();
    gluCylinder(player2, p2_radius, p2_radius, 0.05f, 36, 10);
    gluDeleteQuadric(player2);
    glPopMatrix();

    glPushMatrix(); // 플레이어 2 뚜껑
    glTranslatef(p2_x, p2_y, 0.05f);
    drawCircle(p2_radius, 36);
    glPopMatrix();

    // 조명을 비활성화합니다.
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

}

void setPoint() {
    if (p2_D && p2_x > -(windowWidth - 0.2)) p2_x -= p2_moveX;
    if (p2_A && p2_x < windowWidth - 0.2) p2_x += p2_moveX;
    if (p2_S && p2_y < windowHeight - 0.4) p2_y += p2_moveY;
    if (p2_W && p2_y > 1.0) p2_y -= p2_moveY;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 3.0, 1.5,  // 카메라 위치
        0.0, 0.0, 0.0,  // 카메라가 바라보는 곳의 위치
        0.0, -1.0, 0.0); // 카메라의 위쪽 방향
    setPoint();
    drawTable();
    drawPlayer();
    drawBall();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("클라1");
    glEnable(GL_DEPTH_TEST);
    initValue();
    //서버연결
    ConnectServer();
    while (!isConnecetd)
    {
        recv(clientSocket, (char*)&isConnecetd, sizeof(bool), 0);
    }
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(1000, MyTimer, 1);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMainLoop();
    return 0;
}
