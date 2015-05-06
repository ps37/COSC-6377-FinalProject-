
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>//for getting the time
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define PORT "3490" // the port client will be connecting to
#define udp_port "4950" //the udp port on which the remote server will be listening to.
#define MAXDATASIZE 32*1024 // max number of bytes we can get at once
#define MAXBUFLEN 24
long data_size = 32*1024;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
    {
         if (sa->sa_family == AF_INET) {
         return &(((struct sockaddr_in*)sa)->sin_addr);
         }
         return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

 int sockfd, numbytes, numbytes1;
         char buf[MAXDATASIZE];
         struct addrinfo hints, *servinfo, *p;
         int rv;
         char s[INET6_ADDRSTRLEN];

         //varaiables to find the time interval
          float time_elapsed_total = 0;
          int time_elapsed;
          int number_of_loops = 30; 

//helper function to convert float to string
 string convert_2_string (float number)
    {
    ostringstream buff;
    buff<<number;
    return buff.str();   
}

//helper function to measure the data rate.
int measure_data_rate(string argv1)
    {
        //start the loop for multiple transmissions.
         for (int i = 0; i < number_of_loops; ++i)
         {
            //cout<<"WTF"<<endl;
         if ((rv = getaddrinfo(argv1.c_str(), PORT, &hints, &servinfo)) != 0) {
         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
         return 1;
         }

         // loop through all the results and connect to the first we can
         for(p = servinfo; p != NULL; p = p->ai_next) 
         {
         if ((sockfd = socket(p->ai_family, p->ai_socktype,
         p->ai_protocol)) == -1) {
         perror("client: socket");
         continue;
         }
         if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
         close(sockfd);
         perror("client: connect");
         continue;
         }
         break;
         }

         if (p == NULL) {
         fprintf(stderr, "client: failed to connect\n");
         return 2;
         }

         inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
         s, sizeof s);
         //printf("client: connecting to %s\n", argv[1]);
         freeaddrinfo(servinfo); // all done with this structure

         //create a data(string) of 32KB in size
         string data;
         
         char c = 'a';
         for (int i = 0; i<(data_size); i++)
         {
             data += c;
         }
          //sending the data to server
            if ((numbytes = send(sockfd, data.c_str(), strlen(data.c_str()), 0)) == -1) {
            perror("client: send");
            exit(1);
            }
            //printf("client: sent %d bytes to %s\n", numbytes, argv[1]);

            //get the sent time into a varaiable
            struct timeval tp;
            gettimeofday(&tp, NULL);
            long int sent_time = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
            //cout << sent_time << endl;
               
            //receiving the data from the server   
            if ((numbytes1 = recv(sockfd, buf, strlen(data.c_str()), 0)) == -1) {
            perror("recv");
            exit(1);
            }
            buf[numbytes1] = '\0';
            //printf("client: received packet is %d bytes long\n", numbytes1);

            //get the received time into a varaiable
            struct timeval tp2;
            gettimeofday(&tp2, NULL);
            long int received_time = tp2.tv_sec * 1000 + tp2.tv_usec / 1000; //get current timestamp in milliseconds
            //cout << received_time << endl;

            //get the time elapsed
            time_elapsed = received_time - sent_time;
            //cout<<time_elapsed<<endl;
         time_elapsed_total = time_elapsed_total + time_elapsed; 
         //printf("client: received '%s'\n",buf);
         close(sockfd);
     }
     return 0;
    }