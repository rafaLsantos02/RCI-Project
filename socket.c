#include "struct.h"
#include "header.h"




////////////////////////////////// SERVIDOR TCP ////////////////////////////////////////

/*************************************************************************
 * setup_serverTCP()
 * 
 * Argumentos: char *IP - corresponde ao IP do servidor que irá ser criado
 *             char *TCP - corresponde ao porto 
 * 
 * Return: server_socket - irá returnar o socket do novo servidor
 * 
 * Descrição: Cria e coloca o servidor à "escuta"
**************************************************************************/
int setup_serverTCP(char *IP, char *TCP){

    int server_socket, errcode;
    struct addrinfo hints, *res;

    if( (server_socket = socket(AF_INET, SOCK_STREAM, 0) ) == -1){
        printf("Erro na criação do socket. [TCP]");
        exit(1); //erro
    }

    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
        printf("Erro na configuração da opção SO_REUSEADDR. [TCP]");
        exit(1); // Erro
    }
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = SOCK_STREAM; //TCP socket
    hints.ai_flags = AI_PASSIVE;

    if( (errcode = getaddrinfo(IP, TCP, &hints, &res) ) != 0){
        printf("Erro na atribuição do IP e TCP ao socket. [TCP]");
        exit(1); //erro
    }

    if( bind(server_socket, res->ai_addr, res->ai_addrlen) == -1){
        printf("Erro no bind. [TCP]");
        exit(1); //erro
    }

    if( listen(server_socket, 5) == -1){
        printf("Erro no listen. [TCP]");
        exit(1); //erro
    }


    freeaddrinfo(res);
    return server_socket;
}


/*************************************************************************
 * accept_new_connectionTCP()
 * 
 * Argumentos: server_socket - é o que identifica o socket do servidor
 * 
 * Return: client_socket - retorna a o socket da ligação entre o servidor e o cliente
 * 
 * Descrição: O servidor aceita outros clientes 
**************************************************************************/
int accept_new_connectionTCP(int server_conection_tcp){
    
    int new_connection;
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);

    if( (new_connection = accept(server_conection_tcp, &addr, &addrlen)) == -1){
        printf("Erro no accept. [TCP]");
        exit(1); //erro
    }

    return new_connection;
}


////////////////////////////////// Cliente TCP ////////////////////////////////////////

/*************************************************************************
 * connect_clientTCP()
 * 
 * Argumentos: char *IP - corresponde ao IP do servidor que nos queremos ligar
 *             char *TCP - corresponde ao porto do servidor que nos queremos ligar
 * 
 * Return: client_connection_socket - irá retornar a ligação entre o client e o servidor
 * 
 * Descrição: Cria a ligação entre o cliente e o servidor TCP
**************************************************************************/
int connect_clientTCP(char *IP, char *TCP){

    int client_connection_socket, errcode;
    struct addrinfo hints, *res;
    int n_recived;

    client_connection_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_connection_socket == -1){
        printf("Erro na criação do socket do cliente. [TCP]");
        exit(1);
    }
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errcode = getaddrinfo(IP, TCP, &hints,&res);
    if(errcode != 0){
        printf("Erro getaddrinfo client. [TCP]");
        exit(1);
    }

    n_recived = connect(client_connection_socket, res->ai_addr, res->ai_addrlen);
    if(n_recived == -1){
        printf("Erro na conexão cliente-servidor. [TCP]");
        exit(1);
    }

    freeaddrinfo(res);
    return client_connection_socket;
}


//Envia mensagens via tcp
void send_message_tcp(char* message, int connection){

    ssize_t nbytes,nleft,nwritten;

    nwritten = 0;
    nbytes = strlen(message);
    nleft = nbytes;

    while(nleft>0){
        nwritten=write(connection,message,nleft);
        if(nwritten<=0){
            printf("Erro eviar mensagem tcp\n");
            exit(1);
        }
        nleft -= nwritten;
        message += nwritten;
    }
}


