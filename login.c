#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_LEN 256
#define BUFF_LEN 4096

int main()
{
    char username[MAX_LEN], ip[MAX_LEN];
    char cmd[MAX_LEN * 2 + 10], buff[BUFF_LEN];
    FILE *pin, *pout;
    int iReadLen; 
    
    // get username and ip
    printf("IP: ");
    fgets(ip, MAX_LEN, stdin);
    ip[strlen(ip) - 1] = '\0';
    printf("Username: ");
    fgets(username, MAX_LEN, stdin);
    username[strlen(username) - 1] = '\0';
    
    // command
    sprintf(cmd, "ssh -Y %s@%s", username, ip);
    
    // redirection
    pin = popen(cmd, "r");
    if(!pin)
    {
      perror("popen");
      exit(1);
    }
    
    
    while(1)
    {
      // terminate ssh connection
      if (feof(pin)) break;

      fgets(buff, BUFF_LEN, pin);
      fputs(buff, stdout);
    }
    pclose(pin);
    printf("Bye\n");
    return 0;
}
