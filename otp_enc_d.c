/*
 * =====================================================================================
 *
 *       Filename:  otp_enc_d.c
 *
 *    Description:  encrypts ciphertext with key from otp_enc client
 *
 *        Version:  1.0
 *        Created:  05/31/2016 08:49:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  OSU
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include "encryption.h"
#include "communication.h"

void sighandler(int);

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, clntsockfd, portno;
	//client length
	socklen_t clilen;

	//create server addr and client addr
	struct sockaddr_in serv_addr, cli_addr;
	if (argc < 2) {
	 fprintf(stderr,"ERROR, no port provided\n");
	 exit(1);
	}

	//create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	//zero sockaddr_in struct
	bzero((char *) &serv_addr, sizeof(serv_addr));

	//set important server stuff
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	//why all this dot blah blah dot blah blah business?
	//how about dot done
	//INADDR_ANY is symbolic constant
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	//bind socket to port
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		  sizeof(serv_addr)) < 0) {
		fprintf(stderr, "otp_enc: cannot connect to port %d\n", portno);
		exit(2);
	}

	//get it on
	//listen on socket, number of connections
	listen(sockfd,5);
	clilen = sizeof(cli_addr);

	//http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
	//create signal handler for waiting zombies to death
	struct sigaction act;
	act.sa_handler = sighandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	sigaction(SIGCHLD, &act, 0);

	while (1){

		//block until client connects
		//uses reference pointer to client
		//gets client socket fd from queue
		clntsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (clntsockfd < 0)
            perror("otp_enc_d: error on accept");


		//create new server address
        struct sockaddr_in addr;
        bzero((char*) &addr, sizeof(addr));
        socklen_t addr_size = sizeof(addr);

		//get new socket
        int newsockfd = getSock();
		//put socket info into address
        getsockname(newsockfd, (struct sockaddr *) &addr, &addr_size);

		//listen on new socket
        listen(newsockfd, 1);
		//send new socket port client
        sendNum(addr.sin_port, clntsockfd);

        bzero((char*) &cli_addr, clilen);
        close(clntsockfd);
		//accept new client connection
        clntsockfd = accept(newsockfd, (struct sockaddr*) &cli_addr, &clilen);

		//spawn little worker bee
		pid_t pid;
		if((pid = fork()) == 0){

			int n;
			char buffer[1024];
			bzero(buffer, sizeof(buffer));

			//only accept connection from otp_enc
			n = read(clntsockfd, buffer, 7);
			if (n < 0) fprintf(stderr, "otp_enc_d: cannot receive client type\n");
			if (strncmp(buffer, "OTP_ENC", strlen("OTP_ENC")) != 0) {
				//send rejection notice
				sendNum(0, clntsockfd);
				//perror("otp_enc_d: connection refused\n");
				close(clntsockfd);
				exit(2);
			}
			//send connect acceptance notice
			sendNum(1, clntsockfd);

			//get plaintext from client
			char *plaintext = getMsg(clntsockfd);
//			printf("server plain: '%s'\n", plaintext);
//			printf("server received plaintext\n");
			//get key from client
			char *key = getMsg(clntsockfd);
//			printf("server received key\n");

			//convert plainttext to ciphertext
			char *ciphertext = encrypt(key, plaintext);

			//send ciphertext
			sendMsg(ciphertext, clntsockfd);
			
			close(clntsockfd);
			exit(0);
		}
	}

	close(sockfd);
	int status;
	while(waitpid(-1, &status, WNOHANG) > 0){}
	return 0; 
}

void sighandler(int signum){
	int status;
	while(waitpid(-1, &status, WNOHANG) > 0){};
}









