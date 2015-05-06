/*
** listener.c -- a datagram sockets "server" demo
*/
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
#define MYPORT "4950" // the port users will be connecting to
#define server_port "5950"//the UDP port used on sending server
#define MAXBUFLEN 100
using namespace std;

float desired_data_rate = 0; //MBps
string previous_date = "";

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
 if (sa->sa_family == AF_INET) {
 return &(((struct sockaddr_in*)sa)->sin_addr);
 }
 return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

string convert_2_string (float number)
    {
    ostringstream buff;
    buff<<number;
    return buff.str();   
}

int main(int argc, char *argv[])
{
     if (argc != 3) {
         fprintf(stderr,"usage: server hostname or desired speed not entered correct\n");
         exit(1);
         }

desired_data_rate = ::atof(argv[2]);         

 int sockfd, server_sockfd;
 struct addrinfo hints, *servinfo, *p, hints_server, *servinfo_server, *p_server;
 int rv, rv_server;
 int numbytes, numbytes1;
 struct sockaddr_storage their_addr, their_addr_server;
 char buf[MAXBUFLEN];
 socklen_t addr_len;
 char s[INET6_ADDRSTRLEN];

memset(&hints_server, 0, sizeof hints_server);
 hints_server.ai_family = AF_UNSPEC;
 hints_server.ai_socktype = SOCK_STREAM;

 class email_content
 {
 public:
    string present_date;
    string data_rate;
    email_content(string d, string d_r)
    :present_date(d), data_rate(d_r){}
 };

string data_rate = "";
    string date = "";

while(true)
{
//for sending server info
if ((rv_server = getaddrinfo(argv[1], server_port, &hints_server, &servinfo_server)) != 0) {
 fprintf(stderr, "getaddrinfo_server: %s\n", gai_strerror(rv_server));
 return 1;
 }
//creating socket on remote server of sending_server socket type
// loop through all the results and connect to the first we can
        for(p_server = servinfo_server; p_server != NULL; p_server = p_server->ai_next) 
         {
         if ((server_sockfd = socket(p_server->ai_family, p_server->ai_socktype,
         p_server->ai_protocol)) == -1) {
         perror("remoteserver_server: socket");
         continue;
         }
         if (connect(server_sockfd, p_server->ai_addr, p_server->ai_addrlen) == -1) {
         close(server_sockfd);
         perror("remote_server_server: connect");
         continue;
         }
         break;
         }

         if (p_server == NULL) {
         fprintf(stderr, "remote_server_server: failed to connect\n");
         return 2;
         }

         inet_ntop(p_server->ai_family, get_in_addr((struct sockaddr *)p_server->ai_addr),
         s, sizeof s);
         printf("remote_server_server: connecting to sending_server at %s\n", s);

         freeaddrinfo(servinfo_server); // all done with this structure
//begin the loop for checking the latest emails from client.
    while(1)
    {
    //read email sent by client and write it to a file
    std::system("/usr/local/bin/python email_read.py"); 
    //read from the file measurements_read.txt and assign the data rate to variable thats being sent to server
    ifstream myReadFile;
     myReadFile.open("measurements_read.txt");
     string file_contents;
     if (myReadFile.is_open()) {
     getline(myReadFile, file_contents);
     }
    //cout<<file_contents<<endl;
    myReadFile.close();
    data_rate = "";
    date = "";
    //parse the file contents for the data rate.
    for(int i = 0; i < file_contents.size(); i++)
    {
        if (file_contents[i] == '\"')
        {
            i++;
            while(file_contents[i] != '\"')
            {
                //cout<<"while"<<file_contents[i]<<endl;
                date += file_contents[i];
                i++;
            }//while
        } //if
        //cout<<date<<endl;
        while(i < file_contents.size())
        {
            if (file_contents[i] == '<')
            {
                i++;
                while(file_contents[i] != '>')
                {
                    //cout<<"while"<<file_contents[i]<<endl;
                    data_rate += file_contents[i];
                    i++;
                }//while
            }//if
            i++;
        }//while
        //cout<<data_rate<<endl;
    }//for

        email_content e_c = email_content(date, data_rate);
        if ( (e_c.present_date == "") || (e_c.present_date == previous_date) )
        {//continue looping to get the latest email
            cout<<"**waiting for new email.... "<<endl;
            continue;
        }
        else if ( (e_c.present_date != "") && (e_c.present_date != previous_date) )
        {
            previous_date = e_c.present_date;
            cout<<"\n new mail and the date is: "<<e_c.present_date<<" and the data rate is: "<<data_rate<<endl;
            break;
        }
    }//inner never ending while.

    float measured_data_rate = ::atof(data_rate.c_str());
    float time_delay_4_server = 32 / desired_data_rate;
    time_delay_4_server = time_delay_4_server - (32 / measured_data_rate);
    cout<<"\n The time delay calculated for server is: "<<time_delay_4_server<<endl;
    string time_delay = convert_2_string(time_delay_4_server);

    if(time_delay_4_server > 0)
    {
             if ((numbytes = send(server_sockfd, time_delay.c_str(), strlen(time_delay.c_str()), 0)) == -1) {
                perror("remote_server_server: send");
                exit(1);
                }
                cout<<"\n remote server sent the time delay to server"<<endl;
    }
    else
       cout<<"desired data rate is too low to regulate."<<endl;

}//one more neverending while
close(server_sockfd);
 return 0;
}