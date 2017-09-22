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

void hexadecimal_to_binary(char* hexa, char* binary_line){
	int i,j, n;
	n = strlen(hexa);
	j=0;
	if (hexa[1] == 'x'){
		j = 2;
	}

	for (i=j; i<n; i++)
	/* On commence a 2 car on ne compte pas le prefixe 0x devant le chiffre en hexadecimal */
	{
		if (hexa[i] == '0')
		/* Si notre caractere vaut 0, on le traduit en binaire par "0000" */
		{
			strcat(binary_line, "0000");
		}
		else if (hexa[i] == '1')
		/* Si notre caractere vaut 1, on le traduit en binaire par "0001" */
		{
			strcat(binary_line, "0001");
		}
		else if (hexa[i] == '2')
		/* Si notre caractere vaut 2, on le traduit en binaire par "0010" */
		{
			strcat(binary_line, "0010");
		}
		else if (hexa[i] == '3')
		/* Si notre caractere vaut 3, on le traduit en binaire par "0011" */
		{
			strcat(binary_line, "0011");
		}
		else if (hexa[i] == '4')
		/* Si notre caractere vaut 4, on le traduit en binaire par "0100" */
		{
			strcat(binary_line, "0100");
		}
		else if (hexa[i] == '5')
		/* Si notre caractere vaut 5, on le traduit en binaire par "0101" */
		{
			strcat(binary_line, "0101");
		}
		else if (hexa[i] == '6')
		/* Si notre caractere vaut 6, on le traduit en binaire par "0110" */
		{
			strcat(binary_line, "0110");
		}
		else if (hexa[i] == '7')
		/* Si notre caractere vaut 7, on le traduit en binaire par "0111
		 * " */
		{
			strcat(binary_line, "0111");
		}
		else if (hexa[i] == '8')
		/* Si notre caractere vaut 8, on le traduit en binaire par "1000" */
		{
			strcat(binary_line, "1000");
		}
		else if (hexa[i] == '9')
		/* Si notre caractere vaut 9, on le traduit en binaire par "1001" */
		{
			strcat(binary_line, "1001");
		}
		else if (hexa[i] == 'a')
		/* Si notre caractere vaut A, on le traduit en binaire par "1010" */
		{
			strcat(binary_line, "1010");
		}
		else if (hexa[i] == 'b')
		/* Si notre caractere vaut B, on le traduit en binaire par "1011" */
		{
			strcat(binary_line, "1011");
		}
		else if (hexa[i] == 'c')
		/* Si notre caractere vaut C, on le traduit en binaire par "1100" */
		{
			strcat(binary_line, "1100");
		}
		else if (hexa[i] == 'd')
		/* Si notre caractere vaut D, on le traduit en binaire par "1101" */
		{
			strcat(binary_line, "1101");
		}
		else if (hexa[i] == 'e')
		/* Si notre caractere vaut E, on le traduit en binaire par "1110" */
		{
			strcat(binary_line, "1110");
		}
		else if (hexa[i] == 'f')
		/* Si notre caractere vaut F, on le traduit en binaire par "1111" */
		{
			strcat(binary_line, "1111");
		}
		else
		{
			printf("%c : Caractere invalide\n", hexa[i]);
		}
	}
}

int hamming(char* netmask_bin){
    int count = 0;
    for(int i=0; i<32; i++){
        if(netmask_bin[i] == '1'){
            count++;
        }
    }
    return count;
}

char* unsigned_to_bin(unsigned int uns){
    char* hex;
    char* bin;
    // On prepare les variables de conversion
    hex = malloc(9);
    bin = malloc(33);
    // On traduit unsigned int en hexadecimal
    sprintf(hex, "%08x", uns);
    // On traduit l'hexadecimal en chaine de caracteres binaire
    hexadecimal_to_binary(hex, bin);
    printf("%s\n", bin);
    free(hex);
    return bin;
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
    char *addr_bin, *netmask_bin;
    int netmask_dec;

    addr_bin = NULL;
    netmask_bin = NULL;

    // On recupere la racine de l'arbre
    Noeud *curr = racine;

    addr_bin = unsigned_to_bin(addr);
    netmask_bin = unsigned_to_bin(netmask);

    printf("%s %s\n", addr_bin, netmask_bin);

    // On calcule le poids du netmask binaire pour obtenir le CIDR
    netmask_dec = hamming(netmask_bin);

    printf("Netmask : /%d\n", netmask_dec);

    /* On regarde les (---.---.---.---/n) n premiers caracteres de l'adresse IP
    Chaque bit represente un noeud. Si le prochain bit de la chaine est un 0,
    on partira sur la branche gauche du noeud, et si c'est un 1 on part sur la branche
    droite. Enfin quand on finit le parcours on ajoute la gateway correspondant
    a ce routage au noeud sur lequel on se trouve */
    for(int i=0; i<netmask_dec; i++){
        //printf("NextHop %x\n", curr->nextHop);
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
            }
            curr->nextHop = 0;
            curr->fg->nextHop = 0;
        }
    }
    curr->nextHop = gw;
    printf("NextHop %x\n", curr->nextHop);

    free(addr_bin);
    free(netmask_bin);
}

unsigned int lookupMyAlgo(unsigned int addr){
    char *addr_bin = NULL;
    unsigned int nextHop = 0;

    addr_bin = unsigned_to_bin(addr);
    printf("%s\n", addr_bin);
    // On recupere la racine de l'arbre
    Noeud *curr = racine;
    for(int i=0; i<32; i++){
        printf("%u\n", curr->nextHop);
        if(addr_bin[i] == '1'){
            if(curr->fd == NULL){
                printf("w");
                break;
            }
            if(curr->nextHop != 0){
                printf("Found\n");
                nextHop = curr->nextHop;
            }
            curr = curr->fd;
        }
        else{
            if(curr->fg == NULL){
                printf("w");
                break;
            }
            if(curr->nextHop != 0){
                printf("Found\n");
                nextHop = curr->nextHop;
            }
            curr = curr->fg;
        }
    }
    free(addr_bin);
    return nextHop;
}
