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

float ball_rad;  // ���� ������
float ball_x, ball_y; //���� �ʱ� x��ǥ, ���� �ʱ� y��ǥ
float  ball_moveX, ball_moveY;  //���� �ʱ� x�� �̵� �ӵ�, ���� �ʱ� y�� �̵� �ӵ�
float p1_x, p1_y, p1_radius;//�÷��̾�1 �ʱ� ���� ����
float p1_moveX, p1_moveY;
float p2_moveX, p2_moveY;
float p2_x, p2_y, p2_radius;//�÷��̾�2 �ʱ� ���� ����
bool p2_A, p2_S, p2_D, p2_W;
int p1_score, p2_score;
float windowWidth = 1.0f;
float windowHeight = 2.0f;
bool isConnecetd = false;
WSADATA wsaData;
sockaddr_in serverAddress;
SOCKET clientSocket;

void initValue() {      // �ʱ�ȭ �Լ�
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

    // ���� �ּ� ����
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);  // ���� ��Ʈ ��ȣ
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");  // ���� IP �ּ�(������ �ּ�)

    if (serverAddress.sin_addr.s_addr == INADDR_NONE)
    {
        std::cout << "Invalid server address" << std::endl;
        WSACleanup();
    }

    // Ŭ���̾�Ʈ ���� ����
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "Failed to create socket" << std::endl;
        WSACleanup();
    }

    // ������ ����
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

//������ �����͸� �ְ�޴� �Լ�
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
    glutTimerFunc(10, MyTimer, 1);      // ����ġ ������ ���ʸ��� �Ұ����� ���ϴ� �Լ�. (���� 10ms�� ����.)
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
    // ���̺� ����
    glColor3f(1.0, 1.0, 1);
    glBegin(GL_QUADS);
    glVertex3f(-1, -2, 0);
    glVertex3f(1, -2, 0);
    glVertex3f(1, 2, 0);
    glVertex3f(-1, 2, 0);
    glEnd();

    glColor3f(1.0, 0.0, 0); //�Ʒ� ���
    glBegin(GL_QUADS);
    glVertex3f(-0.5, -2, 0.0001);
    glVertex3f(0.5, -2, 0.0001);
    glVertex3f(0.5, -1.9, 0.0001);
    glVertex3f(-0.5, -1.9, 0.0001);
    glEnd();

    glColor3f(1.0, 0.0, 0); //�� ���
    glBegin(GL_QUADS);
    glVertex3f(-0.5, 2, 0.0001);
    glVertex3f(0.5, 2, 0.0001);
    glVertex3f(0.5, 1.9, 0.0001);
    glVertex3f(-0.5, 1.9, 0.0001);
    glEnd();

    glColor3f(0.0, 0.0, 0); // �߾Ӽ�
    glBegin(GL_QUADS);
    glVertex3f(-1, -0.03, 0.0001);
    glVertex3f(1, -0.03, 0.0001);
    glVertex3f(1, 0.03, 0.0001);
    glVertex3f(-1, 0.03, 0.0001);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0001f); // �߽���

    for (int i = 0; i <= 36; ++i) {
        float angle = 2.0f * 3.14f * static_cast<float>(i) / static_cast<float>(36);
        float x = 0.1 * cos(angle);
        float y = 0.1 * sin(angle);
        glVertex3f(x, y, 0.0001f);
    }

    glEnd();

    //���� ����
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glVertex3f(-1, -2, 0.01);
    glVertex3f(-1, -2, 0.11);
    glVertex3f(-1, 2, 0.11);
    glVertex3f(-1, 2, 0.01);
    glEnd();

    //�����ʺ���
    glColor3f(1.0, 1.0, 1);
    glBegin(GL_QUADS);
    glVertex3f(1, -2, 0.01);
    glVertex3f(1, -2, 0.11);
    glVertex3f(1, 2, 0.11);
    glVertex3f(1, 2, 0.01);
    glEnd();

    glColor3f(0.75, 0.5, 0.25); //���̺� ��
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
    glVertex3f(0.0f, 0.0f, 0.0f); // �߽���

    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * 3.14f * static_cast<float>(i) / static_cast<float>(numSegments);
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        glVertex3f(x, y, 0.0f);
    }

    glEnd();
}

//���� ��ġ ball_x,ball_y
void drawBall() {
    // ������ Ȱ��ȭ�մϴ�.
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // ���� ��ġ�� ������ �����մϴ�.
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };  // ���� ��ġ (x, y, z, w)
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // ���� ���� (R, G, B, A)
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    // ���� ������ �����մϴ�.
    GLfloat mat_diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };  // ������ ���� (R, G, B, A)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    glPushMatrix(); // ���� ���
    glTranslatef(ball_x, ball_y, 0.0f);
    GLUquadricObj* ball = gluNewQuadric();
    gluCylinder(ball, ball_rad, ball_rad, 0.05f, 36, 10);
    gluDeleteQuadric(ball); // quadric ��ü ball�� �����ؼ� �޸� ���� ����
    glPopMatrix();

    glPushMatrix(); // ���� �Ѳ�
    glTranslatef(ball_x, ball_y, 0.05f);
    drawCircle(ball_rad, 36);
    glPopMatrix();

    // ������ ��Ȱ��ȭ�մϴ�.
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

}

void drawPlayer() {

    // ������ Ȱ��ȭ�մϴ�.
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // ���� ��ġ�� ������ �����մϴ�.
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };  // ���� ��ġ (x, y, z, w)
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // ���� ���� (R, G, B, A)
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    // ���� ������ �����մϴ�.
    GLfloat mat_diffuse_p1[] = { 0.0f, 1.0f, 0.0f, 1.0f };  // �÷��̾� 1�� ���� ���� (R, G, B, A)
    GLfloat mat_diffuse_p2[] = { 0.0f, 0.0f, 1.0f, 1.0f };  // �÷��̾� 2�� ���� ���� (R, G, B, A)

    // �÷��̾� 1 ����
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_p1);

    glPushMatrix(); // �÷��̾� 1 ���
    glTranslatef(p1_x, p1_y, 0.0f);
    GLUquadricObj* player1 = gluNewQuadric();
    gluCylinder(player1, p1_radius, p1_radius, 0.05f, 36, 10);
    gluDeleteQuadric(player1);
    glPopMatrix();

    glPushMatrix(); // �÷��̾� 1 �Ѳ�
    glTranslatef(p1_x, p1_y, 0.05f);
    drawCircle(p1_radius, 36);
    glPopMatrix();

    // �÷��̾� 2 ����
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_p2);

    glPushMatrix(); // �÷��̾� 2 ���
    glTranslatef(p2_x, p2_y, 0.0f);
    GLUquadricObj* player2 = gluNewQuadric();
    gluCylinder(player2, p2_radius, p2_radius, 0.05f, 36, 10);
    gluDeleteQuadric(player2);
    glPopMatrix();

    glPushMatrix(); // �÷��̾� 2 �Ѳ�
    glTranslatef(p2_x, p2_y, 0.05f);
    drawCircle(p2_radius, 36);
    glPopMatrix();

    // ������ ��Ȱ��ȭ�մϴ�.
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
    gluLookAt(0.0, 3.0, 1.5,  // ī�޶� ��ġ
        0.0, 0.0, 0.0,  // ī�޶� �ٶ󺸴� ���� ��ġ
        0.0, -1.0, 0.0); // ī�޶��� ���� ����
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
    glutCreateWindow("Ŭ��1");
    glEnable(GL_DEPTH_TEST);
    initValue();
    //��������
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
