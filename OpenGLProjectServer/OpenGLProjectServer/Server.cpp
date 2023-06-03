#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define MAX_CLIENTS 2

WSADATA wsaData;
SOCKET serverSocket;
SOCKET clientSockets[MAX_CLIENTS];

float ball_rad;  // ���� ������
float ball_x, ball_y; // ���� �ʱ� x��ǥ, ���� �ʱ� y��ǥ
float ball_moveX, ball_moveY;  // ���� �ʱ� x�� �̵� �ӵ�, ���� �ʱ� y�� �̵� �ӵ�
float ball_x2, ball_y2; // ���� �ʱ� x��ǥ, ���� �ʱ� y��ǥ
float p1_x, p1_y;
float p2_x, p2_y;
float p_radius;
int p1_score, p2_score; // ���ھ�


void InitValue() {
    ball_x = 0.0f;
    ball_y = 0.0f;
    ball_moveX = 0.08f;
    ball_moveY = 0.08f;
    ball_x2 = 0.0f;
    ball_y2 = 0.0f;
    ball_rad = 0.06f;
    p1_x = 0.0f;
    p1_y = -1.5f;
    p_radius = 0.1f;
    p2_x = 0.0f;
    p2_y = 1.5f;
    p1_score = 0;
    p2_score = 0;
}
void InitServer() {
    // Winsock �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Failed to initialize winsock\n");
    }

    // ���� �ּ� ����
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);  // ���� ��Ʈ ��ȣ
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0)
    {
        printf("Invalid server address\n");
        WSACleanup();
    }

    // ���� ���� ����
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        printf("Failed to create socket\n");
        WSACleanup();
    }

    // ���� ������ ���� �ּҿ� ���ε�
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Failed to bind socket\n");
        closesocket(serverSocket);
        WSACleanup();
    }

    // Ŭ���̾�Ʈ�� ���� ��û ���� ���
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Listen failed\n");
        closesocket(serverSocket);
        WSACleanup();
    }

    printf("Waiting for incoming connections...\n");

    // Ŭ���̾�Ʈ ���� ���� �迭
    int connectedClients = 0;

    while (connectedClients < MAX_CLIENTS)
    {
        // Ŭ���̾�Ʈ ���� ����
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET)
        {
            printf("Accept failed\n");
            closesocket(serverSocket);
            WSACleanup();
        }

        printf("Client connected\n");

        // ����� Ŭ���̾�Ʈ ������ �迭�� ����
        clientSockets[connectedClients] = clientSocket;
        connectedClients++;

        if (connectedClients == MAX_CLIENTS)
        {
            printf("Both clients connected. Starting data exchange.\n");
        }
    }
}

bool CheckCollision(float x1, float y1, float radius1, float x2, float y2, float radius2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < radius1 + radius2 + 0.02f;
}

void setPoint() {
    if (ball_x + ball_rad > 1.0f || ball_x - ball_rad < -1.0f) { // �� �浹 ����
        ball_moveX *= -1.0f;

    }
    if (ball_y + ball_rad > 2.0f || ball_y - ball_rad < -2.0f) {
        ball_moveY *= -1.0f;

    }

    if (CheckCollision(ball_x, ball_y, ball_rad, p1_x, p1_y, p_radius + 0.1)) { // ��Ű ä �浹����
        ball_moveY *= -1.0f;
        ball_y += ball_moveY;
    }

    if (CheckCollision(ball_x, ball_y, ball_rad, -p2_x, -p2_y, p_radius + 0.1)) {
        ball_moveY *= -1.0f;
        ball_y += ball_moveY;
    }
    if (ball_y + ball_rad > 2.0f && ball_x > -0.5 && ball_x < 0.5) { //��� �浹 ����
        ball_x = 0.0f;
        ball_y = 0.0f;
        ball_moveX = 0.08f;
        ball_moveY = 0.08f;
        p1_score++;
    }

    if (ball_y - ball_rad < -2.0f && ball_x > -0.5 && ball_x < 0.5) {
        ball_x = 0.0f;
        ball_y = 0.0f;
        ball_moveX = 0.08f;
        ball_moveY = 0.08f;
        p2_score++;
    }
    if (p1_score == 5) { //���ھ� �˻�
        closesocket(clientSockets[0]);
        ball_x = 0.0f;
        ball_y = 0.0f;
        ball_moveX = 0.0f;
        ball_moveY = 0.0f;
    }
    else if (p2_score == 5) {
        closesocket(clientSockets[1]);
        ball_x = 0.0f;
        ball_y = 0.0f;
        ball_moveX = 0.0f;
        ball_moveY = 0.0f;
    }
    ball_x += ball_moveX;
    ball_y += ball_moveY;

    ball_x2 = -ball_x;
    ball_y2 = -ball_y;

}

