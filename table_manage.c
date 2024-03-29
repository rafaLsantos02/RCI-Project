
#include "struct.h"
#include "header.h"


// Função para inicializar as tabelas 
void initializeTable(struct node *fixed_node) {

    int i, j;

    //Inicializar table (tabela de encaminhamento) a -1
    for ( i = 0; i < ROWS; i++ ){
        for( j = 0; j < COLS; j++ ){
            memset(fixed_node->table[i][j], 0, buffer_short);
            strcpy( fixed_node->table[i][j], "-1");
        }
    }

    //Coloca a célula com origem e destino do próprio nó a "-"
    strcpy( fixed_node->table[ atoi( fixed_node->id ) ][ atoi( fixed_node->id ) ], "-");
    //strcpy( fixed_node->table[ atoi( fixed_node->id ) ][ atoi( fixed_node->id ) ], fixed_node->id);


    //Inicializar short_path (tabela caminhos mais curtos) a -1
    for(i = 0; i < COLS; i++){

        memset(fixed_node->short_path[i], 0, buffer_short);

        strcpy(fixed_node->short_path[i], "-1");
    }

    //Define o caminho mais curto para o próprio nó a "-"
    //strcpy( fixed_node->short_path[ atoi( fixed_node->id ) ], "-");
    strcpy( fixed_node->short_path[ atoi( fixed_node->id ) ], fixed_node->id);


    //Inicializar shipping_table (tabela de expedição) a "-1"
    for( i = 0; i < COLS; i++ ){

        memset(fixed_node->shipping_table[i], 0, 3);

        strcpy(fixed_node->shipping_table[i], "-1");
    }

    //Define o que caminho mais curto para o próprio nó deve seguir "-" (não existe)
    strcpy( fixed_node->shipping_table[ atoi( fixed_node->id ) ], "-" );

    return;
}


// Função para exibir toda a tabela
void printTable(struct node *fixed_node) {
    
    int i, j, content = 0;
    char i_aux[3];

    printf("Tabela de Encaminhamento:\n\t");//Título

    //ALTEREI AQUI ROWS_ADJ para ROWS
    for( i = 0; i < ROWS; i++){//print da primeira linha da tabela (adajecencias que existem)

        if( fixed_node->adjacencies[i] != -1){

            sprintf(i_aux, "%02d", i);
            if(strcmp(i_aux, fixed_node->id) == 0){
                continue;
            } 

            printf("%d\t", i);
        }
    }
    printf("\n");


    
    for ( i = 0 ; i < ROWS ; i++ ){
        for ( j = 0 ; j < COLS ; j++ ){

            if ( strcmp(fixed_node->table[i][j], "-1") != 0){
                
                if(content == 0){
                    j = -1;
                    printf("%d\t", i);
                    content = 1;
                    continue;
                }

                printf("%s\t", fixed_node->table[i][j]);
            }
        }
        if(content != 0){
            printf("\n");
            content = 0;
        }
        
    }
   return;
}



