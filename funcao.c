#include "struct.h"
#include "header.h"




void show_topology(struct node *no){
    printf("Informação sobre o predecessor [id]: %s\n", no->id_back);
    printf("Informação sobre ti próprio [id, IP, TCP]: %s, %s, %s\n", no->id, no->IP, no->TCP);
    printf("Informação sobre o sucessor [id, IP, TCP]: %s, %s, %s\n", no->id_next, no->IP_next, no->TCP_next);
    printf("Informação sobre o segundo sucessor [id, IP, TCP]: %s, %s, %s\n", no->id_nextnext, no->IP_nextnext, no->TCP_nextnext);

    printf("Informação sobre as cordas [id]:\n");
    for(int i = 0; i < ROWS; i++){
        if(no->adjacencies[i] != -1){
            if(no->adjacencies[i] != no->connection_next && no->adjacencies[i] != no->connection_back && no->adjacencies[i] != no->serverTCP){
                printf("%02d\n", i);

            }
        }
    }

}

// --------------------- JOIN ----------------------

struct node *comand_join(char *comando, char *IP_TCP, char *TCP, char *IP_UDP, char *UDP){

    char *ring, *id;
    char *aux1, *aux2;
    int size, i, ring_int , id_int;
    int pf1 = 0, pf2=0, pf3 = 0, pf4 =0;    

    struct node *fixed_node = NULL;
    
    ring = (char*) malloc( sizeof(char) * 5);
    id = (char*) malloc( sizeof(char) * 5);


    strtok(comando, " ");
    aux1 = strtok(NULL, " ");
    aux2 = strtok(NULL," \n");

    if( aux1 == NULL || aux2 == NULL ){
        printf("Necessário inserir os argumentos.\n");
        free(ring);
        free(id);
        return NULL;
    }

    //Validação anel
    size = strlen(aux1);
    for ( i = 0; i < size; i++ ){//Confirma se existem letras ou outros caracteres

        if( aux1[i] < 48 || aux1[i] > 57 ){
            pf1= 1;
        }
    }

    if( size > 3){//Verifica se é algo maior que 999
        pf2 = 1;
    }


    //Validação id

    size = strlen(aux2);
    for ( i = 0; i < size; i++ ){//Confirma se existem letras ou outros caracteres

        if( aux2[i] < 48 || aux2[i] > 57 ){
            pf3 = 1;
        }
    }

    if( size > 2){//Verifica se é algo maior que 99
        pf4 = 1;
    }


    //Erros para a consola

    if( pf1 != 0 || pf2 != 0 || pf3 != 0 || pf4 != 0){

        if( pf1 == 1 ){
            printf("O id do anel apenas pode conter caracteres numéricos.\n");
        }

        if( pf2 == 1 ){
            printf("Os ids dos aneis variam entre 0 e 999\n ");
        }

        if( pf3 == 1 ){
            printf("O id do nó apenas pode conter caracteres numéricos.\n");
        }

        if( pf4 == 1 ){
            printf("Os ids dos nó variam entre 0 e 99\n ");
        }

        free(ring);
        free(id);

        return NULL;
    }


    ring_int = atoi( aux1 );
    sprintf( ring, "%03d", ring_int );    


    id_int = atoi( aux2 );
    sprintf( id, "%02d", id_int );


    fixed_node = join(ring, id, IP_TCP, TCP, IP_UDP, UDP);

    return fixed_node; 
}


struct node *join(char *ring, char *id, char *IP_TCP, char *TCP, char *IP_UDP, char *UDP){
    

    //char message[buffer_short];
    char message[buffer_short], *message_return;

    char buffer[buffer_big], buffer_aux[buffer_big];
    
    char *lista_nos;
    char *id_aux, *IP_aux, *TCP_aux;

    int new_connection;

    struct node *fixed_node_new = NULL;

    memset(buffer, 0, buffer_big);


