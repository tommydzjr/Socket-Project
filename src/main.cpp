#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <thread>
#define PORT 7000
#define NUMBER_OF_THREDS 6
#define QUEUE 20 

int ss;
struct sockaddr_in client_addr;
socklen_t length = sizeof(client_addr);

void thread_fn() {
    while(true) {
        int conn = accept(ss, (struct sockaddr*)&client_addr, &length);
        if( conn < 0 ) {
            perror("connect");
            exit(1);
        }

        fd_set rfds; // file discriptor reference collection
        struct timeval tv;
        int retval, maxfd;

        while(1) {
            FD_ZERO(&rfds); // initialise to zero
            FD_SET(0, &rfds); // add stdin
            maxfd = 0; // next file description ID
            FD_SET(conn, &rfds); // add current connection to the list of file descriptiors

            if(maxfd < conn){
                maxfd = conn; // set the ID to the current connection ID
     	    }

            // 'select' setup
            tv.tv_sec = 5;
            tv.tv_usec = 0;

            retval = select(maxfd+1, &rfds, NULL, NULL, &tv); // block and wait for a ready file descriptor
        
            if(retval == -1){
                std::cerr <<  "select() returned -1" << std::endl;
            }else if(retval) {
                /*The client sent a message.*/
                if(FD_ISSET(conn,&rfds)){
                    char buffer[1024];    
                    memset(buffer, 0 ,sizeof(buffer));
                    int len = recv(conn, buffer, sizeof(buffer), 0);
                
                    std::cout << "------------------ request from client ------------------------------" << std::endl
                    << buffer << std::endl
                    << "------------------- end request from client ----------------------------------" << std::endl;
                
                    /*-----------------------------------------------------------------------
                    Handle the request here
                    1. Parse the http header
                    2. Send response back to the client
                    -------------------------------------------------------------------------*/
                    send(conn, buffer, sizeof(buffer), 0); // example of a response

		            break; // disconnect the client by breaking out
            }
        }
    }
     close(conn);
    }	
}

int main() {
    ss = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(PORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(ss, (struct sockaddr* ) &server_sockaddr, sizeof(server_sockaddr))==-1) {
        perror("bind");
        exit(1);
    }
    if(listen(ss, QUEUE) == -1) {
        perror("listen");
        exit(1);
    }

    for (size_t i = 0; i < NUMBER_OF_THREDS; i++) {
        std::thread a_thread{thread_fn};
        a_thread.join();
    }
    
    close(ss);
    return 0;
}
