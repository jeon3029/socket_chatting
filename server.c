/* TCP Iterative Echo Server */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include "practical.h"
#include <syslog.h>
int main (int argc, char *argv[])
{
  /* 명령 파라미터 개수가 2인지 검사 */
  if (argc != 2) {
    printf ("Usage : %s <port>\n", argv[0]);
    exit (1);
  }
  in_port_t servPort = atoi (argv[1]);

  /* socket() : TCP 서버소켓 생성 */
  int serv_sock = socket (PF_INET, SOCK_STREAM, 0);
  if (serv_sock == -1)return -1;
    //error_handling ("socket() error");

  /* bind() : 서버사용 포트/주소를 서버소켓과 묶음 */
  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  serv_addr.sin_port = htons (servPort);
  if (bind (serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)return -1;
    //error_handling ("bind() error");

  /* listen() : 서버소켓을 리슨소켓으로 변경 */
  if (listen (serv_sock, 5) == -1)return -1;
//    error_handling ("listen() error");

  /* 연결대기 상태 알림 */
  printf(" @@@@@@@@@@@ 1 : 1 채팅 서버가 오픈 되었습니다. 클라이언트 접속대기중.. @@@@@@@@@@ \n ");


  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof (clnt_addr);


  /* accept():클라이언트와연결, 클라이언트소켓리턴 */
  int clnt_sock = accept (serv_sock, &clnt_addr, &clnt_addr_size);
  if (clnt_sock == -1)return -1;
//    error_handling ("accept() error");


  /* syslog */
  char logbuf[50];
  memset(logbuf,0,sizeof(logbuf));
  sprintf(logbuf," connection ip : %s , port : %d ", inet_ntoa(clnt_addr.sin_addr),ntohs(clnt_addr.sin_port));
  syslog(LOG_INFO|LOG_LOCAL0,logbuf); 



  printf(" #################### 1 : 1 채팅이 시작 되었습니다. ####################\n");
  printf(" #########        종료를 원할 경우 /quit 를 입력 하세요       ##########\n");
  /* 접속한 클라이언트 정보(1.2.3.4:41532) 출력 */
  printf ("Connected from %s:%d...\n",
      inet_ntoa(clnt_addr.sin_addr),
      ntohs(clnt_addr.sin_port));

  /* 수신 / 송신 반복문 시작 */
  while(1)
  {
    /* 수신 받을 버퍼 생성 */
    char buffer[BUFSIZ];
    memset(buffer,0,sizeof(buffer));

    /* 데이터 수신 받음 */
    ssize_t numbytesrcvd = recv(clnt_sock,buffer,BUFSIZ,0);
    if(numbytesrcvd == -1)return -1;
  //    error_handling(" recv () error");

    /* 수신받은 데이터 출력*/
    printf("from %s : %s  ",inet_ntoa(clnt_addr.sin_addr),buffer);
    buffer[strlen(buffer)-1] = '\0';
    if(strcmp(buffer,"/quit") == 0)
      break;
    memset(buffer,0,sizeof(buffer));



    /* 서버측 입력받을 버퍼 생성   */
    char message[BUFSIZ];
    memset(message,0,sizeof(message));
    /* 보낼 메시지 입력 받음 */
    printf("Server : ");
    fgets(message,BUFSIZ,stdin);

    /* 서버측 문자열 입력 버퍼 센드 */
    if(send(clnt_sock,message,strlen(message),0)== -1)return -1;
//      error_handling( " send () error ");

    /* 우선 송신후, 입력받은 버퍼가 quit 일 경우 종료  */
    if(strcmp(message,"/quit\n") == 0)
      break;

  }
  fputc('\n',stdout); 
  printf(" #############   1 : 1 채팅이 종료 되었습니다  #############\n");
  /* close() : 소켓 종료 */
  close (clnt_sock);
  close (serv_sock);

  return 0;
}



