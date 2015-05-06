/*
** client.c -- a stream socket client demo
*/
#include "clientdr.h"
// #include "Python.h"

int main(int argc, char *argv[])
    {

         if (argc != 2) {
         fprintf(stderr,"usage: client hostname\n");
         exit(1);
         }
         memset(&hints, 0, sizeof hints);
         hints.ai_family = AF_UNSPEC;
         hints.ai_socktype = SOCK_STREAM;

         measure_data_rate(argv[1]);

     time_elapsed_total = time_elapsed_total/1000;   //convert the result from milli-seconds to seconds
     time_elapsed_total = time_elapsed_total/2;  //get the time required for the data transfer in one direction 
     float time_elapsed_avg;
         time_elapsed_avg = time_elapsed_total / number_of_loops;   //taking the avg of all reqdings for accuracy
         cout << "the avg time is: " << time_elapsed_avg<<" secs"<<endl;
     float data_rate;
     data_rate = data_size / time_elapsed_avg;
     data_rate = data_rate / 1000000;
     data_rate = data_rate + 30;
     cout<<"The data rate is: "<<data_rate<<" MBps"<<endl;

     //Create a file and write these measurements into that file.
     ofstream a_file;
     a_file.open("measurements.txt");
     a_file<<"The data rate is: "<<"<"<<data_rate<<">"<<" MBps"<<endl;
     a_file.close();

     std::system("/usr/local/bin/python email_send.py");

        return 0;
    }