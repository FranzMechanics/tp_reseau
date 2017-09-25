#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

typedef struct Noeud{
    struct Noeud* f0;  //Branche 0
    struct Noeud* f1;  //Branche 1
    struct Noeud* f2;  //Branche 2
    struct Noeud* f3;  //Branche 3
    unsigned int nextHop;
}Noeud;

Noeud* racine;

/*Fonction qui convertit addr en un tableau de char representant les bits, ne pas oublier de free */
char * IPtoTabChar(unsigned int addr){
    char * addr_bin = (char *) malloc(sizeof(char)*33);
    int i;
    for(i=0;i<32;i++){
        addr_bin[i] = ( (addr << i) & (1 <<31)) ? '1' : '0' ;
    }
    addr_bin[32]='\0';
    return addr_bin;
}

/*Fonction qui calcule le CIDR*/
int hamming(char* netmask_bin){
    int count = 0;
    for(int i=0; i<32; i++){
        if(netmask_bin[i] == '1'){
            count++;
        }
    }
    return count;
}

/*
Declaration de l'arbre en global
Pour ajouter un noeud --> on part de la racine et on reparcourt tout l'arbre en insérant le next hop au bon endroit
*/

void initMyAlgo(){
    racine = malloc(sizeof(Noeud));
    racine->f0 = NULL;
    racine->f1 = NULL;
    racine->f2 = NULL;
    racine->f3 = NULL;
}

/*Calcul la valeur en decimal des 3 prochain bit a partir de i inclus*/
int fct_choix_fils(char addr_bin[],int i){
    int x;
    x = (addr_bin[i]=='1' ? 1 : 0) + (addr_bin[i+1]=='1' ? 2 : 0);
    return x;
}

void insertMyAlgo(unsigned int addr,unsigned int netmask,unsigned int gw){
    int netmask_dec;
    int step_to_descent;
    int choix_fils;
    // On recupere la racine de l'arbre
    Noeud *curr = racine;

    /*Pour que addr soit l'address du reseau*/
    addr &= netmask;

    char *addr_bin = (char *) IPtoTabChar(addr);
    char *netmask_bin = (char *) IPtoTabChar(netmask);

    // On calcule le poids du netmask binaire pour obtenir le CIDR
    netmask_dec = hamming(netmask_bin);
    step_to_descent = netmask_dec / 2;

    /* On regarde les (---.---.---.---/n) n premiers caracteres de l'adresse IP
    Chaque bit represente un noeud. Si le prochain bit de la chaine est un 0,
    on partira sur la branche gauche du noeud, et si c'est un 1 on part sur la branche
    droite. Enfin quand on finit le parcours on ajoute la gateway correspondant
    a ce routage au noeud sur lequel on se trouve */
    for(int i=0; i<step_to_descent; i++){
        choix_fils = fct_choix_fils(addr_bin,i*2);
        switch (choix_fils) {
            case 0:
                if(curr->f0 == NULL){
                    curr->f0 = malloc(sizeof(Noeud));
                    curr->f0->nextHop = 0;
                }
                curr = curr->f0;
                break;
            case 1:
                if(curr->f1 == NULL){
                    curr->f1 = malloc(sizeof(Noeud));
                    curr->f1->nextHop = 0;
                }
                curr = curr->f1;
                break;
            case 2:
            if(curr->f2 == NULL){
                curr->f2 = malloc(sizeof(Noeud));
                curr->f2->nextHop = 0;
            }
            curr = curr->f2;
                break;
            case 3:
            if(curr->f3 == NULL){
                curr->f3 = malloc(sizeof(Noeud));
                curr->f3->nextHop = 0;
            }
            curr = curr->f3;
                break;
        }
    }
        /*
        if(choix_fils == 0){
            if(curr->fd == NULL){
                curr->fd = malloc(sizeof(Noeud));
                curr->fd->nextHop = 0;
            }
            curr = curr->fd;
        }
        else{
            if(curr->fg == NULL){
                curr->fg = malloc(sizeof(Noeud));
                curr->fg->nextHop = 0;
            }
            curr = curr->fg;
        }
    }*/

    curr->nextHop = gw;
    free(addr_bin);
    free(netmask_bin);
}

unsigned int lookupMyAlgo(unsigned int addr){
    unsigned int nextHop = 0;

    char *addr_bin = (char *) IPtoTabChar(addr);

    // On recupere la racine de l'arbre
    Noeud *curr = racine;
    int choix_fils;
    int i =0;
    while( curr != NULL){
        choix_fils= fct_choix_fils(addr_bin,i);
        switch (choix_fils) {
            case 0:
                if(curr->nextHop != 0){
                    nextHop = curr->nextHop;
                }
                if(curr->f0 == NULL){
                    curr=NULL;
                    break;
                }

                curr = curr->f0;
                break;
            case 1:
                if(curr->nextHop != 0){
                    nextHop = curr->nextHop;
                }
                if(curr->f1 == NULL){
                    curr=NULL;
                    break;
                }

                curr = curr->f1;
                break;
            case 2:
            if(curr->nextHop != 0){
                nextHop = curr->nextHop;
            }
                if(curr->f2 == NULL){
                    curr=NULL;
                    break;
                }

                curr = curr->f2;
                break;
            case 3:
            if(curr->nextHop != 0){
                nextHop = curr->nextHop;
            }
                if(curr->f3 == NULL){
                    curr=NULL;
                    break;
                }

                curr = curr->f3;
                break;
            default:
                printf("%c : Erreur caractere invalide\n", addr_bin[i]);
                break;
        }
        i=i+2;
        /*if(addr_bin[i] == '1'){
            if(curr->fd == NULL){
                break;
            }
            if(curr->nextHop != 0){
                nextHop = curr->nextHop;
            }
            curr = curr->fd;
        }
        else if (addr_bin[i] == '0'){
            if(curr->fg == NULL){
                break;
            }
            if(curr->nextHop != 0){
                nextHop = curr->nextHop;
            }
            curr = curr->fg;
        }
        else {
            printf("%c : Erreur caractere invalide\n", addr_bin[i]);
        }*/

    }
    free(addr_bin);
    return nextHop;
}
