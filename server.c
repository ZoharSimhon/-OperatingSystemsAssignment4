#include "headers.h"

int createServerSocket()
{
    struct sockaddr_in serverAddressIPv4;
    int serverSocket;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // If the socket is not established the method sock() return the value -1 (INVALID_SOCKET)
    if (serverSocket == -1)
    {
        perror("socket");
        return -1;
    }

    // Often the activation of the method bind() falls with the message "Address already in use".
    int yes = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    {
        perror("setsockopt");
        return -1;
    }

    int bindResult = 0;

    memset(&serverAddressIPv4, 0, sizeof(serverAddressIPv4));
    serverAddressIPv4.sin_family = AF_INET;
    serverAddressIPv4.sin_addr.s_addr = INADDR_ANY;
    serverAddressIPv4.sin_port = htons(PORT);
    bindResult = bind(serverSocket, (struct sockaddr *)&serverAddressIPv4, sizeof(serverAddressIPv4));

    // On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
    if (bindResult == -1)
    {
        perror("bind() failed");
        return -1;
    }

    if (listen(serverSocket, 1) == -1)
    {
        perror("listen() failed");
        return -1;
    }
    printf("The server is listening\n");
    return serverSocket;
}

int got_client_input(int socket)
{
    char buffer[MESSAGE_LENGTH];
    int bytesReceived = recv(socket, buffer, MESSAGE_LENGTH, 0);
    if (bytesReceived <= 0)
    {
        perror("recv");
        return -1;
    }
    buffer[bytesReceived] = '\0';
    printf("\nmessage received: %s\n", buffer);
    return 0;
}

int main()
{
    // createServer
    int listener = createServerSocket();
    if (listener == -1)
    {
        return -1;
    }
    printf("listener sockfd is: %d\n", listener);

    // create reactor thread
    preactor thisReactor = (preactor)createReactor();
    if (!thisReactor)
    {
        return -1;
    }
    //start reactor
    startReactor(thisReactor);

    // define client parametrs
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    // while true
    while (1)
    {
        memset(&clientAddress, 0, sizeof(clientAddress));
        // client = accept
        int clientSocket = accept(listener, (struct sockaddr *)&clientAddress, &clientAddressLen);
        ////If there is an error the method accept() returns the value -1.
        if (clientSocket == -1)
        {
            perror("accept");
            return -1;
        }
        printf("The server accept client connection: %d\n", clientSocket);
        // addFD client to reactor
        addFd(thisReactor, clientSocket, got_client_input);
    }

    return 0;
}