    //Define a mensagem do tipo "NODES ring"
    sprintf(message, "NODES %s", ring);

    if( send_UDP( message, &message_return, IP_UDP, UDP) != 3){
        printf("Erro comunicação UDP\n");
        exit(0);
    }
    
    strcpy(buffer, message_return);
    strcpy(buffer_aux, message_return);
    free(message_return);
    

    //Tira a linha de cabeçalho e obtem a primeira linha
    lista_nos = strtok(buffer_aux, "\n");
    lista_nos = strtok(NULL,"\n");
    

    if(lista_nos == NULL){ //Situação lista de nós vazia

        fixed_node_new = create_node();

        fixed_node_new->serverTCP = setup_serverTCP(IP_TCP, TCP);
        fixed_node_new->adjacencies[atoi(id)] = fixed_node_new->serverTCP;
        fixed_node_new->ring = ring;
        fixed_node_new->id = id;
        fixed_node_new->IP = IP_TCP;
        fixed_node_new->TCP = TCP;

        fixed_node_new->id_next = id;
        fixed_node_new->id_back = id;
        fixed_node_new->id_nextnext = id;

        fixed_node_new->IP_next = IP_TCP;
        fixed_node_new->IP_nextnext = IP_TCP;

        fixed_node_new->TCP_next = TCP;
        fixed_node_new->TCP_nextnext = TCP;

        fixed_node_new->join = 0;

        sprintf( message, "REG %s %s %s %s", fixed_node_new->ring, fixed_node_new->id, fixed_node_new->IP, fixed_node_new->TCP);
        if( send_UDP( message, NULL, IP_UDP, UDP) != 1){
            printf("Erro na confirmação de registo");
            exit(0);
        }

        initializeTable(fixed_node_new);


        return fixed_node_new;
    }else{

        id = validate_id( buffer, id);
        
        fixed_node_new = create_node();

        fixed_node_new->serverTCP = setup_serverTCP(IP_TCP, TCP);
        fixed_node_new->adjacencies[atoi(id)] = fixed_node_new->serverTCP;
        fixed_node_new->ring = ring;
        fixed_node_new->id = id;
        fixed_node_new->IP = IP_TCP;
        fixed_node_new->TCP = TCP;

        fixed_node_new->join = 1;

        id_aux = (char*) malloc( sizeof(char) * 20 );
        IP_aux = (char*) malloc( sizeof(char) * 20 );
        TCP_aux = (char*) malloc( sizeof(char) * 20 );

        //Tirar da lista de nós, a informação relevante (id, ip e porto)
        sscanf( lista_nos, "%s %s %s", id_aux, IP_aux, TCP_aux );

        new_connection = connect_clientTCP( IP_aux, TCP_aux );
        //send_all_table no final da função        

        sprintf( message, "ENTRY %s %s %s\n", id, IP_TCP, TCP);
        send_message_tcp(message, new_connection);

        fixed_node_new->connection_next = new_connection;
        fixed_node_new->adjacencies[atoi(id_aux)] = new_connection;
        fixed_node_new->id_next = id_aux;
        fixed_node_new->IP_next = IP_aux;
        fixed_node_new->TCP_next = TCP_aux;


        initializeTable(fixed_node_new);
        //Envia toda a tabela de caminhos mais curtos para a nova conexão
        send_all_table( fixed_node_new, new_connection);


        return fixed_node_new;
    }

}



//-------------DIRECT JOIN-------------------


struct node *comando_dj(char *comando, char *IP_TCP, char *TCP){

    char *id_ligar, *id_proprio, *IP_ligar, *TCP_ligar, *comando_aux;
    int size, i, id_proprio_int , id_ligar_int;
    int pf1 = 0, pf2=0, pf3 = 0, pf4 =0;

