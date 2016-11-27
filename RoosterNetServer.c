/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio_ext.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>

void ticTacToe(int sock);
void requestMove(int *lin, int *col,int sock);
void sendMove(int *lin, int *col,int sock, int board[][3]);
int checkwin(int board[][3], int turn);
void updateboard(int board[][3]);
void printwinner(int winner);
int askInput();

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, n, pid;
     struct sockaddr_in serv_addr, cli_addr; 
     printf("argc = %d\n", argc);
     int i = 0;
     for(i = 0; i < argc; i++)
     	printf("argv[%d] = %s\n", i, argv[i]);

	// verifica erros (falta de porta, nao consegue abrir socket)
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     
     // inicializações do socket a abrir
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
              
     // fica à espera do cliente
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
     	fflush(stdin);
		fflush(stdout);
     	printf("Waiting for connection\n");
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             ticTacToe(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     return 0; /* we never get here */
}
void ticTacToe(int sock)
{
   int n, col, lin, count;
   char buff[256], r;
   int board[3][3];		// cliente = 2, servidor = 1
   int win = 0;			// cliente = 2, servidor = 1
      
   
   // Estabelecer conecção
     bzero(buff,256);
     n = write(sock,"sync",4);
     if (n < 0) error("ERROR writing to socket");
     n = read(sock,buff,255);
     if (n < 0) error("ERROR reading from socket");
     if(strcmp(buff, "sync") == 0){
     	printf("Connection Established\n");
     }
     else error("Error, couldn't established connection");
     printf("%s\n", buff);
     	
     	// Receber nome do oponente
      bzero(buff,256);
      n = write(sock,"name",4);
      if (n < 0) error("ERROR writing to socket");
      n = read(sock,buff,255);
      if (n < 0) error("ERROR reading from socket");
      
      printf("Name: %s\n", buff);
      
      while(1){
      	count = 1;
      	for(col = 0; col < 3; col++){
   		for(lin = 0; lin < 3; lin++){
   			board[lin][col] = 0;
   		}
   	}
	      // Perguntar utilizador se quer jogar
	      do{
		      	fflush(stdin);
				fflush(stdout);
		      	printf("Do you accept this challenge? (Y/N):\t");
		      	scanf (" %c",&r);
		}while(r != 'N' && r != 'n' && r != 'Y' && r != 'y');
	      if(r == 'N' || r == 'n'){
	      	char z[3] = "00\0";
	      	n = write(sock,z,3);
	   	if (n < 0) error("ERROR writing to socket");
	   	n = read(sock,buff,255);
	   	if (n < 0) error("ERROR reading from socket");
	   	return;
	      }
	      else if(r == 'Y' || r == 'y'){
	      	char z[2] = "11";
	      	n = write(sock,z,2);
	   	if (n < 0) error("ERROR writing to socket");
	   	n = read(sock,buff,255);
	   	if (n < 0) error("ERROR reading from socket");
	      }
	      
	   // perguntar ao cliente se quer ser o 1º
	   bzero(buff,256);
	   fflush(stdin);
	   fflush(stdout);
	   n = write(sock,"f/s",3);
	   if (n < 0) error("ERROR writing to socket");
	   n = read(sock,buff,255);
	   if (n < 0) error("ERROR reading from socket");
	   if(buff[0] == 'f' || buff[0] == 'F'){
	   	while(1){
	   		printf("Opponent's Move\n");
	   		requestMove(&lin, &col, sock);
	   		board[lin][col] = 2;
	   		updateboard(board);
	   		if(checkwin(board,2) == 1){
	   			win = 2;
	   			break;
	   		}
	   		if(count < 9)
	   			count++;
	   		else{
	   			win = 3;
	   			break;
	   		}
	   		printf("Your Move\n");
	   		sendMove(&lin, &col, sock, board);
	   		board[lin][col] = 1;
	   		updateboard(board);
	   		if(checkwin(board,1) == 1){
	   			win = 1;
	   			break;
	   		}
	   		if(count < 9)
	   			count++;
	   		else{
	   			win = 3;
	   			break;
	   		}		
	   		
	   	}
	   }
	   else if(buff[0] == 's' || buff[0] == 'S'){
	   	while(1){
	   		printf("Your Move\n");
	   		sendMove(&lin, &col, sock, board);
	   		board[lin][col] = 1;
	   		updateboard(board);
	   		if(checkwin(board,1) == 1){
	   			win = 1;
	   			break;
	   		}
	   		if(count < 9)
	   			count++;
	   		else{
	   			win = 3;
	   			break;
	   		}
	   		printf("Opponent's Move\n");
	   		requestMove(&lin, &col, sock);
	   		board[lin][col] = 2;
	   		updateboard(board);
	   		if(checkwin(board,2) == 1){
	   			win = 2;
	   			break;
	   		}
	   		if(count < 9)
	   			count++;
	   		else{
	   			win = 3;
	   			break;
	   		}		
	   		
	   	}
	   }
	   
	   printwinner(win);
	   
	   	// perguntar se querem repetir o jogo
	   n = write(sock,"again?",6);
	   if (n < 0) error("ERROR writing to socket");
	   n = read(sock,&r,1);
	   if (n < 0) error("ERROR reading from socket");
	   if(r == '0'){
	   	printf("Client doesn't want to repeat\n");
	   	break;
	   }
	   else if(r == '1'){
	   	printf("Client wants to repeat\n");
	   }
	}
   
}

