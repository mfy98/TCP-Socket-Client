#include <iostream>
#include <cstring>
#include <cstdlib>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <string>


using namespace std;

const int PORT = 12345;


void userLogin(SOCKET clientSocket) {
    char username[100];
    char password[100];

    cout << "Kullanici Adi: ";
    cin >> username;
    send(clientSocket, username, sizeof(username), 0);

    cout << "Sifre: ";
    cin >> password;
    send(clientSocket, password, sizeof(password), 0);

    char serverResponse[100];
    recv(clientSocket, serverResponse, sizeof(serverResponse), 0);

    if (strcmp(serverResponse, "Hosgeldin") == 0) {
        cout << "Giris basarili. Hosgeldiniz!" << endl;
    }
    else {
        cout << "Hatali giris. Program kapatiliyor." << endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup basarisiz oldu." << endl;
        exit(EXIT_FAILURE);
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Socket olusturma hatasi: " << WSAGetLastError() << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    //serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); bunda hata aldım inet_pton denedim
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Sunucuya baglanilamadi.: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    userLogin(clientSocket);

    while (true) {

        cout << "Menu:\n1. Bakiye Goster\n2. Para Cek\n3. Para Gonder\n4. Para Yatir\n5.Cikis\nSeciminiz: ";
        int choice;
        cin >> choice;

        if (choice < 1 || choice > 5) {
            cout << "Hatali secim. Tekrar deneyin." << endl;
            continue;
        }

        string choiceStr = to_string(choice);
        send(clientSocket, choiceStr.c_str(), choiceStr.size() + 1, 0);

        if (choice == 5) {
            cout << "Programdan cikiliyor." << endl;
            break;
        }
        else if (choice == 2 || choice == 3 || choice == 4) {

            if (choice == 3) {
                char recipientAccountNumber[4];
                cout << "Alici Hesap Numarasi: ";
                cin >> recipientAccountNumber;
                send(clientSocket, recipientAccountNumber, sizeof(recipientAccountNumber), 0);
            }
            int amount;
            cout << "Miktar: ";
            cin >> amount;
            send(clientSocket, (char*)&amount, sizeof(amount), 0);
        }

        char serverMessage[100];
        recv(clientSocket, serverMessage, sizeof(serverMessage), 0);
        cout << "Sunucu yaniti ---> " << serverMessage << endl;
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