    struct node *node = NULL;
    
    
    id_ligar = (char *) malloc( sizeof(char) * 20);
    id_proprio = (char *) malloc( sizeof(char) * 20);
    IP_ligar = (char *) malloc( sizeof(char) * 20);
    TCP_ligar = (char *) malloc( sizeof(char) * 20);

    
    //Tira o "dj", e comando_aux fica com o resto
    strtok(comando, " ");
    comando_aux = strtok(NULL, "\n");
    
    if( comando_aux == NULL ){
        printf("Necessário inserir os todos os argumentos.\n");
        free(id_ligar);
        free(id_proprio);
        free(IP_ligar);
        free(TCP_ligar);
        return NULL;
    }


    sscanf(comando_aux, "%s %s %s %s", id_proprio, id_ligar, IP_ligar, TCP_ligar);

    if( id_proprio == NULL || id_ligar == NULL || IP_ligar == NULL || TCP_ligar == NULL ){
        printf("Necessário inserir os todos os argumentos.\n");
        free(id_ligar);
        free(id_proprio);
        free(IP_ligar);
        free(TCP_ligar);
        return NULL;
    }


    //Validação id_proprio
    size = strlen(id_proprio);
    for ( i = 0; i < size; i++ ){//Confirma se existem letras ou outros caracteres

        if( id_proprio[i] < 48 || id_proprio[i] > 57 ){
            pf1= 1;
        }
    }

    if( size > 2){
        pf2 = 1;
    }

    //Validação succ_id
    size = strlen(id_ligar);
    for ( i = 0; i < size; i++ ){//Confirma se existem letras ou outros caracteres

        if( id_ligar[i] < 48 || id_ligar[i] > 57 ){
            pf3= 1;
        }
    }

    if( size > 2){
        pf4 = 1;
    }

    //Erros para a consola

    if( pf1 != 0 || pf2 != 0 || pf3 != 0 || pf4 != 0){

        if( pf1 == 1 ){
            printf("O id do nó apenas pode conter caracteres numéricos. (id_próprio)\n");
        }

        if( pf2 == 1 ){
            printf("Os ids dos nós variam entre 0 e 99. (id_próprio)\n ");
        }
        
        if( pf3 == 1 ){
            printf("O id do nó apenas pode conter caracteres numéricos. (succ_id)\n");
        }

        if( pf4 == 1 ){
            printf("Os ids dos nós variam entre 0 e 99. (succ_id)\n ");
        }

        return NULL;
    }


    id_proprio_int = atoi( id_proprio );
    id_ligar_int =  atoi( id_ligar );

    sprintf( id_proprio, "%02d", id_proprio_int);
    sprintf( id_ligar, "%02d", id_ligar_int);


    node = direct_join(id_proprio, IP_TCP, TCP, id_ligar, IP_ligar, TCP_ligar);

    return node;
}



struct node *direct_join(char *id_novo, char *IP_TCP, char *TCP, char *id_ligar, char *IP_ligar, char *TCP_ligar){

    char mensagem[buffer_short];

    int new_connection;

    struct node *fixed_node_new = NULL;


    memset(mensagem, 0, buffer_short);

