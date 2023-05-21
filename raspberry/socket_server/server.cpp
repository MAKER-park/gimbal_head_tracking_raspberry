#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstring>

void receiveMessage(int clientSocket) {
    char buffer[1024] = {0};
    while (true) {
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            std::cout << "Client: " << buffer << std::endl;
            memset(buffer, 0, sizeof(buffer)); // 버퍼를 비웁니다.
        } else if (bytesRead == 0) {
            std::cout << "Client disconnected." << std::endl;
            break; // 클라이언트가 연결을 끊으면 반복문을 빠져나갑니다.
        } else {
            perror("receive failed");
            break;
        }
    }
}

void sendMessage(int clientSocket) {
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "/quit") {
            break;
        }
        send(clientSocket, message.c_str(), message.length(), 0);
    }
}

int main(int argc, char* argv[]) {
    int serverSocket, clientSocket, PORT;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <PORT>" << std::endl;
        exit(1);
    }

    PORT = atoi(argv[1]);

    // 소켓 생성
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT); // 채팅 포트 설정

    // 소켓 바인딩
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 클라이언트의 연결 요청 대기
    if (listen(serverSocket, 1) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << PORT << " ..." << std::endl;

    while (true) {
        // 클라이언트 연결 수락
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "Client connected." << std::endl;

        // 수신 및 송신 스레드 생성
        std::thread receiverThread(receiveMessage, clientSocket);
        std::thread senderThread(sendMessage, clientSocket);

        // 스레드 분리
        receiverThread.detach();
        senderThread.detach();

        // 클라이언트 소켓 닫기는 main 스레드에서 처리되므로 여기서는 생략합니다.

        // 새로운 클라이언트의 연결을 기다림
        std::cout << "Waiting for a new client connection..." << std::endl;
    }

    // 소켓 닫기
    close(serverSocket);

    return 0;
}
