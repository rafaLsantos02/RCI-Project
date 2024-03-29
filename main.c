#include "struct.h"
#include "header.h"


int main(int argc, char *argv[]){

    char buffer_comando[buffer_short];
    char *IP, *TCP, *regIP,*regUDP;
    int node_created = -1;
    int new_connection;
    int i, max_socket;
    int counter;
    int add_connection;
    
    fd_set current_sockets;   
    fd_set ready_sockets; 

    struct node *fixed_node = NULL;

    validate_arguments(argc, argv, &IP, &TCP, &regIP, &regUDP);
    
    inicialize_global_vectors( );

        
    lista_comandos();

    //----------INICIO SELECT----------------  

    while(1){

        FD_ZERO(&current_sockets); //Remove todos os descritores da lista *current_sockets
        FD_SET(STDIN_FILENO, &current_sockets); //Adiciona o descritor do teclado
        max_socket = STDIN_FILENO;

        
        if( fixed_node != NULL ){
            
            for( i = 0; i < ROWS_ADJ; i++){

                add_connection = fixed_node->adjacencies[i];

                if( add_connection != -1 ){
                    FD_SET( add_connection , &current_sockets); 

                    max_socket = max(max_socket, add_connection);
                }
            }
        } 



        //Usar um auxiliar porque o select é destrutivo
        ready_sockets = current_sockets;
        if( ( counter = select(max_socket + 1, &ready_sockets, NULL, NULL, NULL) ) < 0 ){
            printf("Erro no select.");
            exit(1);
        }


        for( i = 0; i <= max_socket && counter > 0 ; i++ ){ //Vai ver quais os descritores prontos para leitura

            if(FD_ISSET(i, &ready_sockets) != 0 ){ 

                counter--;

                if( i == STDIN_FILENO){ //Verifica que o teclado ta pronto
                    
                    memset(buffer_comando, 0, buffer_short);
                    fgets(buffer_comando, buffer_short, stdin);

                    fixed_node = handle_command(buffer_comando, IP, TCP, regIP, regUDP, &node_created, fixed_node);
                    
                    if( fixed_node == NULL){
                        lista_comandos();
                        continue;
                    }

                    lista_comandos();
                }
                else if( i == fixed_node->connection_next){ //O nó à frente enviou uma mensagem
                    
                    message_from_next( i, fixed_node, regIP, regUDP);

                }
                else if( i == fixed_node->connection_back){//O nó a trás enviou uma mensagem

                    message_from_back( i , fixed_node);

                }
                else if( i == fixed_node->serverTCP){ //Um novo nó quer ligar-se 
                    
                    new_connection = accept_new_connectionTCP(fixed_node->serverTCP);

                    fase2( new_connection, fixed_node );


                }else{

                    message_from_back( i ,fixed_node );
                    
                }

            } 
            
        }

    }
}
    