    if( strcmp(id_novo, id_ligar) == 0 ){ //Situação em que vou introduzir o primeiro nó no "anel"
        
        fixed_node_new = create_node();

        fixed_node_new->serverTCP = setup_serverTCP(IP_TCP, TCP);
        fixed_node_new->adjacencies[atoi(id_novo)] = fixed_node_new->serverTCP;
        fixed_node_new->ring = "-1";
        fixed_node_new->id = id_novo;
        fixed_node_new->IP = IP_TCP;
        fixed_node_new->TCP = TCP;

        fixed_node_new->id_next = id_novo;
        fixed_node_new->id_back = id_novo;
        fixed_node_new->id_nextnext = id_novo;

        fixed_node_new->IP_next = IP_TCP;
        fixed_node_new->IP_nextnext = IP_TCP;

        fixed_node_new->TCP_next = TCP;
        fixed_node_new->TCP_nextnext = TCP;

        initializeTable( fixed_node_new );

        return fixed_node_new;

    } else { //se existir pelo menos um nó

        fixed_node_new = create_node();

        fixed_node_new->serverTCP = setup_serverTCP(IP_TCP, TCP);
        fixed_node_new->adjacencies[atoi(id_novo)] = fixed_node_new->serverTCP;
        fixed_node_new->ring = "-1";
        fixed_node_new->id = id_novo;
        fixed_node_new->IP = IP_TCP;
        fixed_node_new->TCP = TCP;


        new_connection = connect_clientTCP( IP_ligar, TCP_ligar );
        //send_all_table no final da função    

        sprintf(mensagem, "ENTRY %s %s %s\n", id_novo, IP_TCP, TCP);
        send_message_tcp(mensagem, new_connection);

        fixed_node_new->connection_next = new_connection;
        fixed_node_new->adjacencies[atoi(id_ligar)] = new_connection;
        fixed_node_new->id_next = id_ligar;
        fixed_node_new->IP_next = IP_ligar;
        fixed_node_new->TCP_next = TCP_ligar;


        initializeTable( fixed_node_new );
        //Envia toda a tabela de caminhos mais curtos para a nova conexão
        send_all_table( fixed_node_new, new_connection);

        return fixed_node_new;
    }

}


//------------------------------LEAVE--------------------------


void leave(struct node *fixed_node, char *IP_UDP, char *UDP){

    char message[buffer_short], *message_return;
    int i,connection;

    memset(message, 0, buffer_short);


    if(fixed_node->join == 0){
        sprintf(message, "UNREG %s %s", fixed_node->ring, fixed_node->id);

        if( send_UDP( message, &message_return, IP_UDP, UDP) != 2){
            printf("Erro comunicação UDP (leave)\n");
            exit(0);
        }
        fixed_node->join = -1;
    }

    for( i = 0; i < ROWS_ADJ; i++){

        connection = fixed_node->adjacencies[i];

        if( connection != -1 ){
       
            close(connection);
            fixed_node->adjacencies[i] = -1;
            
        }
    }

    //Falta dar free ao fixed node!!!

    return;
}



