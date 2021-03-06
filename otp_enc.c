/*
 * =====================================================================================
 *
 *       Filename:  otp_enc.c
 *
 *    Description:  encrypt plaintext with key as client to otp_enc_d
 *
 *        Version:  1.0
 *        Created:  05/28/2016 06:00:03 PM
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "communication.h"
#include "encryption.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void stderror(const char *msg){
	fprintf(stderr,"%s\n", msg);
	exit(1);
}

//first arg = plaintext
//second arg = key
//third arg = port

int main(int argc, char *argv[])
{
	//important stuff
    int sockfd, portnum, n;

	//setup socket address internet (?)
	//struct sockaddr_in
	//{
	//   short   sin_family; /*  must be AF_INET */
	//   u_short sin_port;
	//   //in_addr only contains one field, sin_addr (wtf?)
	//   struct  in_addr sin_addr;
	//   char    sin_zero[8]; /*  Not used, must be zero */
	//};
    struct sockaddr_in serv_addr;

	//
	//struct  hostent
	//{
	//  char    *h_name;        /*  official name of host */
	//  char    **h_aliases;    /*  alias list */
	//  int     h_addrtype;     /*  host address type */
	//  int     h_length;       /*  length of address */
	//  char    **h_addr_list;  /*  list of addresses from name server */
	//  #define h_addr  h_addr_list[0]  /*  address, for backward compatiblity */
	//};
    struct hostent *server;

    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

	//get port	
    portnum = atoi(argv[3]);

	//create socket gives TCP
	//SOCK_DGRAM tends for UDP
	//0 - protocol that chooses UDP for SOCK_DGRAM, TCP for SOCK_STREAM
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

	//resolve address to hostent struct
    server = gethostbyname("localhost");
    if (server == NULL) {
		error("otp_enc: no such host");
    }

	//zero server address
    bzero((char *) &serv_addr, sizeof(serv_addr));

	//indicate internetwork address family
	//other domain is AF_UNIX for two processes, one filesytem
    serv_addr.sin_family = AF_INET;
	
	//copy address to sockaddr_in struct member's member
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

	//hton - big endian to little endia
    serv_addr.sin_port = htons(portnum);

	//(fd, sockaddr_in, size of sockaddr_in); why size?
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		fprintf(stderr,"otp_enc: could not contact otp_enc_d on port %d\n", portnum); 
		exit(1);
	}


    //get new port from server
    int newsockfd = 0;
    newsockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (newsockfd < 0) error("otp_enc: error getting new socket\n");
    int serv_port = ntohs(getNum(sockfd));

    //assign new address to socket
    struct sockaddr_in tmpaddr;
    bzero((char*) &tmpaddr, sizeof(tmpaddr));
    tmpaddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    tmpaddr.sin_port = htons(serv_port);

    close(sockfd);
    //connect with new socket
    if (connect(newsockfd,(struct sockaddr *) &tmpaddr, sizeof(tmpaddr)) < 0)
        error("otp_enc: cannot connect to new port\n"); 
    sockfd = newsockfd;
	

	//send sender type to server
	char sender[] = "OTP_ENC";
    n = write(sockfd, sender, strlen(sender));
	if (n < 0) error("otp_enc: cannot send client type\n");
    //generate error on rejection
	n = getNum(sockfd);
	if (n == 0) {
		fprintf(stderr,"otp_enc: cannot connect to otp_dec\n");
		exit(2);
	}
	//send plaintext
	struct stat sb;
	stat(argv[1], &sb);
	int ptext_size = sb.st_size;
	char *plaintext = getFile(argv[1], ptext_size);

	//send key 
	stat(argv[2], &sb);
	int key_size = sb.st_size;
	char *key = getFile(argv[2], key_size);

    //generate errors on invalid input and insufficient key length
	if (strlen(plaintext) > strlen(key)){
		stderror("otp_enc: key size is too small");
	}
	else if(!validInput(plaintext)){
		stderror("otp_enc: plaintext file has invalid input");
	}
	else if(!validInput(key)){
		stderror("otp_enc: key file has invalid input");
	}

	//refactor later (?) to use file string instead of path name
//	sendFile(argv[1], sockfd);
//	sendFile(argv[2], sockfd);

    //send plaintext
	sendMsg(plaintext, sockfd);
    //send key
	sendMsg(key, sockfd);

	//get ciphertext
	char *ciphertext = getMsg(sockfd);		
	printf("%s\n", ciphertext);

    close(sockfd);
    return 0;
} 

