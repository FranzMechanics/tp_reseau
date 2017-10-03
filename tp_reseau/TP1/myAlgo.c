#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

/*
Multibit de taille fixe -> 3 bits par strides
000 001 010 011 100 101 110 111

*/
typedef struct Noeud{
    struct Noeud* f0;  //Branche 000
    struct Noeud* f1;  //Branche 001
    struct Noeud* f2;  //Branche 010
    struct Noeud* f3;  //Branche 011
    struct Noeud* f4;  //Branche 100
    struct Noeud* f5;  //Branche 101
    struct Noeud* f6;  //Branche 110
    struct Noeud* f7;  //Branche 111
    unsigned char cidr[8];
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

/*Recupere la valeur du bit à la position i ou le premier bit a gauche a pour indice 0*/
int take_bit(unsigned addr,int i){
    return (addr>>(31-i))&1;
}

/* On admet qu'il ne peut pas y avoir de cidr = 0 */
void initNoeud(Noeud * current){
    current->cidr[0]=0;
    current->cidr[1]=0;
    current->cidr[2]=0;
    current->cidr[3]=0;
    current->cidr[4]=0;
    current->cidr[5]=0;
    current->cidr[6]=0;
    current->cidr[7]=0;
    current->nextHop = 0;
}

/*On initialise la racine */
void initMyAlgo(){
    racine = malloc(sizeof(Noeud));
    racine->f0 = NULL;
    racine->f1 = NULL;
    racine->f2 = NULL;
    racine->f3 = NULL;
    racine->f4 = NULL;
    racine->f5 = NULL;
    racine->f6 = NULL;
    racine->f7 = NULL;
    initNoeud(racine);
}

void insertMyAlgo(unsigned int addr,unsigned int netmask,unsigned int gw){
    int cidr,i,step,choix;
    unsigned int addr_net;
    Noeud * current;
    Noeud * precedent; // permet de recuperer le noeud au dessus de current par lequel on est passer pour le cas_complexe 0
    cidr = calcul_cidr(netmask);
    addr_net = addr & netmask;
    current = racine;
    step = cidr / 3;
    int cas_complexe=0;
    cas_complexe=cidr%3;

    //On parcourt l'abre afin d'arriver sur le noeud exact si cas_complexe = 0 ou sur le noeud parent si cas_complexe != 0
    for(i=0;i<step;i++){
        precedent = current;
        choix = (take_bit(addr_net,i*3)*4 + take_bit(addr_net,i*3+1)*2 + take_bit(addr_net,i*3+2)*1 );
        switch (choix) {
            case 0:
                if(current->f0 == NULL){
                    current->f0=malloc(sizeof(Noeud));
                    initNoeud(current->f0);
                }
                current = current->f0;
                break;

            case 1:
                if(current->f1 == NULL){
                    current->f1=malloc(sizeof(Noeud));
                    initNoeud(current->f1);
                }
                current = current->f1;
                break;
            case 2:
                if(current->f2 == NULL){
                    current->f2=malloc(sizeof(Noeud));
                    initNoeud(current->f2);
                }
                current = current->f2;
                break;

            case 3:
                if(current->f3 == NULL){
                    current->f3=malloc(sizeof(Noeud));
                    initNoeud(current->f3);
                }
                current = current->f3;
                break;
            case 4:
                if(current->f4 == NULL){
                    current->f4=malloc(sizeof(Noeud));
                    initNoeud(current->f4);
                }
                current = current->f4;
                break;

            case 5:
                if(current->f5 == NULL){
                    current->f5=malloc(sizeof(Noeud));
                    initNoeud(current->f5);
                }
                current = current->f5;
                break;
            case 6:
                if(current->f6 == NULL){
                    current->f6=malloc(sizeof(Noeud));
                    initNoeud(current->f6);
                }
                current = current->f6;
                break;

            case 7:
                if(current->f7 == NULL){
                    current->f7=malloc(sizeof(Noeud));
                    initNoeud(current->f7);
                }
                current = current->f7;
                break;
        }
    }

    if(cas_complexe==0){  // Soit X1 X2 X3 les derniers bits utiles de addr_net --> X1,X2 et X3 sont connus
        current->nextHop = gw;
        //On utilise le precedent afin de pouvoir modifier le tableau de cidr du noeud parent
        switch (choix) {
            case 0:
                precedent->cidr[0]=cidr;
                break;
            case 1:
                precedent->cidr[1]=cidr;
                break;
            case 2:
                precedent->cidr[2]=cidr;
                break;
            case 3:
                precedent->cidr[3]=cidr;
                break;
            case 4:
                precedent->cidr[4]=cidr;
                break;
            case 5:
                precedent->cidr[5]=cidr;
                break;
            case 6:
                precedent->cidr[6]=cidr;
                break;
            case 7:
                precedent->cidr[7]=cidr;
                break;
        }

    }

    else if(cas_complexe==2){ // Soit X1 X2 X3 les derniers bits utiles de addr_net --> X1 et X2 sont connus
        if( take_bit(addr_net,i*3)==1 ){ // X1 = 1
            if( take_bit(addr_net,i*3+1)==1 ){ // X2 =1
                // affectations des fils 110 et 111 sois 6 et 7

                //test si un reseau plus precis a ete deja affecte
                if(current->cidr[7]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                    current->cidr[7]=cidr;
                    if(current->f7 == NULL){
                        current->f7=malloc(sizeof(Noeud));
                        initNoeud(current->f7);
                    }
                    current->f7->nextHop=gw;
                }
                if(current->cidr[6]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                    current->cidr[6]=cidr;
                    if(current->f6 == NULL){
                        current->f6=malloc(sizeof(Noeud));
                        initNoeud(current->f6);
                    }
                    current->f6->nextHop=gw;
                }

            }
            else{ // X2 = 0
                // affectation des fils 101 100 sois 5 et 4

                //test si un reseau plus precis a ete deja affecte
                if(current->cidr[5]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                    current->cidr[5]=cidr;
                    if(current->f5 == NULL){
                        current->f5=malloc(sizeof(Noeud));
                        initNoeud(current->f5);
                    }
                    current->f5->nextHop=gw;
                }
                if(current->cidr[4]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                    current->cidr[4]=cidr;
                    if(current->f4 == NULL){
                        current->f4=malloc(sizeof(Noeud));
                        initNoeud(current->f4);
                    }
                    current->f4->nextHop=gw;
                }

            }
        }
        else{ // X1 = 0
            if( take_bit(addr_net,i*3+1)==1 ){ // X2 =1
                // affectations des fils 010 et 011 sois 2 et 3

                //test si un reseau plus precis a ete deja affecte
                if(current->cidr[3]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                    current->cidr[3]=cidr;
                    if(current->f3 == NULL){
                        current->f3=malloc(sizeof(Noeud));
                        initNoeud(current->f3);
                    }
                    current->f3->nextHop=gw;
                }
                if(current->cidr[2]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                    current->cidr[2]=cidr;
                    if(current->f2 == NULL){
                        current->f2=malloc(sizeof(Noeud));
                        initNoeud(current->f2);
                    }
                    current->f2->nextHop=gw;
                }
            }
            else{ // X2 = 0
                // affectation des fils 001 000 sois 1 et 0

                //test si un reseau plus precis a ete deja affecte
                if(current->cidr[1]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                    current->cidr[1]=cidr;
                    if(current->f1 == NULL){
                        current->f1=malloc(sizeof(Noeud));
                        initNoeud(current->f1);
                    }
                    current->f1->nextHop=gw;
                }
                if(current->cidr[0]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                    current->cidr[0]=cidr;
                    if(current->f0 == NULL){
                        current->f0=malloc(sizeof(Noeud));
                        initNoeud(current->f0);
                    }
                    current->f0->nextHop=gw;
                }
            }
        }
    }

    else if(cas_complexe==1){ // Soit X1 X2 X3 les derniers bits utiles de addr_net --> X1 est connus

        if( take_bit(addr_net,i*3)==1 ){ // X1 = 1
            // affectation des fils 100 101 110 111 sois 4 5 6 7

            //test si un reseau plus precis a ete deja affecte
            //test si un reseau plus precis a ete deja affecte
            if(current->cidr[7]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis

                current->cidr[7]=cidr;
                if(current->f7 == NULL){
                    current->f7=malloc(sizeof(Noeud));
                    initNoeud(current->f7);
                }
                current->f7->nextHop=gw;
            }
            if(current->cidr[6]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis

                current->cidr[6]=cidr;
                if(current->f6 == NULL){
                    current->f6=malloc(sizeof(Noeud));
                    initNoeud(current->f6);
                }
                current->f6->nextHop=gw;
            }
            if(current->cidr[5]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis

                current->cidr[5]=cidr;
                if(current->f5 == NULL){
                    current->f5=malloc(sizeof(Noeud));
                    initNoeud(current->f5);
                }
                current->f5->nextHop=gw;
            }
            if(current->cidr[4]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis

                current->cidr[4]=cidr;
                if(current->f4 == NULL){
                    current->f4=malloc(sizeof(Noeud));
                    initNoeud(current->f4);
                }
                current->f4->nextHop=gw;
            }

        }
        else{ // X1= 0
            // affecation des fils  000 001 010 011 sois 0 1 2 3

            //test si un reseau plus precis a ete deja affecte
            if(current->cidr[3]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                current->cidr[3]=cidr;
                if(current->f3 == NULL){
                    current->f3=malloc(sizeof(Noeud));
                    initNoeud(current->f3);
                }
                current->f3->nextHop=gw;
            }
            if(current->cidr[2]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                current->cidr[2]=cidr;
                if(current->f2 == NULL){
                    current->f2=malloc(sizeof(Noeud));
                    initNoeud(current->f2);
                }
                current->f2->nextHop=gw;
            }
            if(current->cidr[1]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                current->cidr[1]=cidr;
                if(current->f1 == NULL){
                    current->f1=malloc(sizeof(Noeud));
                    initNoeud(current->f1);
                }
                current->f1->nextHop=gw;
            }
            if(current->cidr[0]<cidr){ // on modifie la gateway ainsi que le cidr de la gateway car le reseau est plus precis
                current->cidr[0]=cidr;
                if(current->f0 == NULL){
                    current->f0=malloc(sizeof(Noeud));
                    initNoeud(current->f0);
                }
                current->f0->nextHop=gw;
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
        if(current->nextHop != 0 ) nextHop = current->nextHop;

        //on calcule par les 3 prochains bit que l'on a pas encore lu, quel fils choisir
        choix = (take_bit(addr,i*3)*4 + take_bit(addr,i*3+1)*2 + take_bit(addr,i*3+2)*1 );
        switch (choix) {
            case 0:
                current = current->f0;
                break;
            case 1:
                current = current->f1;
                break;
            case 2:
                current = current->f2;
                break;
            case 3:
                current = current->f3;
                break;
            case 4:
                current = current->f4;
                break;
            case 5:
                current = current->f5;
                break;
            case 6:
                current = current->f6;
                break;
            case 7:
                current = current->f7;
                break;
        }
        i++;
    }
    return nextHop;
}
