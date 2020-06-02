#ifndef __SCTP_CLIENT_H__
#define __SCTP_CLIENT_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netinet/sctp.h>
#include<arpa/inet.h>
#include<pthread.h>
#include <unistd.h>
#include <iostream>



void readit(void *vfdp);
void sctp_connect(int &fd,struct sockaddr_in *addr);
int disconnect(int socket);
int send_data(int fd, iovec *iov, size_t iovlen);



#endif	// __SCTP_CLIENT_H__