#include "struct.h"
#include "header.h"



//Esta função envia uma mensagem
//Deve ser definida fora da mensagem qual a mensagem a enviar e a formatação correta
//Em caso de sucesso retorna 1
//Em caso insucesso retorna -1
//Verificar se em caso de insucesso queremos mandar tudo a baixo

//Atençao que o input  quando chamada na main desta função no "char *message_return" deve ser do tipo "&message_return"
//Atençao que depois é preciso dar free ao q é inserido como message_return, -> free(message_return)
int send_UDP( char *message_input, char **message_return, char *IP_UDP, char *UDP){

    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    struct timeval tout;   
    
    socklen_t addrlen;
    int nread;   
    
    fd_set current_sockets;   
    fd_set ready_sockets; 

    char buffer[buffer_big];

    int clientUDP_socket;
    int errcode;
    int end = 1, count = 0;

    
    tout.tv_sec = 2; /* 2 segundos, como exemplo */
    tout.tv_usec = 0;

    //ESTABELECER LIGAÇÃO UDP
    clientUDP_socket = socket(AF_INET,SOCK_DGRAM,0);
    if(clientUDP_socket == -1){
        printf("Erro na criação do socket. [UDP]");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errcode = getaddrinfo(IP_UDP, UDP, &hints, &res);
    if(errcode != 0){
        printf("Erro no getaddrinfo. [UDP]");
        exit(1);
    }

    FD_ZERO(&current_sockets);
    FD_SET(clientUDP_socket,&current_sockets);


    while( end && (count < 5) ){

        ready_sockets = current_sockets;

        //Envia a mensagem que recebeu para o server
        errcode = sendto(clientUDP_socket, message_input, strlen(message_input), 0, res->ai_addr, res->ai_addrlen);
        if(errcode == -1){
            printf("Erro no sendto udp.");
            exit(1); 
        }

        int error;
        if((error = select(clientUDP_socket + 1, &ready_sockets, NULL, NULL, &tout) ) < 0){
            printf("Erro no select.");
            exit(1);
        }

        //RECEBER RESPOSTA UDP DO SERVIDOR
        addrlen = sizeof(addr);

        if(FD_ISSET(clientUDP_socket, &ready_sockets) != 0 ){

            memset(buffer,0,2048);
            nread = recvfrom(clientUDP_socket, buffer, 2048, 0, (struct sockaddr*)&addr, &addrlen);

            if(nread == -1){
                printf("Erro no recvfrom.");

            }else{

                if( strcmp(buffer, "OKREG") == 0){

                    close(clientUDP_socket);
                    freeaddrinfo(res);
                    return 1;

                }else if( strcmp(buffer, "OKUNREG") == 0 ){

                    close(clientUDP_socket);
                    freeaddrinfo(res);
                    return 2;

                }else{

                    // Aloca memória suficiente para armazenar a resposta
                    *message_return = (char *)malloc(nread + 1);
                    if (*message_return == NULL) {
                        printf("Erro ao alocar memória para message_return.");
                        exit(1);
                    }

                    // Copia o conteúdo recebido para o message_return
                    memcpy(*message_return, buffer, nread);
                    (*message_return)[nread] = '\0'; // Adiciona o terminador nulo
                    
                    
                    close(clientUDP_socket);
                    freeaddrinfo(res);
                    return 3;
                }
            }
            
        }

        count++;
    }

    close(clientUDP_socket);
    freeaddrinfo(res);
    return -1;

}