void process_route( struct node *fixed_node, char *message_route ){

    char *message_copy, *message_copy_2;
    char *origin, *dest, *path;
    char new_path[buffer_short];
    int origin_int, dest_int, line_updated = 0;
    int i, invalid_path = 0;



    message_copy = strdup( message_route );
    if (message_copy == NULL) {
        printf("Erro memoria (process_route)");
        exit(1);
    }

    message_copy_2 = strdup( message_route );
    if (message_copy == NULL) {
        printf("Erro memoria (process_route)");
        exit(1);
    }

    strtok( message_copy, " "); //Remove "ROUTE" da string

    origin = strtok(NULL, " ");
    dest = strtok(NULL, " ");

    origin_int = atoi( origin );
    dest_int = atoi( dest );


    path = strtok( NULL, "-\n" ); // Verificar se a mensagem recebida tem caminho

    if( path == NULL ){ //Situação: existe caminho em message_route

        // message_route é do tipo "ROUTE xx xx\n" ou "ROUTE xx yy\n"

        if( origin_int == dest_int ){ // Tipo "ROUTE xx xx\n"  -> o nó xx estabeleceu ligação "comigo"

            sprintf( new_path, "%s-%s", fixed_node->id, dest );

            strcpy( fixed_node->table[ dest_int ][ dest_int ], new_path );

            //Verifica se a tabela de caminhos mais curtos é atualizada
            line_updated = update_short_path( fixed_node, dest_int );
            if( line_updated == 1 ){

                send_updated_path( fixed_node, dest_int );
                line_updated = 0;
            }


        }else{ // Tipo "ROUTE xx yy\n"  -> foi fechada a ligação xx-yy

            if( strcmp( fixed_node->id, dest ) == 0 ){ //Situação: O nó xx fechou a ligação "comigo"

                for( i = 0; i < COLS; i++ ){//Coloca a coluna "origin" toda a -1

                    strcpy( fixed_node->table[ i ][ origin_int ], "-1" );
                }

                strcpy( fixed_node->table[ origin_int ][ dest_int ], "-" );
                
                //Envia todas as atualizações de caminhos mais curtos (se houver) 
                //Atenção: Precisa de fazer isto?
                update_all_shortpath( fixed_node );

            }else{//Situação: Foi fechada a ligação xx-yy , que não é uma ligação direta a mim
                
                //ola
                if( strcmp(fixed_node->table[ origin_int ][ dest_int ], "-1" ) != 0){

                    strcpy( fixed_node->table[ origin_int ][ dest_int ], "-" ); //Atualiza a célula [xx][yy] para "-"

                    //Verifica se a tabela de caminhos mais curtos é atualizada
                    line_updated = update_short_path( fixed_node, origin_int );
                    if( line_updated == 1 ){

                        send_updated_path( fixed_node, origin_int );
                        line_updated = 0;
                    }
                    strcpy( fixed_node->table[ origin_int ][ dest_int ], "-1" );   

                }


                if( strcmp(fixed_node->table[ dest_int ][  origin_int ], "-1" ) != 0){

                    strcpy( fixed_node->table[ dest_int ][ origin_int ], "-" ); //Atualiza a célula [yy][xx] para "-"

                    //Verifica se a tabela de caminhos mais curtos é atualizada
                    line_updated = update_short_path( fixed_node, dest_int );
                    if( line_updated == 1 ){

                        send_updated_path( fixed_node, dest_int );
                        line_updated = 0;
                    }

                    strcpy( fixed_node->table[ dest_int ][ origin_int ], "-1" );

                }

            }

        }




    }else{ //Existe caminho em message_route
        // message_route é do tipo "ROUTE xx yy path\n"


        //Verifica se a mensagem ROUTE, tem o teu próprio id ou não.  Se não tiver -> é um caminho valido   Se tiver -> é um caminho invalido
        for( ; path != NULL; path = strtok(NULL, "-\n")){
            if( strcmp(fixed_node->id, path ) == 0){
                //Encontrou o seu id na mensagem
                invalid_path = 1;
                break;
            }
        }


        if( invalid_path != 1 ){ //Situação: é um caminho válido

            //Retira o caminho completo da message
            strtok(message_copy_2, " ");
            strtok(NULL, " ");
            strtok(NULL, " ");
            path = strtok(NULL, "\n");

            sprintf( new_path, "%s-%s", fixed_node->id, path );

            strcpy(fixed_node->table[ dest_int ][ origin_int ], new_path );

            //Verifica se a tabela de caminhos mais curtos é atualizada
            line_updated = update_short_path( fixed_node, dest_int );
            if( line_updated == 1 ){

                send_updated_path( fixed_node, dest_int );
                line_updated = 0;
            }

        }else{//Situação: é um caminho inválido

            strcpy(fixed_node->table[ dest_int ][ origin_int ], "-" );

        }


    }

    free(message_copy); //RAFAEL
    free(message_copy_2); //RAFAEL

    return;
}





//Esta função deve ser invocada quando foi escrito algo numa dada linha "line"

//Retorna 1 se o o short_path[ line ] foi alterado -> significa que tenho que avisar as adjacencias
//Retorna 0 se o o short_path[ line ] não foi alterado
//Retorna -1 se a linha estiver toda a "-1", mas o short_path[ line ] não está a -1     (ERRO??/ é possível?)

int update_short_path( struct node *fixed_node, int line ){
    
    int i;
    int detect = 0, size_aux;
    char min_path[buffer_short+1];

    memset( min_path, '-', buffer_short); //RAFAEL
    min_path[buffer_short] = '\0'; //RAFAEL
    size_aux = strlen( min_path );


    for( i = 0; i < COLS; i++){

        if( strcmp( fixed_node->table[ line ][ i ], "-1" ) != 0 ){

            if( strcmp( fixed_node->table[ line ][ i ], "-" ) == 0 ){

                detect = 1;
            }else{

                if( strlen( min_path ) > strlen( fixed_node->table[ line ][ i ] ) ){

                    strcpy( min_path, fixed_node->table[ line ][ i ] );

                }else if( strlen( min_path ) == strlen( fixed_node->table[ line ][ i ] ) ){

                    if( strcmp( fixed_node->table[ line ][ i ], fixed_node->short_path[ line ] ) == 0 ){
                        
                        strcpy( min_path, fixed_node->table[ line ][ i ] );
                    }
                }
            }
        }
    }

    if( strlen( min_path ) == size_aux ){ //Situação: não existe qualquer caminho na linha "line"

        if( detect == 1 ){ //Situação: Se na linha existe "-", copia para min_path "-"

            strcpy( min_path, "-" );

            for( i = 0; i < COLS; i++ ){

                strcpy( fixed_node->table[ line ][ i ], "-1" );
                //strcpy( fixed_node->short_path[ line ], "-1" );
            }

        }

    }

    if( strlen( min_path ) != size_aux ){ //Situação: o conteudo de min_path foi alterado

        if( strcmp( fixed_node->short_path[ line ], min_path ) !=  0 ){

            strcpy( fixed_node->short_path[ line ], min_path );
            return 1;
        
        }else{
            return 0;
        }
    }

    if( strcmp( fixed_node->short_path[ line ], "-1" ) == 0 ){

        return -1;
    }

    return -1;

}




