login: login.o popen_noshell.o
	cc login.o popen_noshell.o -o login

login.o: login.c
	cc -c login.c

popen_noshell.o: popen_noshell.c popen_noshell.h
	cc -c popen_noshell.c

clean:
	rm popen_noshell.o login.o
