/*
 * =====================================================================================
 *
 *       Filename:  keygen.c
 *
 *    Description:  generates random key of 27 characters
 *
 *        Version:  1.0
 *        Created:  05/28/2016 06:40:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  OSU
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  generates random 27 character key using size argument
 * =====================================================================================
 */
	int
main ( int argc, char *argv[] )
{
	if (argc != 2 || atoi(argv[1]) == 0){
		fprintf(stderr, "usage: arv[0] keylength\n");
		exit(1);
	}

	//seed randomness
	srand(time(NULL));

	//setup variables
	int i, num, count = atoi(argv[1]);
	char key[count];
	bzero(&key, sizeof(key));

	//define character range
	int min = 64;
	int max = 90;
	//generate characters in range
	for (i=0; i<count; i++){
		num = rand() % (max - min + 1) + min;
		//convert @ char to SPACE
		if (num == 64){
			num = 32;
		}
		sprintf(key, "%s%c", key, num);
	}

//	int urandom = open("/dev/urandom", O_RDONLY);
//	memset(key, 0, sizeof(key));
//	for (i=0; i<count; i++){
//		int valid_character = 0;
//		do {
//			read(urandom, ubuffer, 1);
//			if ((strcmp(ubuffer, "@") > 0 && strcmp(ubuffer, "[") < 0) 
//					|| strcmp(ubuffer, " ") == 0){
//				sprintf(key, "%s%s", key, ubuffer);
//				valid_character = 1;
//			}
//		}while(!valid_character);
//	}
	
	printf("%s\n", key);

	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
