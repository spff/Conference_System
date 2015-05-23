  #include <stdio.h>
  #include <string.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>

  #define MAX_LEN 256
  #define BUFF_LEN 4096

  int main()
  {
      char username[MAX_LEN], ip[MAX_LEN], password[MAX_LEN];
      char cmd[MAX_LEN * 2 + 10], buff[BUFF_LEN];
      FILE *pin;
      int script_fd;
      int iReadLen; 
      
      // get username and ip
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
      {
	perror("write");
	exit(1);
      }
      if (close(script_fd))
      {
	perror("close");
	exit(1);
      }
      
      // execute script
      pin = popen("./login.sh", "w");
      if(!pin)
      {
	perror("popen");
	exit(1);
      }
      
      while(1)
      {
	// read command from standard input
	fgets(buff, BUFF_LEN, stdin);
	
	// write to shell input
	fputs(buff, pin);
	fflush(pin);
      }
      pclose(pin);
      printf("Bye\n");
      return 0;
  }
