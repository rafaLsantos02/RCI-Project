#include "struct.h"
#include "header.h"


/*************************************************************************
 * lista_comandos()
 * 
 * Argumentos: 
 * 
 * Return: 
 * 
 * Descrição: Lista de comandos válidos
**************************************************************************/
void lista_comandos(){

    printf("join (j) ring id\n");
    printf("direct join (dj) id succid succIP succTCP\n");
    printf("chord (c) i\n");
    printf("remove chord (rc)\n" );
    printf("show topology (st)\n");
    printf("show routing (sr) dest\n");
    printf("show path (sp) dest\n");
    printf("show forwarding (sf)\n");
    printf("message (m) dest message\n");
    printf("leave (l)\n");
    printf("exit (x)\n\n\n");

    return;
}


/*************************************************************************
 * handle_command_1()
 * 
 * Argumentos: comando - é a string que contem o comando 
 * 
 * Return: message - retorna a mensagem da ligação
 * 
 * Descrição: Responsavel por verificar se o comando está completo
**************************************************************************/
struct node *handle_command(char *comando, char *IP_TCP, char *TCP, char *IP_UDP, char *UDP, int *node_created, struct node *fixed_node){

    //Se node_created == -1 -> não ha nenhum nó criado
    //Se node_created == 1 -> tá um nó criado

    char delimitador[1] = " ";
    char *comando_id, *comando_aux;


    comando_aux = (char*) malloc( sizeof(char)* strlen(comando) + 1 );
    if(comando_aux == NULL){
        printf("Erro alocação memória (handle_command)");
        exit(1);
    }
    
    strcpy(comando_aux, comando);

    comando_id = strtok(comando_aux, delimitador);


    switch( comando_id[0] ){
    case 'j' :

        if(*node_created == -1){
            fixed_node = comand_join(comando, IP_TCP, TCP, IP_UDP, UDP);
            if (fixed_node != NULL){
                *node_created = 1;
            }
            
        }else{
            printf("Comando inválido, existe um nó já inserido no anel.\n Tente outro comando!\n\n");
        }

        break;
    
    case 'd' :
        if(comando_id[1] == 'j'){

            if(*node_created == -1){
                fixed_node = comando_dj(comando, IP_TCP, TCP);
                *node_created = 1;

            }else{
                printf("Comando inválido, existe um nó já inserido no anel.\n Tente outro comando!\n\n");
            }    

        }
        else {
            printf("Comando desconhecido.\n");
        }

        break;

    case 'c' :

        if(*node_created == -1){
            
            printf("Não é possível realizar este comando num nó que não pertence a nenhum anel.\n");

        }else{
            add_chord(fixed_node);
        }
        break;

    case 'r' :
        if(comando_id[1] == 'c'){

            if(*node_created == -1){
                
                printf("Não é possível realizar este comando num nó que não pertence a nenhum anel.\n");

            }else{
                remove_chord(fixed_node);

            } 

        }
        else {
            printf("Comando desconhecido.\n");
        }

        break;

    case 's' :
        switch ( comando_id[1] ){
            case 't':
                
                if(*node_created == -1){
                
                    printf("Não é possível realizar este comando num nó que não pertence a nenhum anel.\n");

                }else{
                    show_topology(fixed_node);
                } 

                break;

            case 'r':

                if(*node_created == -1){
                    
                    printf("Não é possível realizar este comando num nó que não pertence a nenhum anel.\n");

                }else{
                    show_routing( fixed_node, comando);
                }
                break;

            case 'p':
                if(*node_created == -1){
                    
                    printf("Não é possível realizar este comando num nó que não pertence a nenhum anel.\n");

                }else{
                    show_path( fixed_node, comando);
                }
                break;

            case 'f':
                if(*node_created == -1){
                    
                    printf("Não é possível realizar este comando num nó que não pertence a nenhum anel.\n");

                }else{
                    show_fowarding( fixed_node);
                }            
                break;

            default:
                printf("Comando desconhecido.\n");
                break;
        }

        break;

    case 'm' :
            if(*node_created == -1){
                
                printf("Não é possível realizar este comando num nó que não pertence a nenhum anel.\n");

            }else{
                //command_chat_next( fixed_node, comando);
                command_chat( fixed_node, comando);
            }        
        break;

    case 'l' :
            if(*node_created == -1){
                
                printf("Não é possível realizar este comando num nó que não pertence a nenhum anel.\n");

            }else{
                leave(fixed_node, IP_UDP, UDP);
                //free_node(fixed_node);
                fixed_node = NULL;
                *node_created = -1;
            }
            break;

    case 'x' :
        printf("comando exit.\n");

        //printTable(fixed_node);

        free(comando_aux);
        //free_node(fixed_node);
        //Falta dar free ao node
        exit(0);
        break;
        
    default:
        printf("Comando desconhecido.\n");
        break;
    }

    free(comando_aux);

    return fixed_node;

}



