#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>

#define MAXLEN 100

void initMyAlgo();

void insertMyAlgo(unsigned int addr,unsigned int netmask,unsigned int gw);

unsigned int lookupMyAlgo(unsigned int addr);


// This function must not be modified !!!
int loadFile(char *path)
{
	FILE *fi;
	char s[MAXLEN],*p,*start;
 	struct in_addr c;
	unsigned int addr,netmask,gw;

	// first sanity check
	if (!path) return 0;
	if ((fi=fopen(path,"r")) == NULL ) return 0;

	// read file line by line (max = MAXLEN )
	while (fgets(s,MAXLEN,fi) != NULL) {
		s[MAXLEN]=0;
		p=start=s;
		// seek for blank char
		while (*p && (*p != ' ')) p++;
		if (p > start) {
			*p=0; p++;
			if (inet_aton(start,&c) == 0 ) continue;
			addr=htonl(c.s_addr);
		}
		// skip remaining blank char
		while (*p && (*p == ' ')) p++;

		// stat a new search for blank char
		start=p;
		while (*p && (*p != ' ')) p++;
		if (p > start) {
			*p=0; p++;
			if (inet_aton(start,&c) == 0 ) continue;
			netmask=htonl(c.s_addr);
		}
		// skip remaining blank char
		while (*p && (*p == ' ')) p++;

		// stat a new search for blank '\n' char
		start=p;
		while (*p && (*p != '\n')) p++;
		if (p > start) {
			*p=0; p++;
			if (inet_aton(start,&c) == 0 ) continue;
			gw=htonl(c.s_addr);
		}

		// call your function to insert entry to routing table
		insertMyAlgo(addr,netmask,gw);
	}
	fclose(fi);
	return 1;
}

// only for debug purpose might be modified !
int main (int argc,char *argv[])
{
	char s[MAXLEN];
	int addr;
 	struct in_addr a,b;

	initMyAlgo();

	int count, count_ok;
	count_ok = count = 0;

	struct timeval tval_before, tval_after, tval_result;
	gettimeofday(&tval_before, NULL);

	if ((argc > 1 ) && (loadFile(argv[1]))) {
		printf("IP lookup algo\n");

		FILE* f = fopen("TP1_resultats.", "r");
		while (fgets(s,MAXLEN,f) != NULL) {

			char* ip = strtok(s, " ");
			char* result = strtok(NULL, "\n");

			s[MAXLEN]=0;
			if (inet_aton(ip,&a) == 0 ) continue;
			addr=htonl(a.s_addr);
            printf("Adresse a chercher : %s  ",inet_ntoa(a));
			a.s_addr=htonl(lookupMyAlgo(addr));

			printf("GW found = %s\n",inet_ntoa(a));

			if(strcmp(inet_ntoa(a), result) == 0){
				count_ok++;
			} count++;
		}
		fclose(f);
	}
    if(count==0){
        printf("Aucun fichier lu\n");
        return 0;
    }
	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);
	printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);

	printf("Resultat = %d/%d, %d%%\n", count_ok, count, count_ok*100/count);
	return 0;
}
