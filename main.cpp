#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>


using namespace std;

#define PORT  9000
struct sockaddr_in srv;
fd_set fr, fw, fe;
int nSocket;
sockaddr_in client;
socklen_t clientSize = sizeof(client);

char clientHost[NI_MAXHOST];
char clientService[NI_MAXSERV];


void processClientMessage(int clientSocket) {
    cout << "Processing client message for client: " << clientSocket << " .... " << endl;
    char messageBuffer[256 + 1] = {0, };
    int bytesReceived = recv(clientSocket, messageBuffer, 256, 0);

    if (bytesReceived == 0) {
        cout << "An error occurred while reading client buffer" << endl;
        close(clientSocket);
        exit(EXIT_FAILURE);
    } else {
        cout << "Client says > " << messageBuffer << endl;
        send(clientSocket, messageBuffer, 256, 0);
    }
}

void processRequest() {
    if(FD_ISSET(nSocket, &fr)) {
        int nClientSocket = accept(nSocket, (sockaddr*)&client, &clientSize);

        if (nClientSocket > 0) {
            int clientInfo = getnameinfo((sockaddr*)&client, sizeof(client), clientHost, NI_MAXHOST, clientService, NI_MAXSERV, 0);

            if (clientInfo == 0) {
                cout << clientHost << " connected on port " << clientService << endl;
            } else {
                inet_ntop(AF_INET, &client.sin_addr, clientHost, NI_MAXHOST);
                cout << clientHost << " connected on port " << clientService << endl;
            }
            send(nClientSocket, "Client/Server Connection Established", 255, 0);
            close(nSocket);
            while (true)
                processClientMessage(nClientSocket);
        } else {
            cout << "Failure " << nClientSocket << endl;
            exit(EXIT_FAILURE);
        }
    }
}


int main() {
    cout << "Hello, Socket Programming" << endl;
    int nRet;

    // step 1: init socket
     nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (nSocket < 0) {
        cout <<"Err!! Socket not connected!!! "<< nSocket <<endl;
        exit(EXIT_FAILURE);
    } else {
        cout <<"Socket connected Successfully: "<< nSocket <<endl;
    }

    // step 2: init env for socket address
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(srv.sin_zero), 0, 8);


    // step 3: bind socket to local port
    nRet = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));
    if (nRet < 0) {
        cout << "Unable to bind to " << PORT << endl;
        cout<< nRet << endl;
        exit(EXIT_FAILURE);
    } else {
        cout << "Socket bound to " << PORT << " Successfully!!!"<< endl;
    }

    nRet = listen(nSocket, SOMAXCONN);


    if(nRet < 0 ) {
        cout << "Failed to start listen to local port" <<endl;
        exit(EXIT_FAILURE);
    } else {
        cout << "Listening on PORT: "<< PORT <<endl;
    }

    int nMaxFd = nSocket;
    struct timeval tv{};
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while(true) {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);

        FD_SET(nSocket, &fr);
        FD_SET(nSocket, &fe);
        // step 5: keep waiting for new request and process them
        nRet = select(nMaxFd + 1, &fr, &fw, &fe, &tv);

        if (nRet > 0) {
            cout <<"Connection made"<< endl;
            processRequest();
        } else if (nRet == 0) {
            cout <<"No Connections available"<< endl;
            sleep(5);
        } else {
            cout<<"An error occurred!!"<< &fe << endl;
            exit(EXIT_FAILURE);
        }

    }

}
