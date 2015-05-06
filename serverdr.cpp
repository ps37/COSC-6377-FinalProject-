/*
** server.c -- a stream socket server demo
*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>//for getting the time
#include <iostream>
#define PORT "3490" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold
#define remote_server_port "5950" //the udp port on which the remote server will be listening to.
#define MAXBUFLEN 32*1024
using namespace std;

int count_of_loops = 0;
float time_delay = 0;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
    {
         if (sa->sa_family == AF_INET) {
         return &(((struct sockaddr_in*)sa)->sin_addr);
         }
         return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

inline void delay( float ms )
    {
        cout<<"the delay is :"<<ms<<endl;
    usleep( ms * 1000 );
    }

int main(void)
    {
        fd_set master;
        fd_set read_fds; // temp file descriptor list for select()
        int fdmax;
         int sockfd, new_fd, new_fd1, remote_sockfd; // listen on sock_fd, new connection on new_fd
         struct addrinfo hints, *servinfo, *p, hints_remote, *servinfo_remote, *p_remote;
         int numbytes, numbytes1;
         char buf[MAXBUFLEN];
        char buf1[MAXBUFLEN];
         //struct sockaddr_in *port;
         struct sockaddr_storage their_addr, their_addr_remote; // connector's address information
         socklen_t sin_size;
         struct sigaction sa;
         int yes=1;
         char s[INET6_ADDRSTRLEN];
         int rv, rv_remote;
         memset(&hints, 0, sizeof hints);
         hints.ai_family = AF_UNSPEC;   //we can use either ipv6 or ipv4
         hints.ai_socktype = SOCK_STREAM;
         hints.ai_flags = AI_PASSIVE; // use my IP
         memset(&hints_remote, 0, sizeof hints_remote);
         hints_remote.ai_family = AF_UNSPEC;   //we can use either ipv6 or ipv4
         hints_remote.ai_socktype = SOCK_STREAM;
         hints_remote.ai_flags = AI_PASSIVE; // use my IP

         FD_ZERO(&read_fds);
         FD_ZERO(&master);

        //fill in the struct addrinfo with all the info about the server side
         if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
            cout<<"i am in rv"<<endl;
         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
         return 1;
         }
        //for remote server connection
         //fill in the struct addrinfo with all the info about the server side
         if ((rv_remote = getaddrinfo(NULL, remote_server_port, &hints_remote, &servinfo_remote)) != 0) {
            cout<<"i am in rv_remote"<<endl;
         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_remote));
         return 1;
         }

    // loop through all the results and bind to the first we can
         for(p = servinfo; p != NULL; p = p->ai_next) {
         if ((sockfd = socket(p->ai_family, p->ai_socktype,
         p->ai_protocol)) == -1) {
         perror("server: socket");
         continue;
         }

         setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
         sizeof(int));

         if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
         close(sockfd);
         perror("server: bind");
         continue;
         }
         break;
         }

        freeaddrinfo(servinfo);        // all done with this structure

         if (p == NULL) 
         {
         fprintf(stderr, "server: failed to bind\n");
         return 2;
         }

         // loop through all the results and bind to the first we can
         for(p_remote = servinfo_remote; p_remote != NULL; p_remote = p->ai_next) {
         if ((remote_sockfd = socket(p_remote->ai_family, p_remote->ai_socktype,
         p_remote->ai_protocol)) == -1) {
         perror("server: socket_remote");
         continue;
         }

         setsockopt(remote_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
         sizeof(int));

         if (bind(remote_sockfd, p_remote->ai_addr, p_remote->ai_addrlen) == -1) {
         close(remote_sockfd);
         perror("server: bind_remote");
         continue;
         }
         break;
         }

        freeaddrinfo(servinfo_remote);        // all done with this structure

         if (p_remote == NULL) 
         {
         fprintf(stderr, "server: failed to bind_remote\n");
         return 2;
         }

         if (listen(remote_sockfd, BACKLOG) == -1) 
         {
         perror("listen_remote");
         exit(1);
         }

         if (listen(sockfd, BACKLOG) == -1) 
         {
         perror("listen");
         exit(1);
         }

         //add the remote server socket to listening set
         FD_SET(remote_sockfd, &master);
         FD_SET(sockfd, &master);

         //printf("server: waiting for connections on port number %d \n", &PORT);
         printf("server: waiting for connections... \n\n");

         while(1) // main accept() loop
         { 
            read_fds = master;
         //for every loop, check for connection from the remote server
        if (select(remote_sockfd+1, &read_fds, NULL, NULL, NULL) == -1) {
         perror("select");
         exit(4);
         }
         for(int i = 0; i <= remote_sockfd; i++) {
             if (FD_ISSET(i, &read_fds)) 
                {   //we got a connection from the remote server
                    if (i == remote_sockfd)
                    {
                        printf("waiting to receive data from the Remote server side....\n"); 
                        sin_size = sizeof their_addr;
                    new_fd1 = accept(remote_sockfd, (struct sockaddr *)&their_addr, &sin_size);
                        if (new_fd == -1) 
                         {
                         perror("accept");
                         continue;
                         }
                          if (!fork()) 
                            { 
                                 //receive the data from the remote server
                                 if ((numbytes = recv(new_fd1, buf1, MAXBUFLEN-1 , 0)) == -1) {
                                 perror("recv");
                                 exit(1);
                                 }
                                 buf1[numbytes] = '\0';
                                 cout<<"server got info from the remote server: "<<buf1<<endl;
                                 cout<<"done with remote_server"<<endl;
                                 time_delay = ::atof(buf1);
                                 cout<<"the time delay to be implemented is: "<<time_delay<<endl;
                                 close(new_fd1); // parent doesn't ne
                            }
                    }
                    else
                    {
                         printf("waiting to receive data from the client side....\n"); 
                         sin_size = sizeof their_addr;
                         new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                         if (new_fd == -1) {
                         perror("accept");
                         continue;
                         }
                         inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), 
                         s, sizeof s);
                        printf("server: got connection from %s\n", s);
                        //begin the fork for client connection
                         if (!fork()) 
                         { 
                             //receive the data from client into a buffer
                             if ((numbytes = recv(new_fd, buf, MAXBUFLEN-1 , 0)) == -1) {
                             perror("recv");
                             exit(1);
                             }
                             //get the time2 into a varaiable
                             struct timeval tp2;
                             gettimeofday(&tp2, NULL);
                             long int time2 = tp2.tv_sec * 1000 + tp2.tv_usec / 1000; //get current timestamp in milliseconds
                             cout << "received time: "<<time2 << endl;
                             //print the adress of the client 
                             printf("server got packet from client at %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
                             //print the number of bytes received and print the bytes.
                             printf("server: received packet is %d bytes long\n", numbytes);
                             buf[numbytes] = '\0';
                             //printf("client: packet contains \"%s\"\n", buf);

                             //Add some delay here according to the desired data rate.
                              cout<<"\n befor delay"<<endl;
                              cout<<time_delay<<endl;
                                usleep( time_delay * 100000 );
                                cout<<"after delay \n"<<endl;

                            //sending the capitalized string to the client
                             if ((numbytes1 = send(new_fd, buf, strlen(buf), 0)) == -1) {
                             perror("server: send");
                             exit(1);
                             }
                             printf("server: sent packet is %d bytes long\n", numbytes1);
                             //get the time4 into a varaiable
                             struct timeval tp4;
                             gettimeofday(&tp4, NULL);
                             long int time4 = tp4.tv_sec * 1000 + tp4.tv_usec / 1000; //get current timestamp in milliseconds
                             cout << "sent time: "<<time4 << endl;

                             close(new_fd); // parent doesn't need this
                             cout<<"*****DONE*****\n"<<endl;
                         } //end of if condition of the child process
                    }//end of else

                } //end of if condition to check for the remote server connection.
        }//for loop

        } // end of while loop.
         close(sockfd);
         return 0;
    }