#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <pthread.h>

typedef struct server_information
{
    int fd;
    char* buffer;
}SL;


//服务端端进行读操作
/*
    fd:服务端socket
    read_buffer:读操作的缓冲区
*/
void* server_read(void* arg)
{
    SL* tt=(SL*)arg;
    while(1)
    {
        memset(tt->buffer,'\0',sizeof(tt->buffer));
        int ret=read(tt->fd,tt->buffer,sizeof(tt->buffer));
        if(ret==-1)
        {
            perror("server read false");
            return -1;
        }else if(ret==0){
            continue;
        }else{
            printf("%s\n",tt->buffer);
            if(!strcmp(tt->buffer,"close"))
            {
                close(tt->fd);
                return 0;
            }
        }
    }
    return 0;
}
//服务端进行写操作
/*
    fd:服务端socket
    write_buffer:写操作的缓冲区
*/
void* server_write(void* arg)
{
    SL* tt=(SL*)arg;
    while(1)
    {
        memset(tt->buffer,'\0',sizeof(tt->buffer));
        scanf("%s",tt->buffer);
        if(write(tt->fd,tt->buffer,strlen(tt->buffer))==-1)
        {
            perror("server write false");
            return -1;
        }
    }
    return 0;
}
//封装一个sockfd初始化的函数
int socket_server_init(unsigned short port, char * addr)
{
    int sockfd;
    struct sockaddr_in server_addr;
    printf("port = %d, addr  = %s\n", port, addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("socket");
        return -1;
    }
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))== -1)
    {
        perror("bind");
        return -1;
    }
                                                                                                                                                                    

    if(listen(sockfd, 5) == -1)
    {
        perror("listen");
        return -1;
    }
    return sockfd;
}

int main(int argc, const char *argv[])
{
    int sfd;
    pid_t pid;
    sfd = socket_server_init(40000, "192.168.29.152");//调用生成sfd
    if(sfd < 0)
    {
        perror("socket create false");
        return -1;
    }
    while(1)
    {
        int cfd;
        char client_ip[INET_ADDRSTRLEN];
        struct sockaddr_in client_addr;
        socklen_t addr_len=sizeof(client_addr);
        if( (cfd = accept(sfd, (struct sockaddr*)&client_addr, &addr_len)) == -1)
        { 
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno); 
            continue; 
        } 
        //获取客户端的ip地址
        inet_ntop(AF_INET,&client_addr.sin_addr,client_ip,INET_ADDRSTRLEN);
        printf("client ip: %s\n",client_ip);
        //memset(write_buffer,'\0',sizeof(write_buffer));
        pid=fork();
        if(pid==0)
        {
            //close(sfd);
            char write_buffer[1024],read_buffer[1024];
            //使用线程去执行client的读操作和写操作
            pthread_t th_1,th_2;
            SL read_b,write_b;
            //对线程函数的参数进行赋值
            read_b.fd=write_b.fd=cfd;
            read_b.buffer=read_buffer;
            write_b.buffer=write_buffer;
            if(pthread_create(&th_1,NULL,(void*)server_read,(void*)&read_b))
            {
                perror("read pthread");
                return -1;
            }
            if(pthread_create(&th_2,NULL,(void*)server_write,(void*)&write_b))
            {
                perror("write pthread");
                return -1;
            }
            pthread_detach(th_1);
            pthread_detach(th_2);
        }
    }
    close(sfd);
    return 0;
}
