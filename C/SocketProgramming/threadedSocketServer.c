#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void *connection_handler(void *);

int main()
{
    int socket_desc, new_socket, c, *new_sock;
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

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

        if(pthread_create(&sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0)
        {
            perror("Could not create thread");
            return 1;
        }

//        pthread_join( sniffer_thread, NULL);
        puts("Handler assigned");

    }

    if(new_socket < 0)
    {
        perror("Accept failed");
        return 1;
    }
    return 0;
}

void *connection_handler(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    int read_size;
    char *message, client_message[2000];

    message = "I AM YOUR CONNECTION HANDLER";

    write(sock, message, strlen(message));

    message = "... THERE ARE MANY LIKE ME, BUT THIS ONE IS YOURS. Tell me something..\n";

    write(sock, message, strlen(message));

    while( (read_size = recv(sock, client_message, 2000, 0)) > 0)
    {
        write(sock, client_message, strlen(client_message));
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else
    {
        perror("Recv failed");
    }


    free(socket_desc);

    return 0;
}
