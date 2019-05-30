#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

static int startup(int port)
{
    int sock=socket(AF_INET,SOCK_STREAM,0);   //创建套接字 
    if(sock<0)
    {
        perror("socket");
        close(sock);
        exit(2);
    }

    struct sockaddr_in server;
    server.sin_family=AF_INET;
    server.sin_port=htons(port);
    server.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(sock,(struct sockaddr*)&server,sizeof(server))<0)    //绑定：填充网络 
    {
        perror("bind");
        close(sock);
        exit(3);
    }

    if(listen(sock,10)<0)   //监听
    {
        perror("listen");
        close(sock);
        exit(4);
    }

    return sock;  //返回监听套接字 
}

void* request(void* arg)
{
    int new_sock=(int)arg;
    while(1)
    {
        char buf[1024];
        ssize_t s=read(new_sock,buf,sizeof(buf)-1);
        if(s>0)
        {
            buf[s]=0;
            logger_info("get new client# %s\n",buf);
            write(new_sock,buf,strlen(buf));
        }
        else if(s==0)
        {
            logger_info("client close!!!\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }
    return (void*)0;
}

void* main_thread(void* sock)
{
    int listen_sock=(int)sock;
    while(1)
    {
        struct sockaddr_in client;
        socklen_t addrlen=sizeof(client);
        int new_sock=accept(listen_sock,(struct sockaddr*)&client,&addrlen);  //接收客户端信息进行通信 
        if(new_sock<0)
        {
            perror("accept");
            continue;
        }

        //version 1.3
        pthread_t id;
        pthread_create(&id,NULL,request,(void*)new_sock);    //主线程监听接收客户端，创建线程为每一个客户端服务 
        pthread_detach(id);     //使线程分离，主线程不用阻塞等待，操作系统去回收 

    }
}


int tcp_server_init(int port)
{
    pthread_t main_id;
    int sock=startup(port);  //创建监听套接字：函数三步 
    pthread_create(&main_id,NULL,main_thread,(void*)sock);    //主线程监听接收客户端，创建线程为每一个客户端服务 
    return 0;
}
