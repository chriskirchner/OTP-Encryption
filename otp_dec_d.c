/*
 * =====================================================================================
 *
 *       Filename:  otp_dec_d.c
 *
 *    Description:  decrypts ciphertext with key received from otp_dec client
 *
 *        Version:  1.0
 *        Created:  05/31/2016 08:49:32 PM
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
#include "communication.h"
#include "encryption.h"

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
		  sizeof(serv_addr)) < 0) 
		  error("ERROR on binding");
	//get it on
	//listen on socket, number of connections
	listen(sockfd,5);
	clilen = sizeof(cli_addr);

    //http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
	//create signal handler for reaping using SA_RESTART to not interrupt
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
			perror("otp_dec_d: error on accept");

		//create new server addr
		struct sockaddr_in addr;
		bzero((char*) &addr, sizeof(addr));
		socklen_t addr_size = sizeof(addr);
		//create new server socket
		int newsockfd = getSock();

		//put new server socket info into new addr
		getsockname(newsockfd, (struct sockaddr *) &addr, &addr_size);
		listen(newsockfd, 1);
		//send new address port
		sendNum(addr.sin_port, clntsockfd);	

		//zero client address
		bzero((char*) &cli_addr, clilen);
		close(clntsockfd);
		//accept new incoming requests
		clntsockfd = accept(newsockfd, (struct sockaddr*) &cli_addr, &clilen);

		//spawn child process for concurrency
		pid_t pid;
		if((pid = fork()) == 0){

			int n;
			char buffer[1024];
			bzero(buffer, sizeof(buffer));

			listen(clntsockfd, 1);
			//only accept connection from otp_dec
			n = read(clntsockfd, buffer, 7);
			if (n < 0) fprintf(stderr, "otp_dec_d: cannot read socket");
			if (strncmp(buffer, "OTP_DEC", strlen("OTP_DEC")) != 0) {
				//send 0 for disconnect
				sendNum(0, clntsockfd);
				//perror("otp_dec_d: connection refused\n");
				close(clntsockfd);
				//apoptosis
				exit(2);
			}
			//send 1 for accepted connection
			sendNum(1, clntsockfd);

			//receive ciphertext
			char *ciphertext = getMsg(clntsockfd);
//			printf("server received ciphertext\n");
			//receive key
			char *key = getMsg(clntsockfd);
//			printf("server received key\n");

			//convert plainttext to ciphertext
			char *plaintext = decrypt(key, ciphertext);

			//send plaintext
			sendMsg(plaintext, clntsockfd);
			
			close(clntsockfd);
			exit(0);
		}
		else {
			close(clntsockfd);
		}
	}

	close(sockfd);
	return 0; 
}


void sighandler(int signum){
	int status;
	waitpid(-1, &status, WNOHANG);
}



