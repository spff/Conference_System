#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "popen_noshell.h"

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE);} while (0)
                               
#define MAX_LEN 256
#define BUFF_LEN 1024

int main(int argc, char **argv)
{
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
