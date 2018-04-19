/*
 * =====================================================================================
 *
 *       Filename:  encryption.c
 *
 *    Description:  implementation file for encryption otp_dec/enc library
 *
 *        Version:  1.0
 *        Created:  05/31/2016 07:37:27 PM
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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "encryption.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  encrypt
 *  Description:  returns ciphertext using key and plaintext 
 * =====================================================================================
 */

char *encrypt(char *key, char *text){
	int length = strlen(text);
	char *cipher = (char*) malloc(length);
    memset(cipher, 0, length);
	int i;
    for (i=0; i<length; i++){
        char k = key[i];
        char p = text[i];
        char e = epad(p, k);
        *(cipher+i) = e;
	}
	return cipher;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  epad
 *  Description:  encrypt plaintext character (a) with key character (b)
 * =====================================================================================
 */

int epad(char a, char b){
    if (a == 32) a = 64;
	if (b == 32) b = 64;
    char e = a + b - 64;
    if (e > 90){
        e = e - 27;
    }
	if (e == 64) e = 32;
    return e;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  decrypt
 *  Description:  decrypt ciphertext with key
 * =====================================================================================
 */

char *decrypt(char *key, char *text){
	int length = strlen(text);
	char *plain = (char*) malloc(length);
    memset(plain, 0, length);
	int i;
    for (i=0; i<length; i++){
        char k = key[i];
        char c = text[i];
        char p = dpad(c, k);
        *(plain+i) = p;
	}
	return plain;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  dpad
 *  Description:  decrypt ciphertext character with key character
 * =====================================================================================
 */

char dpad(char c, char k){
	if (c == 32) c = 64;
	if (k == 32) k = 64;	
    char e = c - k + 64;
    if (e < 64){
        e += 27;
    }
	if (e == 64) e = 32;
    return e;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  validInput
 *  Description:  return 1 on valid string input; 0 otherwise
 * =====================================================================================
 */

int validInput(char *str){
    int i;
    for (i=0; i<strlen(str); i++){
        if ((str[i] < 65 || str[i] >90) && str[i] != 32){
            return 0;
        }
    }
    return 1;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getFile
 *  Description:  get file from path using file_size 
 * =====================================================================================
 */

char *getFile(char *path, int file_size){
	char *file = (char*) malloc(file_size);
	int rec = 0;
	int n = 0;
	int fd = open(path, O_RDONLY);
	while(rec < file_size){
		n = read(fd, file+rec, file_size-rec);
		rec += n;	
	}
	char *c;
	if ((c = strchr(file, '\n')) != NULL){
		*c = 0;
	}
	return file;
}
