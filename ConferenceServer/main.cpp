#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <vector>
#include <set>
#include <mutex>

#include <X11/Xlib.h>


extern "C" {
    #include "popen_noshell.h"
}

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE);} while (0)



#define PORT 3412
#define LISTENQ 8 /*maximum number of client connections */
#define RCVBUFSIZE 200
#define SNDBUFSIZE 200

using namespace std;

set<string> LockFileSet;
mutex FileCheckMutex;

class OneConnection{

    private:

        char sendline[SNDBUFSIZE], recvline[RCVBUFSIZE];
        int sockfd;
        string cmd, ip, usrname;

    public:

        OneConnection(int sockfd){
            this->sockfd = sockfd;
        }

        void start(string ip){
            this->ip = ip;
            if(GetUsrname() != 0)
                return;
            if(CheckPermission() == false)
                return;

            SendResolution();

            thread t1{&OneConnection::PopOutTerminal, this};
            thread t2{&OneConnection::GetSignalTillEnd, this};

            t1.join();
            t2.join();
            close(sockfd);
        }

    private:
        int GetUsrname(){
            stringstream ss;
            int n;
            memset(recvline, '\0', RCVBUFSIZE);
            if((n = read(sockfd, recvline, RCVBUFSIZE)) <=0){
                cout << "Client disconnected." << endl;
                return n;
            }
            ss << recvline;
            ss >> usrname;
            return 0;
        }

        bool CheckPermission(){
            ifstream ifs("record", ios::binary);
            stringstream buffer;
            buffer << ifs.rdbuf();
            bool registered = (buffer.str().find(ip + " " + usrname) != string::npos);
            ifs.close();

            if(registered)
                return true;
            while(true){
                cout << "Accept connection?(y,n)";
                getline(cin, cmd);
                if((cmd == "y") || (cmd == "Y")){
                    ofstream ofs("record", ios::binary);
                    ofs << ip << " " << usrname << endl;
                    ofs.close();
                    return true;
                }

                if((cmd == "n") || (cmd == "N"))
                    return false;
            }
        }

        void SendResolution(){
            Display* disp = XOpenDisplay(NULL);
            Screen*  scrn = DefaultScreenOfDisplay(disp);
            string fakereso = scrn->width + " " + scrn->height;
            write(sockfd, fakereso.c_str(), fakereso.length());
        }

        int PopOutTerminal(){
            FILE *pin;
            struct popen_noshell_pass_to_pclose pclose_arg;

            string buff = "xterm -e ssh -Y " + usrname + "@" + ip;
            ofstream script("login.sh");
            script << buff;
            script.close();

            pin = popen_noshell_compat("./login.sh", "w", &pclose_arg);
            if(!pin)
              errExit("popen_noshell_compat");
            pclose_noshell(&pclose_arg);
            printf("Bye\n");
            return 0;
        }

        void GetSignalTillEnd(){

            while(true){
                int nbytes;
                if((nbytes = recv(sockfd, recvline, RCVBUFSIZE, 0)) <= 0){
                    cout << "Client disconnected." << endl;
                    close(sockfd);
                    return;
                }
                cout << recvline << endl;

                /*if(isMouse){
                    MouseAct();
                }
                else if(iskeyboard){
                    KeyboardAct();
                }*/
            }
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
                string ip(inet_ntoa(cliaddr.sin_addr));
                cout << "Received request from ";
                cout << ip << ":" << ntohs(cliaddr.sin_port) << endl;

                OneConnection a{connfd};
                thread{&OneConnection::start, a, ip}.detach();
            }
            close(listenfd);
        }

};
int main(int argc, char**argv){
    Server a;
    return a.start();
}
