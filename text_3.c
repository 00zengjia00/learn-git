#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <errno.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <pthread.h>

typedef struct client_information
{
    int fd;
    char* buffer;
}CL;

//客户端的socket初始化
/*
    port:服务器端口号
    addr:服务器的ip
*/
int client_socket(unsigned short port, char * addr)
{
    int cfd;
    struct sockaddr_in servaddr;
    if((cfd=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        printf("create client socket error: %s\n",strerror(errno));
        exit(0);
    }
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(port);
    servaddr.sin_addr.s_addr=inet_addr(addr);

    if(-1==connect(cfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))
    {
        perror("connect");
    }
    return cfd;

}
//客户端进行读操作
/*
    fd:客户端socket
    read_buffer:读操作的缓冲区
*/
void* client_read(void* arg)
{
    CL* tt=(CL*)arg;
    while(1)
    {
        memset(tt->buffer,'\0',sizeof(tt->buffer));
        int ret=read(tt->fd,tt->buffer,sizeof(tt->buffer));
        if(ret==-1)
        {
            perror("client read false");
            return -1;
        }else if(ret==0){
            continue;
        }else{
            printf("%s\n",tt->buffer);
        }
    }
    return 0;
}
//客户端进行写操作
/*
    fd:客户端socket
    write_buffer:写操作的缓冲区
*/
void* client_write(void* arg)
{
    CL* tt=(CL*)arg;
    while(1)
    {
        memset(tt->buffer,'\0',sizeof(tt->buffer));
        scanf("%s",tt->buffer);
        if(write(tt->fd,tt->buffer,strlen(tt->buffer))==-1)
        {
            perror("client write false");
            return -1;
        }else{
            if(!strcmp(tt->buffer,"close"))
            {
                close(tt->fd);
                printf("链接断开\n");
                exit(0);
            }
        }
    }
    return 0;
}
int main(int argc, char** argv)
{
    char read_buffer[1024],write_buffer[1024];
    //memset(read_buffer,'\0',sizeof(read_buffer));
    //memset(write_buffer,'\0',sizeof(write_buffer));
    int cfd;
    cfd=client_socket(40000,"192.168.29.152");
    //使用线程去执行client的读操作和写操作
    pthread_t th_1,th_2;
    CL read_b,write_b;
    //对线程函数的参数进行赋值
    read_b.fd=write_b.fd=cfd;
    read_b.buffer=read_buffer;
    write_b.buffer=write_buffer;
    if(pthread_create(&th_1,NULL,(void*)client_read,(void*)&read_b))
    {
        perror("read pthread");
        return -1;
    }
    if(pthread_create(&th_2,NULL,(void*)client_write,(void*)&write_b))
    {
        perror("write pthread");
        return -1;
    }
    pthread_detach(th_1);
    pthread_detach(th_2);
    while(1)
    {

    }
    return 0;
}