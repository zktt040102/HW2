#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <net/if.h>
#include <sys/ioctl.h>

int sockfd;//客戶端socket
char* IP = "127.0.0.1";//服務器的IP
short PORT = 10222;//服務器服務端口
typedef struct sockaddr SA;
char name[30];

void init(){
    sockfd = socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);
    if (connect(sockfd,(SA*)&addr,sizeof(addr)) == -1){
        perror("無法連接到服務器");
        exit(-1);
    }
    printf("客戶端啟動成功\n");
}

void start(){
    pthread_t id;
    void* recv_thread(void*);
    pthread_create(&id,0,recv_thread,0);
    char buf2[100] = {};
    sprintf(buf2,"%s進入了聊天室",name);
    send(sockfd,buf2,strlen(buf2),0);
    while(1){
        char buf[100] = {};
        scanf("%s",buf);
        char msg[131] = {};
        sprintf(msg,"%s:%s",name,buf);
        send(sockfd,msg,strlen(msg),0);
        if (strcmp(buf,"bye") == 0){
            memset(buf2,0,sizeof(buf2));
            sprintf(buf2,"%s退出了聊天室",name);
            send(sockfd,buf2,strlen(buf2),0);
            break;
        }
    }
    close(sockfd);
}

void* recv_thread(void* p){
    while(1){
        char buf[100] = {};
        if (recv(sockfd,buf,sizeof(buf),0) <= 0){
            return;
        }
        printf("%s\n",buf);
    }
}

int main(){
    init();
    printf("請輸入您的名字：");
    scanf("%s",name);
    start();
    return 0;
}