void close_connection(struct node *fixed_node, int connection){

    char message[buffer_short];
    int colum_to_remove;
    int new_connection;
    
    if(connection == fixed_node->connection_next ){

        colum_to_remove = atoi(fixed_node->id_next);

        fixed_node->connection_next = -1;
        //fixed_node->adjacencies[ atoi(fixed_node->id_next) ] = -1;
        free( fixed_node->id_next);
        free(fixed_node->IP_next);
        free(fixed_node->TCP_next);
        fixed_node->id_next = NULL;
        fixed_node->IP_next = NULL;
        fixed_node->TCP_next = NULL;

        
        if( strcmp(fixed_node->id_nextnext, fixed_node->id) != 0 ){ //se estiverem pelo menos tres nos

            if( fixed_node->adjacencies[ atoi(fixed_node->id_nextnext)] != -1 && strcmp(fixed_node->id_nextnext, fixed_node->id_back) != 0){
                
                //Passar a corda para ligação ao next

                //Tenho corda dupla ou tenho corda para ele, Vou manter a minha corda
                if(fixed_node->connection_rope != -1){
                    fixed_node->connection_next = fixed_node->connection_rope;

                    fixed_node->connection_rope = -1;
                    free(fixed_node->id_rope);
                    fixed_node->id_rope = NULL;

                    //Vou eliminar o descritor da corda dupla caso exista
                    if(fixed_node->adjacencies[100] != -1){
                        fixed_node->adjacencies[100] = -1;
                    }

                } else { //Caso em que não tenho corda para ele
                    fixed_node->connection_next = fixed_node->adjacencies[ atoi(fixed_node->id_nextnext) ];
                }

                fixed_node->id_next = fixed_node->id_nextnext;
                fixed_node->IP_next = fixed_node->IP_nextnext;
                fixed_node->TCP_next = fixed_node->TCP_nextnext;


                //Remove a coluna da adjacencia que foi fechada e informa as adjacencias abertas (não informa a nova adjacencia)        
                route_end_connection( fixed_node, connection );
            
                fixed_node->id_nextnext = NULL;
                fixed_node->IP_nextnext = NULL;
                fixed_node->TCP_nextnext = NULL;

                memset(message, 0, buffer_short);
                sprintf( message, "SUCC %s %s %s\n", fixed_node->id_next, fixed_node->IP_next, fixed_node->TCP_next);
                send_message_tcp(message, fixed_node->connection_back);

                memset(message, 0, buffer_short);
                sprintf( message, "PRED %s\n", fixed_node->id);
                send_message_tcp(message, fixed_node->connection_next);
                
                //Envia toda a tabela de caminhos mais curtos para a nova conexão
                send_all_table( fixed_node, fixed_node->connection_next);
                

            } else {
                //O que já estava a ser feito

                new_connection = connect_clientTCP( fixed_node->IP_nextnext, fixed_node->TCP_nextnext );
                //send_all_table no final da função    

                fixed_node->connection_next = new_connection;

                fixed_node->id_next = fixed_node->id_nextnext;
                fixed_node->IP_next = fixed_node->IP_nextnext;
                fixed_node->TCP_next = fixed_node->TCP_nextnext;


                //Remove a coluna da adjacencia que foi fechada e informa as adjacencias abertas (não informa a nova adjacencia)        
                route_end_connection( fixed_node, connection );
            
                
                if(strcmp(fixed_node->id_back, fixed_node->id_nextnext) == 0){ //Situação: Estão tres nós e vão passar a ficar apenas dois (uma das ligações para para o adjacencies[100])
                    fixed_node->adjacencies[100] = fixed_node->adjacencies[ atoi(fixed_node->id_back) ];
                    fixed_node->adjacencies[ atoi(fixed_node->id_nextnext) ] = new_connection;
                } else {
                    fixed_node->adjacencies[ atoi(fixed_node->id_nextnext) ] = new_connection; 
                }



                fixed_node->id_nextnext = NULL;
                fixed_node->IP_nextnext = NULL;
                fixed_node->TCP_nextnext = NULL;

                memset(message, 0, buffer_short);
                sprintf( message, "SUCC %s %s %s\n", fixed_node->id_next, fixed_node->IP_next, fixed_node->TCP_next);
                send_message_tcp(message, fixed_node->connection_back);

                memset(message, 0, buffer_short);
                sprintf( message, "PRED %s\n", fixed_node->id);
                send_message_tcp(message, fixed_node->connection_next);
                
                //Envia toda a tabela de caminhos mais curtos para a nova conexão
                send_all_table( fixed_node, new_connection);

            }

        }else{ //situação que estavam dois nós e saiu um, fica so um no

            fixed_node->id_next = fixed_node->id;
            fixed_node->IP_next = fixed_node->IP;
            fixed_node->TCP_next = fixed_node->TCP;

            fixed_node->adjacencies[ colum_to_remove ] = -1;

            fixed_node->id_back = fixed_node->id;
            

            //Remove a coluna da adjacencia que foi fechada e informa as adjacencias abertas (neste caso ninguém porque não tem adjacencias)        
            //remove_table( fixed_node, colum_to_remove );
            initializeTable(fixed_node);
         
        }


    }else if( connection == fixed_node->connection_back){


        if(strcmp(fixed_node->id_next, fixed_node->id_back) == 0){//Situação: Só estão dois nós e passa a estar só 1
            
            fixed_node->adjacencies[100] = -1;

            if( strcmp(fixed_node->id, fixed_node->id_nextnext) == 0){
                fixed_node->connection_back = -1;
            }

        }else {
            //fixed_node->adjacencies[ atoi(fixed_node->id_back) ] = -1;
            route_end_connection( fixed_node, connection );
        }


    }else if( connection == fixed_node->connection_rope ){
        //Fazer o remove chord

        colum_to_remove = atoi(fixed_node->id_rope);

        remove_chord(fixed_node);

        //Só remove a linha da tabela caso deixe de existir ligação (se existia corda reciproca não elimina coluna)
        if( fixed_node->adjacencies[colum_to_remove] == -1 ){
            //route_end_connection( fixed_node,  connection );
        }


    }else if( connection == fixed_node->adjacencies[100]){
        
        fixed_node->adjacencies[100] = -1;

    }else{
        //Aqui só chegaram as cordas + situação de quebrar a ligação quando só tão dois nós
        for( int i = 0; i < ROWS; i++){
            if( connection == fixed_node->adjacencies[i]){

                //fixed_node->adjacencies[i] = -1;
                
                route_end_connection( fixed_node, connection );

                break;
            }
        }
    }

    return;
}