//Envia os caminhos mais curtos que foram atualizados
void send_updated_path( struct node *fixed_node, int line_updated ){

    int i;
    char message[buffer_big];

    update_shipping_table( fixed_node, line_updated );

    memset(message, 0, buffer_big);

    if( strcmp( fixed_node->short_path[line_updated], "-" ) != 0){

        sprintf(message, "ROUTE %s %02d %s\n", fixed_node->id, line_updated, fixed_node->short_path[line_updated]);

    }else{
        
        sprintf(message, "ROUTE %s %02d\n", fixed_node->id, line_updated);
        strcpy( fixed_node->short_path[ line_updated ], "-1" );//ola
    }

    if( fixed_node->id_back == NULL){
        fixed_node->id_back = "-";
    }
    if( fixed_node->id_next == NULL){
        fixed_node->id_next = "-";
    }
    if( strcmp( fixed_node->id_back, fixed_node->id_next) == 0){//Situação: Existem apenas dois nós (envia apenas uma mensagem)

        send_message_tcp( message, fixed_node->adjacencies[ atoi( fixed_node->id_next ) ] );


    }else{ //Situação: Existem pelo menos 3 nós (envia mensagem para todas as adjacencias)

        for( i = 0; i < ROWS_ADJ; i++ ){

            if( fixed_node->adjacencies[i] != -1 && atoi( fixed_node->id ) != i ){

                send_message_tcp( message, fixed_node->adjacencies[i] );
            }
        }

    }

}



//Verifica se cada linha da tabela vai ter um shortpath novo
//SE estiver, envia logo a informar que atualizou o caminho mais curtos para todas as adjacencias
void update_all_shortpath( struct node *fixed_node ){

    int i;
    int line_updated = 0;

    for( i = 0; i < COLS ; i++ ){

        if( i == atoi( fixed_node->id ) ){
            continue;
        }
        
        line_updated = 0;
        line_updated = update_short_path( fixed_node, i );

        if( line_updated == 1 ){

            send_updated_path( fixed_node, i );
        }
    }

}




//Envia a tabela de caminhos mais curtos toda
void send_all_table( struct node *fixed_node, int connection){

    int i;
    char message[buffer_big];

    for( i = 0; i < ROWS; i++ ){

        if( strcmp( fixed_node->short_path[i], "-1" ) != 0 ){
            
            memset( message, 0, buffer_big);

            if( strcmp( fixed_node->short_path[i], "-" ) != 0 ){

                //Envia mensagem do tipo "ROUTE xx yy zz"
                sprintf( message, "ROUTE %s %02d %s\n", fixed_node->id, i, fixed_node->short_path[i] );
                send_message_tcp( message, connection );

            }
        }
    }

    return;
}



//Envia um ROUTE xx yy a informar que a conexão xx-yy foi fechada
void route_end_connection( struct node *fixed_node, int connection_closed ){

    int i, id_removed = -1; //RAFAEL
    char message[buffer_big];

    memset(message, 0, buffer_big);


    for( i = 0; i < ROWS_ADJ; i++ ){

        if( fixed_node->adjacencies[i] == connection_closed ){
            id_removed = i;
            break;
        }
    }

    //Salvaguarda
    if(id_removed == -1){
        return;
    }

    fixed_node->adjacencies[id_removed] = -1;

    sprintf( message, "ROUTE %s %02d\n", fixed_node->id, id_removed );

    //if( strcmp( fixed_node->id_back, fixed_node->id_next) == 0){//Situação: Existem apenas dois nós (envia apenas uma mensagem)

        //send_message_tcp( message, fixed_node->adjacencies[ atoi( fixed_node->id_next ) ] );


    //}else{ //Situação: Existem pelo menos 3 nós (envia mensagem para todas as adjacencias)

        for( i = 0; i < ROWS_ADJ; i++ ){

            if( fixed_node->adjacencies[i] != -1 && atoi( fixed_node->id ) != i ){

                send_message_tcp( message, fixed_node->adjacencies[i] );
            }
        }

    //}

    for( i = 0; i < COLS; i++ ){//Coloca a coluna "origin" toda a -1

        strcpy( fixed_node->table[ i ][ id_removed ], "-1" );
    }


    update_all_shortpath( fixed_node );


}


void update_shipping_table( struct node *fixed_node, int line_updated ){


    char path[buffer_short];
    char *aux;

    memset( path, 0, buffer_short);

    strcpy( path, fixed_node->short_path[line_updated] );

    aux = strtok( path, "-" );

    if( aux != NULL ){
        aux = strtok( NULL, "-\0" );
    }

    if( aux != NULL ){

        strcpy( fixed_node->shipping_table[line_updated], aux );
    }else{
        strcpy( fixed_node->shipping_table[line_updated], "-1" );
    }

}


