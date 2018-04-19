/*
 * =====================================================================================
 *
 *       Filename:  communication.c
 *
 *    Description:  implementation file for socket communication otp_dec/enc library
 *
 *        Version:  1.0
 *        Created:  05/31/2016 07:01:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  OSU
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "communication.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sendNum
 *  Description:  send unsigned 32-bit integer to socket
 * =====================================================================================
 */

int sendNum(uint32_t num, int sockfd){
    int wn = write(sockfd, (char*) &num, sizeof(num));
	return wn;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getNum
 *  Description:  receive integer from socket 
 * =====================================================================================
 */

int getNum(int sockfd){
	int num;
	int r = read(sockfd, (char*) &num, sizeof(num));
	if (r > 0){
		return num;
	}
	else {
		return 0;
	}
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getSock
 *  Description:  get socket with bind-assigned port number
 * =====================================================================================
 */

int getSock(){

	struct sockaddr_in tmpaddr;
	bzero((char*) &tmpaddr, sizeof(tmpaddr));
	tmpaddr.sin_family = AF_INET;
	tmpaddr.sin_addr.s_addr = INADDR_ANY;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(sockfd, (struct sockaddr*) &tmpaddr, sizeof(tmpaddr));

	return sockfd;

}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  bindPort
 *  Description:  bind internet address and socket
 * =====================================================================================
 */

int bindPort(struct sockaddr_in *serv_addr, int sockfd){

	if (bind(sockfd, (struct sockaddr *) serv_addr, sizeof(struct sockaddr_in)) == 0){
		return serv_addr->sin_port;    
	}
    return 0;
}  


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  connectPort
 *  Description:  connect internet address to socket 
 * =====================================================================================
 */

int connectPort(int port, struct sockaddr_in *serv_addr, int sockfd){
	serv_addr->sin_port = htons(port);
	if (connect(sockfd, (struct sockaddr*) serv_addr, sizeof(serv_addr)) == 0){
		return 1;
	}
	return 0;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sendPort
 *  Description:  wrapper for sendNum
 * =====================================================================================
 */

int sendPort(int port, int sockfd){
	return sendNum(port, sockfd);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setPort
 *  Description:  set port number to address and socket 
 * =====================================================================================
 */


int setPort(int port, struct sockaddr_in *addr, int sockfd){
	addr->sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr *) addr, sizeof(struct sockaddr_in)) == 0){
		return port;    
	}
	return 0;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sendMsg
 *  Description:  sends message to socket
 * =====================================================================================
 */

int sendMsg(char *msg, int sockfd){

	//send message size
    int msg_size = strlen(msg);
	if (sendNum(msg_size, sockfd) < 0) return -1;

	//send message
    int sent = 0;
	while (sent < msg_size){
		int wn = write(sockfd, msg+sent, msg_size-sent);
		if (wn < 0) return -1;
		sent += wn;
	}

	return sent;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getMsg
 *  Description:  receive message from socket
 * =====================================================================================
 */

char *getMsg(int sockfd){
	
	int n;

	//get msg size 
	uint32_t msg_size;
	n = read(sockfd, (char*) &msg_size, sizeof(msg_size));

	//get msg
	char *msg = (char*) malloc(msg_size);	
	int rec = 0;
	while (rec < msg_size){
		n = read(sockfd, msg+rec, msg_size-rec);
		if (n < 0) return 0;
		rec += n;
	}
	
	return msg;
}


