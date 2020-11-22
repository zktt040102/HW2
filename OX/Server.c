#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <net/if.h>
#include <sys/ioctl.h>

int sockfd;//服務器socket
int fds[100];//客戶端的socketfd,100個元素，fds[0]~fds[99]
int size =100 ;//用來控制進入聊天室的人數為100以內
char* IP ="127.0.0.1";
short PORT = 10222;
typedef struct sockaddr SA;

void init(){
    sockfd = socket(PF_INET,SOCK_STREAM,0);
    if (sockfd == -1){
        perror("創建socket失敗");
        exit(-1);
    }

    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);
    if (bind(sockfd,(SA*)&addr,sizeof(addr)) == -1){
        perror("綁定失敗");
        exit(-1);
    }
    if (listen(sockfd,100) == -1){
        perror("設置監聽失敗");
        exit(-1);
    }
}

void SendMsgToAll(char* msg){
    int i;
    for (i = 0;i < size;i++){
        if (fds[i] != 0){
            printf("sendto%d\n",fds[i]);
            send(fds[i],msg,strlen(msg),0);
        }
    }
}

void* service_thread(void* p){
    int fd = *(int*)p;
    printf("pthread = %d\n",fd);
    while(1){
        char buf[100] = {};
        if (recv(fd,buf,sizeof(buf),0) <= 0){
            int i;
            for (i = 0;i < size;i++){
                if (fd == fds[i]){
                    fds[i] = 0;
                    break;
                }
            }
                printf("退出：fd = %dquit\n",fd);
                pthread_exit((void*)i);
        }
        //把服務器接受到的信息發給所有的客戶端
        SendMsgToAll(buf);
    }
}

void service(){
    printf("服務器啟動\n");
    while(1){
        struct sockaddr_in fromaddr;
        socklen_t len = sizeof(fromaddr);
        int fd = accept(sockfd,(SA*)&fromaddr,&len);
        if (fd == -1){
            printf("客戶端連接出錯...\n");
            continue;
        }
        int i = 0;
        for (i = 0;i < size;i++){
            if (fds[i] == 0){
                //記錄客戶端的socket
                fds[i] = fd;
                printf("fd = %d\n",fd);
                //有客戶端連接之后，啟動線程給此客戶服務
                pthread_t tid;
                pthread_create(&tid,0,service_thread,&fd);
                break;
            }
        if (size == i){
            //發送給客戶端說聊天室滿了
            char* str = "對不起，聊天室已經滿了!";
            send(fd,str,strlen(str),0); 
            close(fd);
        }
        }
    }
}

int main(){
    init();
    service();
}