//------------------------------CHORD------------------------------------------------------------------------------

void add_chord(struct node *fixed_node){

    char message[buffer_short];
    char *nodes_list;
    char *line;
    char *id_next, *id_back;
    char  *message_id, *message_IP, *message_port; 


    if( fixed_node->join == -1 ){
        printf("Não é possivel adicionar cordas num nó inserido por dj.\n");
        return;
    }

    id_next = fixed_node->id_next;
    id_back = fixed_node->id_back;

    memset(message, 0, buffer_short);
    sprintf(message, "NODES %s\n", fixed_node->ring);

    if( send_UDP( message, &nodes_list, regIP_base, regUDP_base) != 3){
        printf("Erro na receção da lista de nós (add_chord)");
        return;
    }

    strtok(nodes_list, "\n");//Remove cabeçalho
    line = strtok(NULL, "\n");

    for( ; line != NULL; line = strtok(NULL, "\n") ){

        if( ( id_next[0] != line[0] ) || ( id_next[1] != line[1] ) ){ //Se passar este if, significa que a nova corda não é para o next
            
            if( ( id_back[0] != line[0] ) || ( id_back[1] != line[1] ) ){//Se passar este if, significa que a nova corda não é para o back

                if( ( fixed_node->id[0] != line[0] ) || ( fixed_node->id[1] != line[1] ) ){//Se passar este if, significa que a nova corda não é para ele próprio

                    break;
                }
            }

        }

    }
    
    if( line == NULL ){
        printf("Erro. Só é permitido inserir cordas em aneis com mais do que 3 nós.\n");
        free(nodes_list);
        return;
    }

    if( fixed_node->connection_rope == -1){

        message_id = (char *) malloc(sizeof(char) * 20 + 1);
        message_port = (char *) malloc(sizeof(char) * 20 + 1);
        message_IP = (char *) malloc(sizeof(char) * 30 + 1);

        //Retira as informações do nó que se vai ligar a corda
        sscanf( line, "%s %s %s",  message_id, message_IP, message_port);


        fixed_node->id_rope = message_id;
        fixed_node->connection_rope = connect_clientTCP( message_IP, message_port);
        //send_all_table no final da função    


        if( fixed_node->adjacencies[atoi(message_id)] != -1){

            fixed_node->adjacencies[100] = fixed_node->adjacencies[atoi(message_id)];
        }
        fixed_node->adjacencies[atoi(message_id)] = fixed_node->connection_rope;

        //Define a mensagem do tipo "CHORD id" e envia para a nova corda
        memset(message, 0, buffer_short);
        sprintf(message, "CHORD %s\n", fixed_node->id);
        send_message_tcp(message, fixed_node->connection_rope);

        //Envia toda a tabela de caminhos mais curtos para a nova conexão
        send_all_table( fixed_node, fixed_node->connection_rope);

        free(message_IP);
        free(message_port);
        free(nodes_list);


    }else{
        printf("Já existe uma corda adicionada.\nTente outro comando!\n");
    }
}