void SendDataToClients() {
    while (1) {
        float ball_x_send = ball_x;
        float ball_y_send = ball_y;
        float ball_x2_send = ball_x2;
        float ball_y2_send = ball_y2;
        float player1_x_recv;
        float player1_y_recv;
        float player2_x_recv;
        float player2_y_recv;
        float player1_x_send;
        float player1_y_send;
        float player2_x_send;
        float player2_y_send;

        // Ŭ���̾�Ʈ 0��°���� �״�� �����͸� �����ϴ�.
        send(clientSockets[0], (char*)&ball_x_send, sizeof(float), 0);
        send(clientSockets[0], (char*)&ball_y_send, sizeof(float), 0);
        recv(clientSockets[0], (char*)&player1_x_recv, sizeof(float), 0);
        recv(clientSockets[0], (char*)&player1_y_recv, sizeof(float), 0);
        //1��° Ŭ���̾�Ʈ ������ �÷��̾�2�� ��ġ��ǥ
        p1_x = player1_x_recv;
        p1_y = player1_y_recv;
        player1_x_send = -p1_x;
        player1_y_send = -p1_y;
        send(clientSockets[1], (char*)&player1_x_send, sizeof(float), 0);
        send(clientSockets[1], (char*)&player1_y_send, sizeof(float), 0);
        // Ŭ���̾�Ʈ�� ������ ���� ��, ���� �ð� ���
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        // Ŭ���̾�Ʈ 1��°���� -1�� ���� �����͸� �����ϴ�.
        ball_x_send *= -1;
        ball_y_send *= -1;

        send(clientSockets[1], (char*)&ball_x_send, sizeof(float), 0);
        send(clientSockets[1], (char*)&ball_y_send, sizeof(float), 0);
        recv(clientSockets[1], (char*)&player2_x_recv, sizeof(float), 0);
        recv(clientSockets[1], (char*)&player2_y_recv, sizeof(float), 0);
        //2��° Ŭ���̾�Ʈ ������ �÷��̾�2�� ��ġ��ǥ
        p2_x = player2_x_recv;
        p2_y = player2_y_recv;
        player2_x_send = -p2_x;
        player2_y_send = -p2_y;
        send(clientSockets[0], (char*)&player2_x_send, sizeof(float), 0);
        send(clientSockets[0], (char*)&player2_y_send, sizeof(float), 0);

        send(clientSockets[0], (char*)&p1_score, sizeof(float), 0);
        send(clientSockets[0], (char*)&p2_score, sizeof(float), 0);
        send(clientSockets[1], (char*)&p1_score, sizeof(float), 0);
        send(clientSockets[1], (char*)&p2_score, sizeof(float), 0);
        setPoint();


    }
}

int main()
{
    InitValue();
    InitServer();
    bool flag = true;
    send(clientSockets[0], (char*)&flag, sizeof(bool), 0);
    send(clientSockets[1], (char*)&flag, sizeof(bool), 0);

    std::thread sendDataThread(SendDataToClients);

    // ��� �� ������ ����
    sendDataThread.join();

    // ���� ����
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        closesocket(clientSockets[i]);
    }
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
