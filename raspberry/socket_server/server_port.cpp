#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);

    int serverSocket, newSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[1024] = {0};

    // 소켓 생성
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 소켓 옵션 설정
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

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

    // 클라이언트 연결 수락
    if ((newSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // 클라이언트로부터 메시지 수신
    int bytesRead = read(newSocket, buffer, sizeof(buffer));
    std::cout << "Client: " << buffer << std::endl;

    // 클라이언트로 메시지 전송
    std::string message = "Hello from server";
    send(newSocket, message.c_str(), message.length(), 0);
    std::cout << "Server: " << message << std::endl;

    close(newSocket);
    close(serverSocket);

    return 0;
}
