/*
 * =====================================================================================
 *
 *       Filename:  encryption.h
 *
 *    Description:  header file for encryption otp_dec/enc library
 *
 *        Version:  1.0
 *        Created:  05/31/2016 07:36:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chris Kirchner (CLK), kirchnch@oregonstate.edu
 *   Organization:  OSU
 *
 * =====================================================================================
 */


#ifndef ENCRYPTION_H
#define ENCRYPTION_H

char *encrypt(char*,char*);
int epad(char,char);
char *decrypt(char*,char*);
char dpad(char,char);
int validInput(char*);
char *getFile(char*,int);

#endif
