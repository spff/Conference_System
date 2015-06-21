#include <iostream>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define RCVBUFSIZE 200
#define SNDBUFSIZE 200

using namespace std;

class Client{
private:
    char sendline[SNDBUFSIZE], recvline[RCVBUFSIZE];
    struct sockaddr_in servaddr;
    int remotewidth, remoteheight;


    int ConnectionEstablishReturnSockfd(){
        int sockfd;
        struct sockaddr_in servaddr;
        string cmd, host, port;

        while(true){

            host = "127.0.0.1";
            port = "3413";

            if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0){
                cerr << "Problem in creating the socket" << endl;
                continue;
            }
            cout << "Socket opened " << sockfd << endl;

            memset(&servaddr, 0, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr= inet_addr(host.c_str());
            servaddr.sin_port =  htons(atoi(port.c_str())); //convert to big-endian order

            if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
                cerr << "Problem in connecting to the server" << endl;
                return -1;
            }
            cout << "Connected successfully" << endl;
            return sockfd;
        }
    }

    void GetResolution(int sockfd){
        string resolution;

        memset(recvline, '\0', RCVBUFSIZE);
        recv(sockfd, recvline, RCVBUFSIZE, 0);
        stringstream ss(recvline);
        ss >> remotewidth >> remoteheight;

        return ;
    }

public:
    int start(){

        int sockfd = ConnectionEstablishReturnSockfd();
        if(sockfd <= 0)
            return sockfd;

        string msg = "spff";
        copy(msg.begin(), msg.end(), sendline);
        send(sockfd, sendline, SNDBUFSIZE, 0);
        GetResolution(sockfd);
        cout << remotewidth << "*" << remoteheight << endl;
        while(true){
            if(msg == "bye"){
                cout << "bye" << endl;
                close(sockfd);
                return 0;
            }
            getline(cin, msg);
    cout << "msg = \"" << msg << "\"" << endl;
            memset(sendline, '\0', SNDBUFSIZE);
            copy(msg.begin(), msg.end(), sendline);
            send(sockfd, sendline, SNDBUFSIZE, 0);
        }
    }
};

int main(int argc, char**argv){
    srand(time(NULL));
    Client a;
    return a.start();
 }
