#include <sstream>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
string myname;

int ConnectionEstablishReturnSockfd(){
    int sockfd;
    struct sockaddr_in servaddr;
    string cmd, host, port;

    while(true){
        char split_char = ' ';
        getline(cin, cmd);
        istringstream split(cmd);
        vector<string> tokens;
        for (string each; getline(split, each, split_char); tokens.push_back(each));

            if(tokens.size() !=4){
                cerr << "invalid command.Type \"connect <host> <port> <username>\"" << endl;
                continue;
            }

            cmd = tokens[0];
            if(cmd != "connect"){
                cerr << "invalid command.Type \"connect <host> <port> <username>\"" << endl;
                continue;
            }
            host = tokens[1];
            port = tokens[2];
            myname = tokens[3];
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
                continue;
            }
            cout << "Connected successfully" << endl;
            write(sockfd, myname.c_str(), strlen(myname.c_str()));
            return sockfd;
    }
}


int main(int argc, char **argv){
    char buf[1024];
    int sockfd = ConnectionEstablishReturnSockfd();

    fd_set rfds, tpfds;
    FD_ZERO(&rfds);
    FD_SET(fileno(stdin), &rfds);
    FD_SET(sockfd, &rfds);

    while (true){
        cout << endl;//NEVER REMOVE THIS LINE, OR ELSE THE OUTPUT FROM OTHER CLIENT MAY NOT DISPLAY UNTIL ANY ENTER PRESSED
        tpfds = rfds;
        select(sockfd+1, &tpfds, NULL, NULL, NULL);

        if(FD_ISSET(sockfd, &tpfds)){
            int nbytes;
            if((nbytes = recv(sockfd, buf, sizeof(buf), 0)) <= 0){
                cerr << "recv() error!";
                close(sockfd);
                return 1;
            }
            cout << buf;
            usleep(10);
            memset(buf, '\0', sizeof(buf));
            continue;
        }
        if(FD_ISSET(0, &tpfds)){
            string cmd;
            cin >> cmd;
            if(cmd == "bye")
                break;

            if(cmd == "chat"){
                string content;
                cin.ignore(1);
                getline(cin, content);
                content += "\r\n";
                write(sockfd, content.c_str(), content.size());
            }
            else if(cmd.length() != 0)
                cerr << "Type \"chat\" or \"bye\"" << endl;
        }
    }

    close(sockfd);
    cout << "Goodbye." << endl;
    return 0;
}
