#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <vector>
#include <set>
#include <mutex>


extern "C" {
    #include "popen_noshell.h"
}

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE);} while (0)

#define MAX_LEN 256
#define BUFF_LEN 1024


#define PORT 3412
#define LISTENQ 8 /*maximum number of client connections */
#define RCVBUFSIZE 200
#define SNDBUFSIZE 200

using namespace std;

set<string> LockFileSet;
mutex FileCheckMutex;
class OneConnection{

private:

    vector<char> tempvector;
    bool finishreading;
    char sendline[SNDBUFSIZE], recvline[RCVBUFSIZE];
    int sockfd;
    string cmd;


    int GetCmdFromClientandPassToSSH(){
        stringstream ss;
        string cmd;
        int n;

        memset(recvline, '\0', RCVBUFSIZE);
        if((n = read(sockfd, recvline, RCVBUFSIZE)) <=0){
            cout << "Client disconnected." << endl;
            return n;
        }
        ss << recvline;
        ss >> cmd;
        ss.str("");
        ss.clear();

        return 0;
    }


//TODO MAKE SSH able to read cmd from client

    int LoginViaSSH(){
        char username[MAX_LEN], ip[MAX_LEN], password[MAX_LEN];
        char buff[BUFF_LEN];
        FILE *pin;
        int script_fd;
        struct popen_noshell_pass_to_pclose pclose_arg;

        // get username and ip password
        printf("IP: ");
        fgets(ip, MAX_LEN, stdin);
        ip[strlen(ip) - 1] = '\0';
        printf("Username: ");
        fgets(username, MAX_LEN, stdin);
        username[strlen(username) - 1] = '\0';
        printf("Password: ");
        fgets(password, MAX_LEN, stdin);
        password[strlen(password) - 1] = '\0';

        // create expect script
        sprintf(buff, "#!/usr/bin/expect -f\n"
              "set timeout 30\n"
              "spawn ssh -Y %s@%s\n"
              "expect \"password:\"\n"
              "send \"%s\\r\"\n"
              "expect \"~$*\"\n"
              "interact\n", username, ip, password);
        script_fd = open("login.sh", O_WRONLY|O_CREAT, S_IRWXU|S_IXGRP);
        if (write(script_fd, buff, strlen(buff)) < 0)
          errExit("write");
        if (close(script_fd))
          errExit("close");

        // execute script
        pin = popen_noshell_compat("./login.sh", "w", &pclose_arg);
        if(!pin)
          errExit("popen_noshell_compat");

        while(1)
        {
          // read command from standard input
          fgets(buff, BUFF_LEN, stdin);

          // write to shell input
          fputs(buff, pin);
          fflush(pin);
        }
        pclose_noshell(&pclose_arg);
        printf("Bye\n");
        return 0;
    }


public:

    OneConnection(int sockfd){
        this->sockfd = sockfd;
    }

    void start(){
        LoginViaSSH();
    }
};

class Server{

private:
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    int listenfd, connfd, n;

public:
    int start(){

        listenfd = socket (AF_INET, SOCK_STREAM, 0);
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);

        bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

        listen (listenfd, LISTENQ);
        cout << "Server running...waiting for connections." << endl;

        while(true){
            clilen = sizeof(cliaddr);
            connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
            cout << "Received request from ";
            cout << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << endl;
            OneConnection a{connfd};
            thread{&OneConnection::start, a}.detach();
        }
        close(listenfd);
    }

};
int main(int argc, char**argv){
    Server a;
    return a.start();
}
