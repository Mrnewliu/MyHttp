#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

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
static void Init(void);
struct sockaddr_in server,client;
static void* thread_fun(void* arg);
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
        num = read(sc[i],rbuf,sizeof(rbuf));
        if( num < 0 )
        {
            printf("read finish\n");
            break;
        }
        else
        {
            printf("%s\n",rbuf);
        }
    }
    max_flag--;
    pthread_exit((void*)11);
}
