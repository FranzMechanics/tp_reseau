#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

typedef struct Noeud{
    struct Noeud* f0;  //Branche 0
    struct Noeud* f1;  //Branche 1
    unsigned int nextHop;
}Noeud;

Noeud* racine;

// Representation d une adresse 128.0.0.2 -> 10000000.00000000.00000000.00000010 indice de 0->31 de gauche a droite

/*Fonction qui calcule le CIDR*/
int calcul_cidr(unsigned int netmask){
    int cidr=0;
    for(int i=0;i<32;i++){
        if ((netmask >> i) & 1) cidr++;
    }
    return cidr;
}

/*i = indice du tableu binaire virtuel*/
int take_bit(unsigned addr,int i){
    return (addr>>(31-i))&1;
}
/*
Declaration de l'arbre en global
Pour ajouter un noeud --> on part de la racine et on reparcourt tout l'arbre en insérant le next hop au bon endroit
*/

void initMyAlgo(){
    racine = malloc(sizeof(Noeud));
    racine->nextHop = 0;
}

/*Calcul la valeur en decimal des 3 prochain bit a partir de i inclus*/

void insertMyAlgo(unsigned int addr,unsigned int netmask,unsigned int gw){
    int cidr,i;
    unsigned int addr_net;
    Noeud * current;
    cidr = calcul_cidr(netmask);
    addr_net = addr & netmask;
    current = racine;
    for(i=0;i<cidr;i++){
        if (!(take_bit(addr_net,i))){ //Cas 0
            if(current->f0 == NULL){
                current->f0 = malloc(sizeof(Noeud));
                current->f0->nextHop=0;
            }
            current= current->f0;
        }
        else{ //Caqs 1
            if(current->f1 == NULL){
                current->f1 = malloc(sizeof(Noeud));
                current->f1->nextHop=0;
            }
            current= current->f1;
        }
    }
    current->nextHop = gw;
}

unsigned int lookupMyAlgo(unsigned int addr){
    unsigned int nextHop=0;
    int i=0;
    Noeud * current;
    current = racine;
    while(current != NULL){
        if(current->nextHop != 0 ) nextHop = current->nextHop;
        if (take_bit(addr,i)==0 ){ //Cas 0
            current= current->f0;
        }
        else{ //Caqs 1
            current= current->f1;
        }
        i++;
    }
    return nextHop;
}