void requestMove(int *lin, int *col,int sock){
	int n;
	// pede linha
	n = write(sock,"lin",3);
   	if (n < 0) error("ERROR writing to socket");
   	n = read(sock,lin,sizeof(int));
   	if (n < 0) error("ERROR reading from socket");
   	*lin = ntohs(*lin);
   	// pede coluna
   	n = write(sock,"col",3);
   	if (n < 0) error("ERROR writing to socket");
   	n = read(sock,col,sizeof(int));
   	if (n < 0) error("ERROR reading from socket");
   	*col = ntohs(*col);
}
void sendMove(int *lin, int *col,int sock, int board[][3]){
	int n, l, c, aux;
	char buffer[256];
	
	do{
	printf("Select your moves' row (1-3):\t");
	l = askInput();
	
	printf("Select your moves' column (1-3):\t");
	c = askInput();
	}while(board[l][c] != 0);
	
	// envia linha
	aux = htons(l);
	n = read(sock,buffer,256);
   	if (n < 0) error("ERROR reading from socket");
	n = write(sock,&aux,sizeof(aux));
   	if (n < 0) error("ERROR writing to socket");
   	
   	
   	// envia coluna
   	aux = htons(c);
   	n = read(sock,buffer,256);
   	if (n < 0) error("ERROR reading from socket");
   	n = write(sock,&aux,sizeof(aux));
   	if (n < 0) error("ERROR writing to socket");
   	
   	*lin = l;
   	*col = c;   	
}
int checkwin(int board[][3], int turn){
// return 1 on win. return 0 otherwise
	 int check = 0;
	 int i, j;
	 
	 // verifica linhas
	 for(i = 0; i < 3; i++){
	 	if(board[i][0] == board[i][1] && board[i][0] == board[i][2] && board[i][0] == turn){
	 		check = 1;
	 		break;
	 	}
	 }
	 
	 // verifica colunas
	 for(j = 0; j < 3; j++){
	 	if(board[0][j] == board[1][j] && board[0][j] == board[2][j] && board[0][j] == turn){
	 		check = 1;
	 		break;
	 	}
	 }
	 
	 // verificar diagonais
	 if(board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] == turn)
	 	check = 1;
	 if(board[2][0] == board[1][1] && board[2][0] == board[0][2] && board[2][0] == turn)
	 	check = 1;
	 	
	 return(check);
}
void updateboard(int board[][3]){
	int j, i;
	printf("\n");
	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			if(board[i][j] == 0){
				printf("-");
			}
			else if(board[i][j] == 1){
				printf("x");
			}
			else
				printf("o");
		}
		printf("\n");
	}
}
void printwinner(int winner){
	if(winner == 1){
		printf("Server wins!!!!!!!!!\n");
	}
	else if(winner == 2){
		printf("Client wins!!!!!!!\n");
	}
	else
		printf("Empate......\n");
}
int askInput(){
	int n = 0;
	do{
		fflush(stdin);
		scanf("%i", &n);
	}while(n < 1 || n > 3);
	
	return(n-1);
}
