#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <stdio_ext.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}
void ticTacToe(int sock);
void requestMove(int *lin, int *col,int sock);
void sendMove(int *lin, int *col,int sock, int board[][3]);
int checkwin(int board[][3], int turn);
void updateboard(int board[][3]);
void printwinner(int winner);
int valueInput();
void nameInput(char *nome);

int main(int argc, char *argv[])
{	
	// inicializações
    int sockfd, portno, n;
    char nome[20];
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
        // estabelecer conecção
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("Received from server: %s\n", buffer);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    printf("Connection Established\n");
    
    	// enviar nome
    n = read(sockfd,buffer,255);
    if (n < 0) 
    	error("ERROR reading from socket");
    printf("Received from server: %s\n", buffer);

    nameInput(nome);
    n = write(sockfd,nome,strlen(nome));
    if (n < 0) 
         error("ERROR writing to socket");
    printf("Introduced Name = %s\n", nome);
    ticTacToe(sockfd);
    
    
    
    
    
    return(0);
}


void ticTacToe(int sock)
{
   int n, col, lin, count;
   char buff[256];
   char r;
   int board[3][3];		// cliente = 2, servidor = 1
   int win = 0;			// cliente = 2, servidor = 1, empate = 3;
   
   while(1){
   	count = 1;
	   for(col = 0; col < 3; col++){
	   	for(lin = 0; lin < 3; lin++){
	   		board[lin][col] = 0;
	   	}
	   }
	      
	   bzero(buff,256);
	   
	   // Ver se servidor aceita ou nao
	    char desafio[3];		// 3 because of '\0'
	    n = read(sock,desafio,3);
	    if (n < 0) 
	    	error("ERROR reading from socket");
	    if(strcmp(desafio, "00") == 0){
	    	printf("Server did not accept challenge\n");
	    	return(0);
	    }
	    else{
	    	printf("Server accepted the challenge\n");
	    }
	    n = write(sock,"ok",2);
	    if (n < 0) 
		 error("ERROR writing to socket");
		 
		 
	   // Responder se quer ser 1º ou 2º
	    n = read(sock,buff,255);
	    if (n < 0) 
	    	error("ERROR reading from socket");
	    if(strcmp(buff, "f/s") != 0)
	    	error("Server and Client not in Sync");
	    fflush(stdin);
	    fflush(stdout);
	    do{
	    	printf("First or Second (f/s):\t");
	    	scanf("%c", &r);
	    }while(r != 'f' && r != 's' && r != 'S' && r != 'F');
	    n = write(sock,&r,sizeof(r));
	    if (n < 0) 
		 error("ERROR writing to socket");
		 
		 
		 
	   if(r == 'f' || r == 'F'){
	   	while(1){
	   		printf("Your Move\n");
	   		sendMove(&lin, &col, sock, board);		// jogada do cliente
	   		board[lin][col] = 2;
	   		updateboard(board);
	   		if(checkwin(board, 2) == 1){
	   			win = 2;
	   			break;
	   		}
	   		printf("count = %d\n",count);
	   		if(count < 9)
	   			count++;
	   		else{
	   			win = 3;
	   			break;
	   		}
	   		printf("Opponent's Move\n");
	   		requestMove(&lin, &col, sock);		// jogada do servidor
	   		board[lin][col] = 1;
	   		updateboard(board);
	   		if(checkwin(board, 1) == 1){
	   			win = 1;
	   			break;
	   		}
	   		printf("count = %d\n",count);
	   		if(count < 9)
	   			count++;
	   		else{
	   			win = 3;
	   			break;
	   		}		
	   		
	   	}
	   }
	   if(r == 's' || r == 'S'){
	   	while(1){
	   		printf("Opponent's Move\n");
	   		requestMove(&lin, &col, sock);		// jogada servidor
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
	   		printf("Your Move\n");
	   		sendMove(&lin, &col, sock, board);		// jogada cliente
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
	   
	   	// repetir jogo
		n = read(sock,buff,256);
		if (n < 0) error("ERROR reading from socket");
			// perguntar utilizador
		do{
			fflush(stdout);
			printf("Want a Rematch?(Y/N)\t");
			fflush(stdin);
			scanf(" %c", &r);
		}while(r != 'N' && r != 'n' && r != 'Y' && r != 'y');
		if(r == 'N' || r == 'n'){
			n = write(sock,"0",1);
			if (n < 0) error("ERROR writing to socket");
			break;
		}
		else if(r == 'Y' || r == 'y'){
			n = write(sock,"1",1);
			if (n < 0) error("ERROR writing to socket");
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
	l = valueInput();
	
	printf("Select your moves' column (1-3):\t");
	c = valueInput();
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
		printf("Tie...........\n");
}
int valueInput(){
	int n = 0;
	
	do{
		__fpurge(stdin);
		scanf("%i", &n);
	}while(n < 1 || n > 3);
	
	return(n-1);
}

void nameInput(char *nome){
	
	printf("Which Name you want to send to server:\t");
	// nome = getchar();
	fflush(stdin);
	fflush(stdout);
	// fgets(nome,strlen(nome),stdin);
	scanf("%s", nome);
	
}
