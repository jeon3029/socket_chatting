/* TCP Echo Client */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
//#include "practical.h"

int main (int argc, char *argv[])
{
  /* 명령 파라미터의 개수가 3 or 4인지 검사 */
  if (argc<2 || argc>3)  {
    printf ("Usage : %s <Server Address> [<Server Port>]\n", argv[0]);
    exit (1);
  }
  char *servIP = argv[1];
  in_port_t servPort = (argc == 3) ? atoi (argv[2]) : 7 ;

  /* socket() : TCP 소켓 생성 */
  int sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock == -1){
    printf("socket error\n");
  //error_handling ("socket() error");
    return -1;
  }

  /* connect() : 서버 접속 */
  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr (servIP);
  serv_addr.sin_port = htons (servPort);
  if (connect (sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
    printf("connection error\n");
    return -1;
  }
  //error_handling ("connect() error");

  printf(" ##################### 1 : 1 채팅이 시작 되었습니다. ##################\n");
  printf(" ##########         종료를 원할 경우 /quit 입력 하세요      ###########\n");

  while(1)
  {
    /* 입력 받을 버퍼 생성  */
    char message[BUFSIZ];
    memset(message,0,strlen(message));

    /* 사용자로부터 입력 받음 */
    printf("Client : ");
    fgets(message,BUFSIZ,stdin);
    int len = strlen(message);
    int flag = 0;
    
    /* 한글확인*/
    for(int i=0;i<len;i++){
      if((message[i] & 0x80) == 0x80) {
        i++;
        continue;
      }
      else{
        printf("한글을 입력해 주세요!\n");
        flag=1;break;
      }
    }if(flag)continue;



    /* 입력 받은 문자 센드 */
    size_t echoStringLen = strlen(message);
    ssize_t numBytes = send (sock,message, echoStringLen, 0);
    if (numBytes == -1){
      printf("send error\n");
      return -1;
      //error_handling ("send() error");
    }
    /* 입력 받은 문자가 /quit 일 경우 브레이크 */
    if(strcmp(message,"/quit\n") == 0)
      break;

    /* 서버측으로부터 리시브  */
    char buffer[BUFSIZ];
    memset(buffer,0,sizeof(buffer));
    numBytes = recv(sock,buffer,BUFSIZ,0);
    if(numBytes == -1 )return -1;
//      error_handling(" recv() error ");

    if(strcmp(buffer,"/quit\n") == 0)
      break; 
    printf("from %s : %s ",inet_ntoa(serv_addr.sin_addr),buffer);
    memset(buffer,0,sizeof(buffer));
  }
  /* close() : 소켓 종료 */
  fputc('\n',stdout);
  printf(" #############     1 : 1 채팅이 종료 되었습니다.    #############\n");
  close (sock);
  return 0;
}