void remove_chord(struct node *fixed_node){

    if( fixed_node->connection_rope != -1){

        close(fixed_node->connection_rope);

        if( fixed_node->adjacencies[100] != -1){// tem uma corda dupla

            fixed_node->adjacencies[atoi(fixed_node->id_rope)] = fixed_node->adjacencies[100];
            fixed_node->adjacencies[100] = -1;
        }else{

            route_end_connection( fixed_node, fixed_node->connection_rope );
        }

        fixed_node->connection_rope = -1;

        free(fixed_node->id_rope);
        fixed_node->id_rope = NULL;

    }else{
        printf("Não é possível remover uma corda que não existe.\n Tenta outro comando!\n");
    }


}



void show_fowarding( struct node *fixed_node){

    for( int i = 0; i < COLS; i++){
        if( strcmp(fixed_node->shipping_table[i], "-1") != 0){

            printf("%02d\t%s\n", i, fixed_node->shipping_table[i]);
            
        }

        
    }
}

void show_path( struct node *fixed_node, char *comand ){

    char *dest;

    strtok(comand, " ");
    dest = strtok(NULL, "\n");


    if(strlen(dest) != 2){
        printf("Erro, formatação errada. Deve conter dois digitos EX. 01\n");
        return;
    }

    if( strcmp( fixed_node->short_path[atoi(dest)] , "-1") == 0 || strcmp( fixed_node->short_path[atoi(dest)] , "-") == 0 ){
        
        printf("Não contém caminho para o nó selecionado\n");
        return;
    }

    printf("O caminho mais curto de %s para %s é: %s\n", fixed_node->id, dest, fixed_node->short_path[atoi(dest)]);
    
    return;
}

//Mostra a linha toda
void show_routing( struct node *fixed_node, char *comand){

    char *dest;

    strtok(comand, " ");
    dest = strtok(NULL, "\n");


    if(strlen(dest) != 2){
        printf("Erro, formatação errada. Deve conter dois digitos EX: 01\n");
        return;
    }

    printf("Nó %s: ", fixed_node->id);

    for( int i = 0; i < ROWS; i++ ){

        
        if( fixed_node->adjacencies[i] != -1 && i != atoi(fixed_node->id)){
            
            printf("\t%s", fixed_node->table[atoi(dest)][i]);
        }
    }
    printf("\n");
    return;
}


void command_chat( struct node *fixed_node, char *comand){
    //Processa a linha de comando a mensagem " m dest message"

    char *dest, *message, *message_type;
    char response[buffer_big];

    message = (char *) malloc(sizeof(char) * buffer_short + 1);
    dest = (char *) malloc(sizeof(char) * 20);
    message_type = (char *) malloc(sizeof(char) * 20 + 1);

    sscanf( comand, "%s %s %s", message_type, dest, message );

    if(strcmp(dest, fixed_node->id) == 0){
        printf("Mensagem Recebida de %s: %s\n", fixed_node->id, message);

    } else {
        sprintf( response, "CHAT %s %s %s\n", fixed_node->id, dest, message);
        send_message_tcp(response, fixed_node->adjacencies[ atoi(fixed_node->shipping_table[atoi(dest)] )] );
    }

    free(message);
    free(dest);
    free(message_type);

}


