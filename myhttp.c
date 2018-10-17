#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>

#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>
#include<arpa/inet.h>

#define MAX_LISTEN 10
#define K 1024
int s,sc[MAX_LISTEN];
static pthread_t th[MAX_LISTEN];
static int max_flag;
char rbuf[10*K];
char wbuf[10*K];
static void Init(void);
struct sockaddr_in server,client;
static void* thread_fun(void* arg);
static unsigned char respond_pack(void* buf);

int main(int argc,char* argv[])
{
    Init();

    int i=0,len;
    len = sizeof(struct sockaddr);

    while(1)
    {
        while(max_flag == 10);
        if( (sc[i] = accept(s,(struct sockaddr*)(&client),&len)) == -1)
        {
            printf("accept  error,errrno is %d\n",errno);
            printf("%s\n",strerror(errno));
            continue;
        }

        if( pthread_create(&th[i], NULL,thread_fun,(void*)i) != 0)
        {
            printf("phtread create %d failed,errno is %d\n",i, errno);
        }
        i++;

        if(i > MAX_LISTEN)
        {
            i = 0;
        }


    }


    return 0;
}

static void Init(void)
{
    int err;
    if((s = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        printf("socket create failed,errno is %d\n",errno);
        exit(-1);
    }

    bzero(&server,sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(54188);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    //server.sin_addr.s_addr = inet_addr("47.89.8.236");



    if(bind(s,(struct sockaddr*)(&server),sizeof(server)) == -1)
    {
        printf("bind fialed,errno is %d\n",errno);
        exit(-1);
        //return;
    }

    listen(s,MAX_LISTEN);

}
static void* thread_fun(void* arg)
{
    int num;
    int i = (int)arg;
    max_flag++;
    while(1)
    {
        num = recv(sc[i],rbuf,sizeof(rbuf),0);
        if( num < 0 )
        {
            printf("read finish\n");
            break;
        }
        else
        {
            printf("%s\n",rbuf);
            respond_pack(wbuf);
            num = send(sc[i],wbuf,strlen(wbuf),0);
            if(num < 0)
            {
                printf("send error,errno is %d\n",errno);
                sleep(1);
                break;
            }
        }
    }
    max_flag--;
    pthread_exit((void*)11);
}
static unsigned char respond_pack(void* buf)
{
    int fd,num = 8;
    char datafile[2*K];

    bzero(buf,sizeof(buf));

    if( (fd = open("www/index.html",O_RDWR)) == -1)
    {
        printf("open failed,errno is %d\n",errno);
        exit(-1);
    }
    do{
        num = read(fd,datafile,124);
        if(num < 0)
        {
            printf("read error,errorno is %d\n",errno);
            continue;
        }
    }while(num == 124);
    
    close(fd);

    sprintf(buf,
    "HTTP/1.1  OK\r\n"
    "Conten-Type:text/html:charset-iso--8b59-1\r\n"
    "%s\r\n",datafile);
    
    return 0;
}
