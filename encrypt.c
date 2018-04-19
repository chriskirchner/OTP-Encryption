/*
 * =====================================================================================
 *
 *       Filename:  encrypt.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/28/2016 06:00:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  OSU
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int encrypt(char, char);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
	int
main ( int argc, char *argv[] )
{	
	FILE *key = fopen(argv[1], "r");
	FILE *msg = fopen(argv[2], "r");
	fseek(msg, 0, SEEK_END);
	int count = ftell(msg);
	fseek(msg, 0, SEEK_SET);

	char buffer[count];
	memset(buffer, 0, sizeof(buffer));
	for (int i=0; i<count; i++){
		char p = fgetc(msg);
		char k = fgetc(key);
		char e = encrypt(p, k);
		int length = strlen(buffer);	
		buffer[length] = e;
		buffer[length+1] = '\0';
	}	

	printf("%s\n", buffer);

	fclose(key);
	fclose(msg);
	
	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

int encrypt(char a, char b ){
	if (a == 32) a = 64;
	char e = a + b - 64;
	if (e > 90){
		e = e - 27;
	}		
	return e;
}