void chat_message( char *comand, struct node *fixed_node){
    //Recebe em comand algo do tipo "CHAT i n chat\n"
    char *message, *message_type, *id_origin, *id_dest;
    char response[buffer_big];

    message_type = (char *) malloc(sizeof(char) * 20 + 1);
    id_origin = (char *) malloc(sizeof(char) * 20 + 1);
    id_dest = (char *) malloc(sizeof(char) * 30 + 1);
    message = (char *) malloc(sizeof(char) * buffer_short + 1);

    sscanf( comand, "%s %s %s %s", message_type, id_origin, id_dest, message );

    if( strcmp( fixed_node->id, id_dest) == 0){

        printf( "Mensagem Recebida de %s: %s\n", id_origin, message);
    }else{

        sprintf( response, "CHAT %s %s %s\n", id_origin, id_dest, message);
        send_message_tcp(response, fixed_node->adjacencies[ atoi(fixed_node->shipping_table[atoi(id_dest)] )] );
    }
    
    free(message_type);
    free(id_origin);
    free(id_dest);
    free(message);

}



////////////////////////// MESSAGE SEGURO //////////////////////////////////////////


void command_chat_next( struct node *fixed_node, char *comand){
    //Processa a linha de comando a mensagem " m dest message"

    char *dest, *message, *message_type;
    char response[buffer_big];

    message = (char *) malloc(sizeof(char) * buffer_short + 1);
    dest = (char*) malloc(sizeof(char) * 20);
    message_type = (char *) malloc(sizeof(char) * 20 + 1);

    sscanf( comand, "%s %s %s", message_type, dest, message );

    if(strcmp(dest, fixed_node->id) == 0){
        printf("Mensagem Recebida de %s: %s\n", fixed_node->id, message);

    } else {

        sprintf( response, "CHAT %s %s %s\n", fixed_node->id, dest, message);
        send_message_tcp(response, fixed_node->connection_next );
    }

    free(message);
    free(dest);
    free(message_type);

}


void chat_message_next( char *comand, struct node *fixed_node){
    //Recebe em comand algo do tipo "CHAT i n chat\n"
    char *message, *message_type, *id_origin, *id_dest;
    char response[buffer_big];

    message_type = (char *) malloc(sizeof(char) * 20 + 1);
    id_origin = (char *) malloc(sizeof(char) * 20 + 1);
    id_dest = (char *) malloc(sizeof(char) * 30 + 1);
    message = (char *) malloc(sizeof(char) * buffer_short + 1);

    sscanf( comand, "%s %s %s %s", message_type, id_origin, id_dest, message );

    if( strcmp( fixed_node->id, id_dest) == 0){

        printf( "Mensagem Recebida de %s: %s\n", id_origin, message);
    }else{

        sprintf( response, "CHAT %s %s %s\n", id_origin, id_dest, message);
        send_message_tcp(response, fixed_node->connection_next );
    }
    
    free(message_type);
    free(id_origin);
    free(id_dest);
    free(message);

}


void free_node(struct node *fixed_node){

    if(fixed_node != NULL){
        if(fixed_node->ring != NULL && strcmp(fixed_node->ring, "-1") != 0){ 
            free(fixed_node->ring);
        }

        //Caso em que estavam 3 ou mais nós no anel
        if(fixed_node->id_nextnext != NULL && strcmp(fixed_node->id_nextnext, fixed_node->id) != 0){
            free(fixed_node->id);
            free(fixed_node->id_nextnext);
            free(fixed_node->IP_nextnext);
            free(fixed_node->TCP_nextnext);

            free(fixed_node->id_back);
            free(fixed_node->id_next);
            free(fixed_node->IP_next);
            free(fixed_node->TCP_next);

        } else if( strcmp(fixed_node->id_back, fixed_node->id_next) == 0 && strcmp(fixed_node->id, fixed_node->id_back) != 0){ //Caso em que estão 2 nós no anel
            
            free(fixed_node->id);
            free(fixed_node->id_next);
            free(fixed_node->IP_next);
            free(fixed_node->TCP_next);

        } else { //Caso em que só está um nó no anel
            free(fixed_node->id);
        }


        if(fixed_node->id_rope != NULL){
            free(fixed_node->id_rope);
        }

        free(fixed_node);
    }

}