#include "struct.h"
#include "header.h"

char global_buffer_fase2[buffer_big];
char global_buffer_next[buffer_big];
char global_buffer_back[buffer_big];

void fase2(int connection, struct node *fixed_node){

    char buffer[buffer_big], message[buffer_short], message_2[buffer_short];
    char *message_type, *message_id, *message_IP, *message_port; 
    int new_connection;
    char *aux_buffer, *complete_message;
    char *aux_free; //RAFAEL
    char *last_message;//FIRME

    complete_message = (char *) malloc(sizeof(char) * 50 + 1);
    aux_free = complete_message; //RAFAEL

    memset(buffer, 0, buffer_big);
    memset(message, 0, buffer_short);
    memset(message_2, 0, buffer_short);


    //É efetuada a leitura da mensagem enviada por tcp

    if( read(connection,buffer,buffer_big) == -1 ){        
            printf("Erro na leitura tcp fase2\n");
            exit(1);         
    }

    if( strlen(global_buffer_fase2) != 0 ){
        
        strcat(global_buffer_fase2, buffer ); //junto as duas
        memset(buffer, 0, buffer_big); //meto o buffer a 0
        strcpy( buffer, global_buffer_fase2 ); //copio para o buffer
        
        memset(global_buffer_fase2, 0, buffer_big);//meto o global a 0
    }
    

    aux_buffer = buffer;
    complete_message = strsep(&aux_buffer, "\n");
    

    for( ; complete_message != NULL; complete_message = strsep(&aux_buffer, "\n") ){

        last_message = complete_message; 

        if( strcmp(complete_message, "") == 0 ){
            break;
        }

        message_type = (char *) malloc(sizeof(char) * 20 + 1);
        message_id = (char *) malloc(sizeof(char) * 20 + 1);
        message_IP = (char *) malloc(sizeof(char) * 30 + 1);
        message_port = (char *) malloc(sizeof(char) * 20 + 1);
        
        sscanf( complete_message, "%s %s %s %s", message_type, message_id, message_IP, message_port );

        //Verifica de que tipo é a mensagem
        if( strcmp(message_type,"ENTRY") == 0){//Se a mensagem é dp tipo ENTRY

            if(fixed_node->connection_back == -1){ //Caso só exista um nó no anel
                
                //Informa o novo nó que o seu sucedor é ele próprio
                sprintf( message, "SUCC %s %s %s\n", fixed_node->id, fixed_node->IP, fixed_node->TCP);
                send_message_tcp(message, connection);
                
                //Atualiza que o seu back é o novo nó
                fixed_node->connection_back = connection;
                fixed_node->adjacencies[100] = connection;
                fixed_node->id_back = message_id;


                //Atualiza que o seu next é o novo nó
                new_connection = connect_clientTCP(message_IP, message_port);   
                fixed_node->connection_next = new_connection;
                fixed_node->adjacencies[atoi(message_id)] = new_connection;
                fixed_node->id_next = message_id;
                fixed_node->IP_next = message_IP;
                fixed_node->TCP_next = message_port;

                sprintf( message_2, "PRED %s\n", fixed_node->id);
                send_message_tcp(message_2, new_connection);

                //Atualiza que o seu nextnext é ele próprio
                fixed_node->id_nextnext= fixed_node->id;
                fixed_node->IP_nextnext = fixed_node->IP;
                fixed_node->TCP_nextnext = fixed_node->TCP;        
                
            }else{

                sprintf( message, "%s %s %s %s\n", message_type, message_id, message_IP, message_port);
                send_message_tcp(message, fixed_node->connection_back);

                sprintf( message_2, "SUCC %s %s %s\n", fixed_node->id_next, fixed_node->IP_next, fixed_node->TCP_next);
                send_message_tcp(message_2, connection);

                //Apagar a coluna da tabela de expedição do back antigo
                if(strcmp(fixed_node->id_back, fixed_node->id_next) == 0){
                    fixed_node->adjacencies[100] = -1;
                } else {

                    route_end_connection( fixed_node, fixed_node->connection_back );                    
                    //fixed_node->adjacencies[atoi(fixed_node->id_back)] = -1;

                }

                fixed_node->connection_back = connection;
                fixed_node->id_back = message_id;

                //Adiciona nova ligação à lista de adjacencias
                fixed_node->adjacencies[atoi(message_id)] = connection;


            }

            //Envia toda a tabela de caminhos mais curtos para a nova conexão
            send_all_table( fixed_node, connection);

        }else if( strcmp(message_type, "SUCC") == 0){

            //Não utilizado

        }else if( strcmp(message_type, "PRED") == 0){

            if( fixed_node->connection_back != -1){//Situação: Foi removida uma adjacencia 
                
                if( strcmp(message_id, fixed_node->id_next) == 0){//Situaçãp: só ficam dois nos

                    fixed_node->connection_back = connection;
                    fixed_node->adjacencies[100] = connection;
                }else {

                    fixed_node->connection_back = connection;
                    fixed_node->adjacencies[atoi(message_id)] = connection;
                }

                sprintf( message, "SUCC %s %s %s\n", fixed_node->id_next, fixed_node->IP_next, fixed_node->TCP_next);
                send_message_tcp(message, connection);

                free(fixed_node->id_back);

                fixed_node->id_back = message_id;


            }else{

                fixed_node->connection_back = connection;
                //fixed_node->id_back = message_id; //RAFAEL

                if(strcmp(message_id, fixed_node->id_next) == 0){
                    
                    fixed_node->id_back = fixed_node->id_next;//RAFAEL
                    fixed_node->adjacencies[100] = connection;

                    //Define que o nó nextnext é ele próprio
                    fixed_node->id_nextnext = fixed_node->id;
                    fixed_node->IP_nextnext = fixed_node->IP;
                    fixed_node->TCP_nextnext = fixed_node->TCP;

                    free(message_id); //RAFAEL

                } else {
                    fixed_node->id_back = message_id; //RAFAEL
                    fixed_node->adjacencies[atoi(message_id)] = connection;
                }

                if(fixed_node->join == 1){
                    
                    memset( message, 0, buffer_short);
                    sprintf( message, "REG %s %s %s %s", fixed_node->ring, fixed_node->id, fixed_node->IP, fixed_node->TCP);

                    if( send_UDP( message, NULL, regIP_base, regUDP_base) != 1){
                        printf("Erro comunicação UDP (leave)\n");
                        exit(1);
                    }

                    fixed_node->join = 0;
                }

            }

            //Envia toda a tabela de caminhos mais curtos para a nova conexão
            send_all_table( fixed_node, connection);

            free(message_IP);
            free(message_port);

        }else if( strcmp(message_type, "ROUTE") == 0 ){

            process_route( fixed_node, complete_message );

            free(message_id);
            free(message_IP);
            free(message_port);
            

        }else if( strcmp(message_type, "CHORD") == 0){


            if( fixed_node->adjacencies[atoi(message_id)] != -1){
                fixed_node->adjacencies[100] = connection;
            }else{
                fixed_node->adjacencies[atoi(message_id)] = connection;
            }

            free(message_id);
            free(message_IP);
            free(message_port);

            //Envia toda a tabela de caminhos mais curtos para a nova conexão
            send_all_table( fixed_node, connection);
            
        }else{
            printf("ERRO, foi recebida uma mensagem desconhecida via tcp\n");
            //exit(1);
        }

        free(message_type);
    }


    if( strlen( last_message ) != 0 ){ 
        
        strcpy( global_buffer_fase2, last_message);
    }

    
    free(aux_free); 
    return;
}


