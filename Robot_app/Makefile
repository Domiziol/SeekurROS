

CPPFLAGS= -Wall -g -I/usr/local/Aria/include -I/usr/local/Arnl/include -I/usr/local/Aria/ArNetworking/include
LDFLAGS= -L/usr/local/Aria/lib -lAria -lpthread -ldl -lrt -L/usr/local/Arnl/lib -lArnl

%: %.cpp
	g++ -std=c++11 $< -o $@ ${CPPFLAGS} ${LDFLAGS} -D_REENTRANT


