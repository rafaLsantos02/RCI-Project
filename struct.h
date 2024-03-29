
#include "header.h"

typedef struct node{
    //Informação dele mesmo 
    int serverTCP;
    char *ring;
    char *id;
    char *IP;
    char *TCP;

    int join;

    //Informação sobre o next
    int connection_next;
    char *id_next;
    char *IP_next;
    char *TCP_next;

    //Informação sobre o back
    int connection_back;
    char *id_back;


    //Informação sobre o next next
    char *id_nextnext;
    char *IP_nextnext;
    char *TCP_nextnext;

    //Informação sobre corda
    int connection_rope; 
    char *id_rope;

    //Lista de adjacencias
    int adjacencies[ROWS_ADJ];

    //Informação sobre as tabelas
    //int update_tables;
    char short_path[ROWS][buffer_short];
    char shipping_table[ROWS][3];
    char table[ROWS][COLS][buffer_short];

}node;