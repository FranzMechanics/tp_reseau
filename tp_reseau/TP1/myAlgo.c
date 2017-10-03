#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

typedef struct Noeud{
    struct Noeud* f0;  //Branche 00
    struct Noeud* f1;  //Branche 01
    struct Noeud* f2;  //Branche 10
    struct Noeud* f3;  //Branche 11
    unsigned int nextHop;
}Noeud;

Noeud* racine;
int ligne_debug=0;
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
    racine->f0 = NULL;
    racine->f1 = NULL;
    racine->f2 = NULL;
    racine->f3 = NULL;
    racine->nextHop = 0;
}

void creeNoeud(Noeud *current){
    current=malloc(sizeof(Noeud));
    current->nextHop = 0;
}

void affiche_binary(unsigned int addr){
    for(int i=0;i<32;i++){
        if(take_bit(addr,i)){
            printf("1");
        }
        else printf("0");
        if(i==7 || i==15 || i ==23) printf(".");
    }printf("\n");
}

void debug(unsigned int addr,unsigned int netmask,unsigned int gw){
    printf("Address =\t");
    affiche_binary(addr);
    printf("Netmask =\t");
    affiche_binary(netmask);
    printf("Gateway =\t");
    affiche_binary(gw);
    printf("CIDR = %d\n",calcul_cidr(netmask));
}
/*Calcul la valeur en decimal des 3 prochain bit a partir de i inclus*/

void insertMyAlgo(unsigned int addr,unsigned int netmask,unsigned int gw){
    int cidr,i,step,choix;
    unsigned int addr_net;
    Noeud * current;
    cidr = calcul_cidr(netmask);
    addr_net = addr & netmask;
    current = racine;
    step = cidr / 2;
    int cas_complexe=0;
    if(cidr%2==1) cas_complexe=1;

    for(i=0;i<step;i++){
        //printf("step = %d\n",i);
        choix = (take_bit(addr_net,i*2)*2 + take_bit(addr_net,i*2+1)*1 );
        if (choix==0){ //Cas 00
            if(current->f0 == NULL){
                current->f0=malloc(sizeof(Noeud));
                current->f0->nextHop =0;
            }

            current = current->f0;
        }
        else if(choix == 1){ //Cas 01
            if(current->f1 == NULL){
                current->f1=malloc(sizeof(Noeud));
                current->f1->nextHop =0;
            }
            current= current->f1;
        }
        else if(choix == 2){ //Cas 10
            if(current->f2 == NULL){
                current->f2=malloc(sizeof(Noeud));
                current->f2->nextHop = 0;
            }
            current= current->f2;
        }
        else if(choix ==3){ //Cas 11
            if(current->f3 == NULL){
                current->f3=malloc(sizeof(Noeud));
                current->f3->nextHop =0;
            }
            current= current->f3;
        }
    }
    //debug(addr,netmask,gw);
    if(cas_complexe==0){ current->nextHop = gw; // Cas facile on est sur de la gateway 00 01 11 10
        //printf("Ajout facile direct\n");
        //printf("Over write\n\n");
    }
    else{//cas complexe , quel fils auquel on affecte la gw
        //printf("Hesitation...\n\n");
        if(take_bit(addr_net,cidr-1)==0){ // 0* -> 01 et 00 f0 et f1
            if(current->f0 == NULL){ // noeud non crée -> on le creer
                current->f0 = malloc(sizeof(Noeud));
                current->f0->nextHop=gw;
            }
            else{ //noeud creer
                if(current->f0->nextHop ==0 ){ //gw a 0
                    current->f0->nextHop = gw;
                }
                else{ //gw presente sur le noeud
                }
            }
            if(current->f1 == NULL){ // noeud non crée -> on le creer
                current->f1 = malloc(sizeof(Noeud));
                current->f1->nextHop=gw;
            }
            else{ //noeud creer
                if(current->f1->nextHop ==0 ){ //gw a 0
                    current->f1->nextHop = gw;
                }
                else{ //gw presente sur le noeud
                }
            }
        }
        else{ //1* -> 11 10
            if(current->f2 == NULL){ // noeud non crée -> on le creer
                current->f2 = malloc(sizeof(Noeud));
                current->f2->nextHop=gw;
            }
            else{ //noeud creer
                if(current->f2->nextHop ==0 ){ //gw a 0
                    current->f2->nextHop = gw;
                }
                else{ //gw presente sur le noeud
                }
            }

            if(current->f3 == NULL){ // noeud non crée -> on le creer
                current->f3 = malloc(sizeof(Noeud));
                current->f3->nextHop=gw;
            }
            else{ //noeud creer
                if(current->f3->nextHop ==0 ){ //gw a 0
                    current->f3->nextHop = gw;
                }
                else{ //gw presente sur le noeud
                }
            }
        }
    }
}

unsigned int lookupMyAlgo(unsigned int addr){
    unsigned int nextHop=0;
    int i=0;
    int choix;
    Noeud * current;
    current = racine;
    while(current != NULL){
        //printf("lookup i=%d\n",i);
        if(current->nextHop != 0 ) nextHop = current->nextHop;

        choix = take_bit(addr,i*2)*2 + take_bit(addr,i*2+1)*1;
        if (choix==0){ //Cas 0
            current = current->f0;
        }
        else if(choix == 1){ //Caqs 1
            current= current->f1;
        }
        else if(choix == 2){ //Caqs 1
            current= current->f2;
        }
        else if(choix == 3){ //Caqs 1
            current= current->f3;
        }
        i=i+1;
    }
    return nextHop;
}
