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

#include <signal.h>
#include <sys/prctl.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <vector>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XInput2.h>


#define PORT 3412
#define LISTENQ 8 /*maximum number of client connections */
#define RCVBUFSIZE 200
#define SNDBUFSIZE 200

using namespace std;

class OneConnection{

private:

    char sendline[SNDBUFSIZE], recvline[RCVBUFSIZE];
    int sockfd;
    string cmd, ip, usrname;
    pid_t terminalpid;

    Display *dpy;
    Window window;

public:

    OneConnection(int sockfd){
        this->sockfd = sockfd;
    }

    void start(string ip){
        this->ip = ip;
        if(GetUsrname() != 0)
            return;
        //if(CheckPermission() == false)
        //    return;

        PopOutTerminalandSetPid();
        SendResolution();
        GeneratePointer();
        thread t{&OneConnection::GetSignalTillEnd, this};
        t.join();
        RemovePointer();
        kill(terminalpid, SIGTERM);
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
        string buffer;
        bool registered = false;
        while(getline(ifs, buffer)){
            registered = (buffer.find(ip + " " + usrname) != string::npos);
            if(registered){
                ifs.close();
                return true;
            }
        }
        ifs.close();

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
        string resolution = to_string(scrn->width) + " " + to_string(scrn->height);
        resolution += "\r\n";

        memset(sendline, '\0', SNDBUFSIZE);
        copy(resolution.begin(), resolution.end(), sendline);
        write(sockfd, sendline, resolution.length());

    }

    /**
      * generate a new mouse pointer
      * @param  name  given a mouse pointer name
      * @return On success, generated mouse pointer's device id
      *         On failure, return -1
      */
    int  GeneratePointer(char *name){

        XIAddMasterInfo c;
        XIDeviceInfo *info;
        int ndevices, deviceid, i;
        const int MaxNameLen = 1024;
        char devicename[MaxNameLen];


        deviceid = -1;
        c.type = XIAddMaster;
        c.name = name;
        c.send_core = 1;
        c.enable = 1;

        Status s = XIChangeHierarchy(dpy, (XIAnyHierarchyChangeInfo*)&c, 1);

        if (!s)
        {
          // get device id
          info = XIQueryDevice(dpy, XIAllDevices, &ndevices);
          strncpy(devicename, name, MaxNameLen);
          strncat(devicename, " pointer", MaxNameLen);
          for(i = 0; i < ndevices; i++)
          {
          // printf("device: %s\t id: %d\n", info[i].name, info[i].deviceid);
          if (!strncmp(info[i].name, devicename, MaxNameLen))
          {
              deviceid = info[i].deviceid;
              break;
          }
          }
        }

        return deviceid;
    }

    void RemovePointer(){

        //run "xinput -remove-master \"" + ip + usrname + "\"";
    }

    int PopOutTerminalandSetPid(){
        pid_t proc = fork();

        if (proc < 0){
            printf("Error: Unable to fork.\n");
            exit(EXIT_FAILURE);
        }
        else if (proc == 0){
            if (execlp("mate-terminal", "mate-terminal", "-x", "ssh", "-Y", "140.117.183.100", NULL)){
                printf("Error: Unable to load the executable.\n");
                exit(EXIT_FAILURE);
            }
        }
        else{
            terminalpid = proc;
            int status = -1;
        }

        return EXIT_SUCCESS;
    }


    void GetSignalTillEnd(){

        init();

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


    Bool init(){
        // connect to X server
        // arg: NULL, it defaults to the value of the DISPLAY environment variable.
        if ((dpy=XOpenDisplay(NULL)) == NULL){
            fprintf(stderr, "fail to open display!\n");
            return False;
        }

        //returns the root window for the default screen.
        window = DefaultRootWindow(dpy);

        return True;
    }



    /**
     * @param which: which button
     * 	1 - "left",
     * 	2 - "middle",
     * 	3 - "right",
     * 	4 - "fourth",
     * 	5 - "fifth"
     * @param what: what event is
     * 	ButtonPress
     * 	ButtonRelease
     */
    Bool GenerateMouseEvent(int which, int what){
        return XTestFakeButtonEvent(dpy, which, what==ButtonPress, CurrentTime);
    }

    /*****************************************************************************
    Bool GenerateMouseEvent(int which, int what){
      XEvent event;
      memset(&event, 0x00, sizeof(event));

      event.type 			= what;
      event.xbutton.button 		= which;
      // the event window is on the same screen as the root window
      event.xbutton.same_screen 	= True;

      //get pointer coordinates - assign to event
      XQueryPointer(dpy,
            RootWindow(dpy, DefaultScreen(dpy)),
            &event.xbutton.root,
            &event.xbutton.window,
            &event.xbutton.x_root,
            &event.xbutton.y_root,
            &event.xbutton.x,
            &event.xbutton.y,
            &event.xbutton.state	// state: button or key mask
              );
      event.xbutton.subwindow = event.xbutton.window; // child window

      while (event.xbutton.subwindow){
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(dpy,
              event.xbutton.window,
              &event.xbutton.root,
              &event.xbutton.subwindow,
              &event.xbutton.x_root,
              &event.xbutton.y_root,
              &event.xbutton.x,
              &event.xbutton.y,
              &event.xbutton.state
        );
      }
      int rc = XSendEvent(dpy,
                  PointerWindow, // he destination window is the window that contains the pointer.
                  True, 	     // propagate
                  0xfff,	     // event mask
                  &event
                );

      XFlush(dpy);

      return rc;
    }
    ******************************************************************************/

    /* New */
    void MoveMousePointer(int deviceid, int x, int y){
        XIWarpPointer(dpy, deviceid, None, window, 0, 0, 0, 0, x, y);
        XFlush(dpy);
    }

    /**
     * @param which: which key
     * @param what: what event is
     * 	KeyPress
     * 	KeyRelease
     */
    Bool GenerateKeyEvent(unsigned int which, int what){
        return XTestFakeKeyEvent(dpy, which, what==KeyPress, CurrentTime);
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
            thread{&OneConnection::start, a, ip} .detach();
        }
        close(listenfd);
    }

};
int main(int argc, char**argv){
    Server a;
    return a.start();
}
