#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

typedef struct Noeud{
    struct Noeud* fg;  //Branche 0
    struct Noeud* fd;  //Branche 1
    unsigned int nextHop;
}Noeud;

Noeud* racine;

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
    racine->fg = NULL;
    racine->fd = NULL;
}

void insertMyAlgo(unsigned int addr,unsigned int netmask,unsigned int gw){
    int netmask_dec;

    // On recupere la racine de l'arbre
    Noeud *curr = racine;

    char addr_bin[8*sizeof(unsigned int) + 1] = {0};
    for (int j = 0; j < (8*sizeof(unsigned int) + 1); j++) {
        addr_bin[j] = (addr << j) & (1 << (8*sizeof(unsigned int)-1)) ? '1' : '0';
    }
    addr_bin[32] = '\0';
    //printf("addr is b%s\n", addr_bin);

    char netmask_bin[8*sizeof(unsigned int) + 1] = {0};
    for (int j = 0; j < (8*sizeof(unsigned int) + 1); j++) {
        netmask_bin[j] = (netmask << j) & (1 << (8*sizeof(unsigned int)-1)) ? '1' : '0';
    }
    netmask_bin[32] = '\0';
    //printf("netmask is b%s\n", netmask_bin);

    //printf("%s %s\n", addr_bin, netmask_bin);

    // On calcule le poids du netmask binaire pour obtenir le CIDR
    netmask_dec = hamming(netmask_bin);

    //printf("Netmask : /%d\n", netmask_dec);

    /* On regarde les (---.---.---.---/n) n premiers caracteres de l'adresse IP
    Chaque bit represente un noeud. Si le prochain bit de la chaine est un 0,
    on partira sur la branche gauche du noeud, et si c'est un 1 on part sur la branche
    droite. Enfin quand on finit le parcours on ajoute la gateway correspondant
    a ce routage au noeud sur lequel on se trouve */
    for(int i=0; i<netmask_dec; i++){
        //printf("%c", addr_bin[i]);
        if(addr_bin[i] == '1'){
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
    }
    curr->nextHop = gw;
    //printf("\n%x\n", curr->nextHop);
}

unsigned int lookupMyAlgo(unsigned int addr){
    unsigned int nextHop = 0;

    char addr_bin[8*sizeof(unsigned int) + 1] = {0};
    for (int j = 0; j < (8*sizeof(unsigned int) + 1); j++) {
        addr_bin[j] = (addr << j) & (1 << (8*sizeof(unsigned int)-1)) ? '1' : '0';
    }
    addr_bin[32] = '\0';
    //printf("addr is b%s\n", addr_bin);

    //printf("%s\n", addr_bin);
    // On recupere la racine de l'arbre
    Noeud *curr = racine;
    for(int i=0; i<32; i++){
        //printf("%u\n", curr->nextHop);
        if(addr_bin[i] == '1'){
            if(curr->fd == NULL){
                //printf("w");
                break;
            }
            if(curr->nextHop != 0){
                //printf("Found\n");
                nextHop = curr->nextHop;
            }
            curr = curr->fd;
        }
        else if (addr_bin[i] == '0'){
            if(curr->fg == NULL){
                //printf("w");
                break;
            }
            if(curr->nextHop != 0){
                //printf("Found\n");
                nextHop = curr->nextHop;
            }
            curr = curr->fg;
        }
        else {
            printf("%c : Erreur caractere invalide\n", addr_bin[i]);
        }
    }

    return nextHop;
}
