login: login.o popen_noshell.o
	g++ login.o popen_noshell.o -o login

login.o: login.cpp
	g++ -c login.cpp -std=c++11

popen_noshell.o: popen_noshell.c popen_noshell.h
	cc -c popen_noshell.c

clean:
	rm -f popen_noshell.o login.o login
