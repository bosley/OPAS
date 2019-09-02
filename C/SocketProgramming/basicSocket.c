#include <stdio.h>
#include <sys/socket.h>
#include<arpa/inet.h>   //inet_addr

#include <string.h>
#include <unistd.h>

int main()
{

    int socket_desc;

    //                   IP V4  , TCP        , IPPROTO_IP -> IP Protocol
    //                            SOCK_DGRAM = UDP
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_desc == -1)
    {
        printf("Could not create socket");
    }

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("172.217.4.206");
    server.sin_family = AF_INET;
    server.sin_port = htons(80);


    // Connect
    if(connect(socket_desc , (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Connect error");
        return 1;
    }
    puts("Connected");


    // Send
    char *message;
    message = "GET / HTTP/1.1\r\n\r\n";

    if(send(socket_desc, message, strlen(message), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    puts("Data Send");

    // Receive Datas
    char server_reply[2000];
    if(recv(socket_desc, server_reply, 2000, 0) < 0)
    {
        puts("recv fail");
    }
    puts("Reply received\n");
    puts(server_reply);

    /*
     * Sending is the same as writing to a file, so write() can be used, 
     * similarly receiving is the same as reading from a file so read() can be used
     */

    close(socket_desc);

    return 0;
}