void message_from_next(int connection, struct node *fixed_node, char* IP_UDP, char *UDP){

    char buffer[buffer_big], message_2[buffer_short], message_3[buffer_short];
    char *message_type, *message_id, *message_IP, *message_port, *complete_message; 
    int nbyte_read;
    char *aux_buffer;
    char *aux_free;
    char *last_message; 

    complete_message = (char *) malloc(sizeof(char) * 50 + 1);
    aux_free = complete_message; 

    memset(buffer, 0, buffer_big);

    nbyte_read = read(connection,buffer,buffer_big);        
    if(nbyte_read <= 0){ 

        close_connection(fixed_node, connection);
        free(aux_free); 
        return;
    }


    if( strlen(global_buffer_next) != 0 ){
        
        strcat(global_buffer_next, buffer ); //junto as duas
        memset(buffer, 0, buffer_big); //meto o buffer a 0
        strcpy( buffer, global_buffer_next ); //copio para o buffer
        
        memset(global_buffer_next, 0, buffer_big);//meto o global a 0
    }


    aux_buffer = buffer;
    complete_message = strsep(&aux_buffer, "\n");


    for( ; complete_message != NULL; complete_message = strsep(&aux_buffer, "\n")){
        
        last_message = complete_message; 

        if( strcmp(complete_message, "") == 0 ){
            break;
        }

        message_type = (char *) malloc(sizeof(char) * 20 + 1);
        message_id = (char *) malloc(sizeof(char) * 20 + 1);
        message_IP = (char *) malloc(sizeof(char) * 30 + 1);
        message_port = (char *) malloc(sizeof(char) * 20 + 1);


        sscanf( complete_message, "%s %s %s %s", message_type, message_id, message_IP, message_port);

        //Verifica de que tipo é a mensagem
        if( strcmp(message_type,"ENTRY") == 0){

            //atualiza o seu sucessor


            //Fecha adjacencia com o no que estaria à frente
            if( strcmp( fixed_node->id_back, fixed_node->id_next) == 0 ){
                fixed_node->adjacencies[ atoi(fixed_node->id_back) ] = fixed_node->adjacencies[100];
                fixed_node->adjacencies[100] = -1;

            } else{

                route_end_connection( fixed_node, fixed_node->connection_next );
                //fixed_node->adjacencies[ atoi(fixed_node->id_next) ] = -1;

                //Remove da tabela a coluna da conexão que eu fechei em cima 
                
            }



            close(fixed_node->connection_next);


    
            //abre uma adjacencia com o novo no
            fixed_node->connection_next = connect_clientTCP(message_IP, message_port);
            //send_all_table no final da função    
            fixed_node->adjacencies[ atoi(message_id) ] = fixed_node->connection_next;


            //informa que será o precessor
            sprintf( message_3, "PRED %s\n", fixed_node->id);
            send_message_tcp(message_3, fixed_node->connection_next);

            //informa a tras do nó a sua frente
            sprintf( message_2, "SUCC %s %s %s\n", message_id, message_IP, message_port);
            send_message_tcp(message_2, fixed_node->connection_back);

            //atualiza o seu segundo sucessor
            fixed_node->id_nextnext = fixed_node->id_next;
            fixed_node->IP_nextnext = fixed_node->IP_next;
            fixed_node->TCP_nextnext= fixed_node->TCP_next;
            
            //atualiza o seu sucessor
            fixed_node->id_next = message_id;
            fixed_node->IP_next = message_IP;
            fixed_node->TCP_next = message_port;
            
            //Envia toda a tabela de caminhos mais curtos para a nova conexão
            send_all_table( fixed_node, fixed_node->connection_next);

        }else if( strcmp(message_type, "SUCC") == 0){

            //Estabelece o nó da mensagem como o seu segundo sucessor
            if( strcmp(fixed_node->id_next, message_id) != 0){
                fixed_node->id_nextnext = message_id;
                fixed_node->IP_nextnext = message_IP;
                fixed_node->TCP_nextnext= message_port;
            } else{ //RAFAEL
                free(message_id);
                free(message_IP);
                free(message_port);
            }
            

        }else if( strcmp(message_type, "PRED") == 0){
            
            //Não Utilizado

        }else if( strcmp(message_type, "CHORD") == 0){

            //Não utilizado

        }else if( strcmp(message_type, "ROUTE") == 0){

            process_route( fixed_node, complete_message );

            free(message_id);
            free(message_IP);
            free(message_port);

        }else if( strcmp(message_type, "CHAT") == 0){

            //chat_message_next(complete_message, fixed_node);
            chat_message( complete_message, fixed_node);

            free(message_id);
            free(message_IP);
            free(message_port);    
            

        }else{
            printf("ERRO, foi recebida uma mensagem desconhecida via tcp\n");
            //exit(1);
        }

        free(message_type);
        
    }

    if( strlen( last_message ) != 0 ){ 
        
        strcpy( global_buffer_next, last_message);
    }

    free(aux_free); 
    return;
}

