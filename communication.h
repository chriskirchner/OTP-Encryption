/*
 * =====================================================================================
 *
 *       Filename:  communication.h
 *
 *    Description:  header implementation file for otp_dec/enc communication library
 *
 *        Version:  1.0
 *        Created:  05/31/2016 07:01:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  OSU
 *
 * =====================================================================================
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

int getSock();
int bindPort(struct sockaddr_in*,int);
int connectPort(int, struct sockaddr_in*, int);
int setPort(int,struct sockaddr_in*,int);
int sendPort(int,int);
int sendNum(uint32_t,int);
int getNum(int);
int sendMsg(char*,int);
char *getMsg(int);


#endif
