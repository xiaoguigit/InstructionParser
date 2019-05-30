#include "common.h"

typedef struct serverStruct
{
    threadpool_t *epollpool;
    int epollfd;
    clilist_t clilist;  
}*pserver_t, server_t;

threadpool_t *epollpool;
int epollfd;
clilist_t clilist;  

static void set_nonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);

    if(opts < 0) {
        logger_error("fcntl(sock, GETFL)");
        return ;
    }

    opts = opts | O_NONBLOCK;

    if(fcntl(sock, F_SETFL, opts) < 0) {
        logger_error("fcntl(sock, SETFL, opts)");
        return ;
    }
}

/*
static void op_add_event(int *fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = *fd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,*fd,&ev);
}
*/

static void op_delete_event(int *fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = *fd;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,*fd,&ev);
}

/*
static void op_modify_event(int *fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = *fd;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,*fd,&ev);
}
*/

static int init_servsock(char * ip, int port)
{
    int listenfd;
    int on = 1;
    int ret = 0;
    struct sockaddr_in serveraddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    set_nonblocking(listenfd);

    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(int));

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_aton(ip, &(serveraddr.sin_addr));
    serveraddr.sin_port = htons(port);

    ret = bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if(ret < 0)
    {
        logger_error("%s\n", "bind error!");
        return -1;
    }

    ret = listen(listenfd, MAX_LISTEN_QUEUE);
    if(ret<0)
    {
        logger_error("errno code is %d\n", errno);
    }

    return listenfd;
}


int add_clilist(clientmember cli)
{
    clilist.clilist[clilist.tailindex] = cli;
    clilist.tailindex ++;
    return clilist.tailindex;
}

int del_clilist(clientmember cli)//del by fd
{
    int i=0, j=0;
    for(i=0; i<clilist.tailindex; i++)
    {
        if(clilist.clilist[i].fd == cli.fd)
        {
            for(j=i; j<clilist.tailindex-1; j++)
            {
                clilist.clilist[j] = clilist.clilist[j+1];
            }
        }
    }
    clilist.tailindex--;
    return clilist.tailindex;
}

int op_read(int *fd, void * buf, ssize_t count)
{
    int n = read((*fd), buf, count);
    if(n < 0 && errno == EAGAIN)
    {
        logger_error("%s\n", "Data has been read, no more data can be read.");
        return -1;
    }
    else if(n == 0)
    {
        logger_info("%s, fd is %d\n", "Client disconnect.", (*fd));
        op_delete_event(fd);
        clientmember member;
        member.fd = (*fd);
        del_clilist(member);
        return -2;
    }
    else if(n < 0)
    {
        logger_error("Read error. Error code is %d\n", errno);
        op_delete_event(fd);
        clientmember member;
        member.fd = (*fd);
        del_clilist(member);
        return -3;
    }
    else
    {
        return n;
    }
}

int op_write(int *fd, const void *buf, size_t count)
{
    int n = write((*fd), buf, count);
    if(n < 0 && errno == EAGAIN)
    {
        logger_error("%s\n", "System buffer data has been written.");
        return -1;
    }
    else if(n == 0)
    {
        logger_info("%s,fd is %d\n", "Client disconnect.", (*fd));
        op_delete_event(fd);
        clientmember member;
        member.fd = (*fd);
        del_clilist(member);
        return -2;
    }                   
    else if(n < 0)
    {                   
        logger_error("Read error. Error code is %d\n", errno);
        op_delete_event(fd);
        clientmember member;
        member.fd = (*fd);
        del_clilist(member);
        return -3;  
    }
    else
    {
        return n;
    }
}

int start_server(char *ip, int port)
{   
    int i, listenfd, connfd, nfds;
    struct sockaddr_in clientaddr;
    
    socklen_t clilen;

    struct epoll_event ev, events[MAX_CONNECT];

    logger_info("bind : \"ip\":%s,\"port\":%d\n", ip, port);
    
    epollpool = threadpool_create(THREAD, QUEUE, 0);
    if(!epollpool)
    {
        logger_error("%s\n", "threadpool init error!");
        return -1;
    }
    listenfd = init_servsock(ip, port); 
    epollfd = epoll_create(MAX_CONNECT-1);
    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

    for(; ;) 
    {
        nfds = epoll_wait(epollfd, events, MAX_CONNECT, -1);

        for(i = 0; i < nfds; ++i) 
        {
            if(events[i].data.fd == listenfd) {

                connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clilen);
                if(connfd < 0) {
                    logger_error("connfd < 0");
                    return -1;
                }               
                set_nonblocking(connfd);
                logger_info("accept connect from %s, fd is %d\n", inet_ntoa(clientaddr.sin_addr), connfd);

                clientmember member;
                member.fd = connfd;
                strcpy(member.ip, inet_ntoa(clientaddr.sin_addr));
                add_clilist(member);

                ev.data.fd = connfd;
                ev.events = EPOLLIN | EPOLLET;

                epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev);
            }
            else if(events[i].events & EPOLLIN) 
            {                       
                threadpool_add(epollpool, &handle, &events[i].data.fd, 0);
            }
            else if(events[i].events & EPOLLOUT) 
            {

            }
        }
    }
}
