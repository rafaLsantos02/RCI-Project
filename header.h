#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>


#include <sys/select.h>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define regIP_base "193.136.138.142"
#define regUDP_base "59000"

#define buffer_short 128
#define buffer_big 2048

#define COLS 100
#define ROWS 100
#define ROWS_ADJ 101


#define max(A,B) ((A)>=(B)?(A):(B))

struct node;

//funcao.c
void show_topology(struct node *no);

struct node *comand_join(char *comando, char *IP_TCP, char *TCP, char *IP_UDP, char *UDP);
struct node *join(char *ring, char *id, char *IP_TCP, char *TCP, char *IP_UDP, char *UDP);

struct node *comando_dj(char *comando, char *IP_TCP, char *TCP);
struct node *direct_join(char *id_novo, char *IP_TCP, char *TCP, char *id_ligar, char *IP_ligar, char *TCP_ligar);

void leave(struct node *fixed_node, char *IP_UDP, char *UDP);
void close_connection(struct node *fixed_node, int connection); 

void add_chord(struct node *fixed_node);
void remove_chord(struct node *fixed_node);
void remove_chord_plus(struct node *fixed_node, int connection_to_remove);

void show_fowarding( struct node *fixed_node);
void show_path( struct node *fixed_node, char *comand );
void show_routing( struct node *fixed_node, char *comand);

void command_chat( struct node *fixed_node, char *comand);
void chat_message( char *comand, struct node *fixed_node);

void command_chat_next( struct node *fixed_node, char *comand);
void chat_message_next( char *comand, struct node *fixed_node);

void free_node(struct node *fixed_node);


//udp_message.c
int send_UDP( char *message_input, char **message_return, char *IP_UDP, char *UDP);
void confirm_server_regist(struct node *fixed_node);


//socket.c
int setup_serverTCP(char *IP, char *TCP);
int accept_new_connectionTCP(int server_conection_tcp);
//A função de baixo existia mas foi à vida
//void handle_connectionTCP(struct node *no, int client_socket); 
int connect_clientTCP(char *IP, char *TCP);
void send_message_tcp(char* message, int connection);
void recieve_message_tcp(int connection);


//global.c
void lista_comandos();
char* validate_id( char* list, char *id);
struct node *handle_command(char *comando, char *IP_TCP, char *TCP, char *IP_UDP, char *UDP, int *node_created, struct node *no);

//aux_func.c
void validate_arguments( int argc, char *argv[], char **IP, char **TCP, char **regIP, char **regUDP );
struct node *create_node();
int ver_id_repetido(int id_a_ver, int *lista_todos_ids, int tamanho_lista);

//manage_messages.c
void fase2(int connection, struct node *fixed_node);
void message_from_next(int connection, struct node *fixed_node, char* IP_UDP, char *UDP);
void message_from_back(int connection, struct node *fixed_node);
void inicialize_global_vectors();






//NOVIDADES

void initializeTable(struct node *fixed_node);
void printTable(struct node *fixed_node);
void process_route( struct node *fixed_node, char *message_route );
int update_short_path( struct node *fixed_node, int line );
void send_updated_path( struct node *fixed_node, int line_updated );
void send_all_table( struct node *fixed_node, int connection);
void route_end_connection( struct node *fixed_node, int connection_closed );
void update_all_shortpath( struct node *fixed_node );
void update_shipping_table( struct node *fixed_node, int line_updated );