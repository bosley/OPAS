#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>  //hostent
#include <arpa/inet.h>

int main()
{
    char *hostname = "www.google.com";
    char ip[100];
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname( hostname )) == NULL)
    {
        herror("gethostbyname");
        return 1;
    }

    // Get list of addresses from name server (h_addr_list)
    addr_list = (struct in_addr **) he->h_addr_list;

    for(int i = 0; addr_list[i] != NULL; i++)
    {
                   // convert long into dotted IP (inet_ntoa)
        strcpy(ip, inet_ntoa(*addr_list[i]));
    }

    printf("%s resolved to: %s", hostname, ip);
    return 0;
}
