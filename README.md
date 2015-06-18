# Conference_System
for network programming course
the server will be concurrent

#WorkFlow
```
1.client:  
    get username by whoami and pass to server

2.server:  
	check record if permitted before  
   if firsttimelogin  
     ask for permission  
      if yes  
        store the ip/user pair data so don't need to ask for permission everytime  
      endif
    endif
    if permitted  
      popout a terminal and auto prompt "ssh -X username@ip"  
      generate a virtual mouse/keyboard pair  
      make it multi point by xinput setting  
      tell client success  
    endif  

3.client  
  wait for response  
  if success  
    loop:send mouse and keyboard signal to server  

4.server  
  loop:get signal  
```
#Requirements

EXPECT SHELL
SSH
an X-Server
