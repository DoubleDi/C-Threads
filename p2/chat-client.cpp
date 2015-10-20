#include <iostream>
#include <algorithm>
#include <set>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <sys/epoll.h>

#include <errno.h>
#include <string.h>

#define POLL_SIZE 32
#define MAX_EVENTS 32

int set_nonblock(int fd)
{
	int flags;
#if defined(O_NONBLOCK)
	if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	flags = 1;
	return ioctl(fd, FIOBIO, &flags);
#endif
}


using namespace std;


struct epoll_event * Events = (struct epoll_event *) calloc(MAX_EVENTS, sizeof(struct epoll_event));
int EPoll = epoll_create1(0);

int main(int argc, char **argv)
{
    char str1[1024] = {};
    char str2[1024] = {};
	int ClientSocketIn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(3100);
	SockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(ClientSocketIn, (sockaddr *) &SockAddr, sizeof(SockAddr)) < 0) {
         cout << "Error connecting\n";
    }
    set_nonblock(ClientSocketIn);
    struct pollfd Set[POLL_SIZE];
	Set[0].fd = ClientSocketIn;
	Set[1].fd = fileno(stdin);
	Set[0].events = POLLIN | POLLOUT;
	Set[1].events = POLLIN;
    while(true)
	{
		if (poll(Set, 2, -1) < 0) {
		    cout << "Connection terminated\n";
		    break;
		}
        if (Set[0].revents & POLLIN) {
            if (recv(Set[0].fd, str1, 1024, MSG_NOSIGNAL) > 0) {
                cout << str1 << "\n";
            }
        }
        if (Set[1].revents & POLLIN) {
            fgets(str1, 1024, stdin);
            str[strlen(str1) - 1] = 0;
            send(Set[0].fd, str1, 1024, MSG_NOSIGNAL);
        }
    }
    shutdown(ClientSocketIn, SHUT_RDWR);
	close(ClientSocketIn);
	return 0;
}