void message_from_back(int connection, struct node *fixed_node){

    char buffer[buffer_big], message_2[buffer_short], message_3[buffer_short], *aux_buffer;
    char *message_type, *message_id, *message_IP, *message_port, *complete_message; 
    char *entry ="ENTRY", *succ = "SUCC", *chord = "CHORD";
    int nbyte_read;
    char *aux_free; 
    char *last_message;

    complete_message = (char *) malloc(sizeof(char) * 50 + 1);
    aux_free = complete_message; 

    memset(buffer, 0, buffer_big);
    memset(message_2, 0, buffer_short);
    memset(message_3, 0, buffer_short);

    nbyte_read = read(connection,buffer,buffer_big);
    if( nbyte_read <= 0){

        close_connection(fixed_node, connection);
        free(aux_free); 
        return;
    }

    
    if( strlen(global_buffer_back) != 0 ){
        
        strcat(global_buffer_back, buffer ); //junto as duas
        memset(buffer, 0, buffer_big); //meto o buffer a 0
        strcpy( buffer, global_buffer_back ); //copio para o buffer
        
        memset(global_buffer_back, 0, buffer_big);//meto o global a 0
    }

    aux_buffer =buffer;
    complete_message = strsep(&aux_buffer, "\n");


    for( ; complete_message != NULL; complete_message = strsep(&aux_buffer, "\n") ){

        last_message = complete_message;

        if( strcmp(complete_message, "") == 0 ){
            break;
        }

        message_type = (char *) malloc(sizeof(char) * 20 + 1);
        message_id = (char *) malloc(sizeof(char) * 20 + 1);
        message_IP = (char *) malloc(sizeof(char) * 30 + 1);
        message_port = (char *) malloc(sizeof(char) * 20 + 1);

        sscanf( complete_message, "%s %s %s %s", message_type, message_id, message_IP, message_port );;

        //Verifica de que tipo é a mensagem
        if( strcmp(message_type,entry) == 0){

            //Não utilizado

        }else if( strcmp(message_type, succ) == 0){

            //Não utilizado


        }else if( strcmp(message_type, "PRED") == 0){
            
            //Caso em que eu tenho uma corda
            if(fixed_node->connection_rope != -1){

                //Caso em que existe corda dupla
                if(fixed_node->adjacencies[100] != -1){

                    close(fixed_node->connection_rope);

                    fixed_node->connection_back = fixed_node->adjacencies[100];
                    fixed_node->adjacencies[ atoi(message_id) ] = fixed_node->connection_back;
                    fixed_node->adjacencies[100] = -1;

                    

                } else {
                    fixed_node->connection_back = fixed_node->connection_rope;
                }

                fixed_node->connection_rope = -1;
                free(fixed_node->id_rope);
                fixed_node->id_rope = NULL;

            } else{
                //Situação, só existia uma corda dele para mim
                fixed_node->connection_back = connection;
            }

            sprintf( message_2, "SUCC %s %s %s\n", fixed_node->id_next, fixed_node->IP_next, fixed_node->TCP_next);
            send_message_tcp(message_2, connection);

            free(fixed_node->id_back);

            fixed_node->id_back = message_id;



        }else if( strcmp(message_type, chord) == 0){

            //Não utilizado

        }else if( strcmp(message_type, "ROUTE") == 0){

            process_route( fixed_node, complete_message );

            free(message_id);
            free(message_IP);
            free(message_port);


        }else if( strcmp(message_type, "CHAT") == 0){

            //chat_message_next(complete_message, fixed_node);
            chat_message( complete_message, fixed_node);

            
            free(message_id);
            free(message_IP);
            free(message_port);

        }else{
            printf("ERRO, foi recebida uma mensagem desconhecida via tcp\n");
            //exit(1);
        }

        free(message_type);
    }

    if( strlen( last_message ) != 0 ){ //FIRME
        
        strcpy( global_buffer_back, last_message);
    }

    //RAFAEL
    free(aux_free);
    return;

}



void inicialize_global_vectors( ){

    memset( global_buffer_fase2, 0, buffer_big);
    memset( global_buffer_next, 0, buffer_big);
    memset( global_buffer_back, 0, buffer_big);
}




