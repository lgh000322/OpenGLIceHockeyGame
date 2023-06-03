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

float ball_rad;  // 공의 반지름
float ball_x, ball_y; // 공의 초기 x좌표, 적구 초기 y좌표
float ball_moveX, ball_moveY;  // 공의 초기 x축 이동 속도, 적구 초기 y축 이동 속도
float ball_x2, ball_y2; // 공의 초기 x좌표, 적구 초기 y좌표
float p1_x, p1_y;
float p2_x, p2_y;
float p_radius;
int p1_score, p2_score; // 스코어


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
    // Winsock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Failed to initialize winsock\n");
    }

    // 서버 주소 설정
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);  // 서버 포트 번호
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0)
    {
        printf("Invalid server address\n");
        WSACleanup();
    }

    // 서버 소켓 생성
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        printf("Failed to create socket\n");
        WSACleanup();
    }

    // 서버 소켓을 서버 주소와 바인딩
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Failed to bind socket\n");
        closesocket(serverSocket);
        WSACleanup();
    }

    // 클라이언트의 연결 요청 수락 대기
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Listen failed\n");
        closesocket(serverSocket);
        WSACleanup();
    }

    printf("Waiting for incoming connections...\n");

    // 클라이언트 소켓 저장 배열
    int connectedClients = 0;

    while (connectedClients < MAX_CLIENTS)
    {
        // 클라이언트 연결 수락
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET)
        {
            printf("Accept failed\n");
            closesocket(serverSocket);
            WSACleanup();
        }

        printf("Client connected\n");

        // 연결된 클라이언트 소켓을 배열에 저장
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
    if (ball_x + ball_rad > 1.0f || ball_x - ball_rad < -1.0f) { // 벽 충돌 감지
        ball_moveX *= -1.0f;

    }
    if (ball_y + ball_rad > 2.0f || ball_y - ball_rad < -2.0f) {
        ball_moveY *= -1.0f;

    }

    if (CheckCollision(ball_x, ball_y, ball_rad, p1_x, p1_y, p_radius + 0.1)) { // 하키 채 충돌감지
        ball_moveY *= -1.0f;
        ball_y += ball_moveY;
    }

    if (CheckCollision(ball_x, ball_y, ball_rad, -p2_x, -p2_y, p_radius + 0.1)) {
        ball_moveY *= -1.0f;
        ball_y += ball_moveY;
    }
    if (ball_y + ball_rad > 2.0f && ball_x > -0.5 && ball_x < 0.5) { //골대 충돌 감지
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
    if (p1_score == 5) { //스코어 검사
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

        // 클라이언트 0번째에는 그대로 데이터를 보냅니다.
        send(clientSockets[0], (char*)&ball_x_send, sizeof(float), 0);
        send(clientSockets[0], (char*)&ball_y_send, sizeof(float), 0);
        recv(clientSockets[0], (char*)&player1_x_recv, sizeof(float), 0);
        recv(clientSockets[0], (char*)&player1_y_recv, sizeof(float), 0);
        //1번째 클라이언트 소켓의 플레이어2의 위치좌표
        p1_x = player1_x_recv;
        p1_y = player1_y_recv;
        player1_x_send = -p1_x;
        player1_y_send = -p1_y;
        send(clientSockets[1], (char*)&player1_x_send, sizeof(float), 0);
        send(clientSockets[1], (char*)&player1_y_send, sizeof(float), 0);
        // 클라이언트로 데이터 전송 후, 일정 시간 대기
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        // 클라이언트 1번째에는 -1을 곱한 데이터를 보냅니다.
        ball_x_send *= -1;
        ball_y_send *= -1;

        send(clientSockets[1], (char*)&ball_x_send, sizeof(float), 0);
        send(clientSockets[1], (char*)&ball_y_send, sizeof(float), 0);
        recv(clientSockets[1], (char*)&player2_x_recv, sizeof(float), 0);
        recv(clientSockets[1], (char*)&player2_y_recv, sizeof(float), 0);
        //2번째 클라이언트 소켓의 플레이어2의 위치좌표
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

    // 대기 및 스레드 종료
    sendDataThread.join();

    // 연결 종료
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        closesocket(clientSockets[i]);
    }
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
