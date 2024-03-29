#include "struct.h"
#include "header.h"


void validate_arguments( int argc, char *argv[], char **IP, char **TCP, char **regIP, char **regUDP ){
    
    if (argc < 3 || argc > 5) {
        printf("Comando inválido, inserir na seguinte ordem COR IP TCP regIP regUDP, onde os ultimos 2 são opcionais.\n");
        exit(1);
    }

    *IP = argv[1];
    *TCP = argv[2];
    
    if (argc == 4) {
        *regIP = argv[3];
        *regUDP = regUDP_base;
    } else if (argc == 5) {
        *regIP = argv[3];
        *regUDP = argv[4];
    } else {
        *regIP = regIP_base; 
        *regUDP = regUDP_base; 
    }
}


//A lista vem sem o cabeçalho
char* validate_id(char* list, char* id){

    char *line;
    char *list_copy;
    srand(time(NULL));

    list_copy = strdup(list);
    if (list_copy == NULL) {
        printf("Erro memoria validate_id");
        exit(0);
    }

    line = strtok(list_copy, "\n");
    line = strtok(NULL, "\n");

    while (line != NULL){


        if ( line[0]==id[0] && line[1]==id[1]){

            sprintf(id, "%02d", rand() % 100);
            free(list_copy); 
            list_copy = strdup(list); 
            if (list_copy == NULL) {
                printf("Erro memoria validate_id");
                exit(0);
            }
            line = strtok(list_copy, "\n");
            line = strtok(NULL, "\n");
            continue;
        }
        line = strtok(NULL, "\n");
    }

    free(list_copy);

    return id;
}

struct node *create_node(){
    struct node *new_node = (struct node *) malloc(sizeof(struct node));
    if(new_node == NULL){
        printf("Erro de alocação de memória. [new_node]");
        exit(1);
    }

    new_node->serverTCP = -1;
    new_node->connection_next = -1;
    new_node->connection_back = -1;
    new_node->join = -1;


    new_node->id = NULL;
    new_node->id_back = NULL;
    new_node->id_next = NULL;
    new_node->id_nextnext = NULL;

    new_node->IP = NULL;
    new_node->IP_next = NULL;
    new_node->IP_nextnext = NULL;

    new_node->TCP = NULL;
    new_node->TCP_next = NULL;
    new_node->TCP_nextnext = NULL;


    new_node->id_rope = NULL;
    new_node->connection_rope = -1;

    for( int i = 0; i <  ROWS_ADJ; i++){
        new_node->adjacencies[i] = -1;
    }
    
    //new_node->update_tables = -1;


    return new_node;
}


int ver_id_repetido(int id_a_ver, int *lista_todos_ids, int tamanho_lista){
    int i;

    for(i = 0; i <= tamanho_lista; i++){
        if(id_a_ver == lista_todos_ids[i]){
            return 1;
        }
    }
    return 0;
}