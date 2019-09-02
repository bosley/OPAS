#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int socket_desc, new_socket, c;
    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1)
    {
        printf("Could not create socket");
    }

    // Create sockaddr_in struct
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind
    if( bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        puts("Bind failed");
    }

    puts("Bind done");

    // Listen
    listen(socket_desc, 3);

    // Accept 
    puts("Waiting for connections..");

    c = sizeof(struct sockaddr_in);

    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c) ))
    {

        printf("Connection accepted: %s, %i", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        char * message = "Hello client";
        write(new_socket, message, strlen(message));
    }

    if(new_socket < 0)
    {
        perror("Accept failed");
        return 1;
    }
    return 0;
}
