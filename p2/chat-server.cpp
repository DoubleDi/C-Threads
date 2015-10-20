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


int main(int argc, const char * argv[])
{
    int index = 0;
    char str[1024] = {};
    int SavedSockets[100] = {};
    int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in SockAddr; //порт - его слушает сокет

	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(3100);
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int val = 1;

    setsockopt(MasterSocket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    if (bind(MasterSocket, (struct sockaddr *)&SockAddr, sizeof(SockAddr)) < 0) {
        puts("Error bind");
        exit(1);
    }
    set_nonblock(MasterSocket);
    if (listen(MasterSocket, SOMAXCONN) < 0) {
        puts("Error listen");
        exit(2);
    }
    struct epoll_event Event;
	Event.data.fd = MasterSocket;
	Event.events = EPOLLIN;

	struct epoll_event * Events;
	Events = (struct epoll_event *) calloc(MAX_EVENTS, sizeof(struct epoll_event));

	int EPoll = epoll_create1(0);
	epoll_ctl(EPoll, EPOLL_CTL_ADD, MasterSocket, &Event);

    int SlaveSocket;
	while(true)
	{
	    struct sockaddr_storage addr;
        socklen_t socklen;
	    int N = epoll_wait(EPoll, Events, MAX_EVENTS, -1);
        for(unsigned int i = 0; i < N; i++) {
            if((Events[i].events & EPOLLERR)||(Events[i].events & EPOLLHUP)) {
				puts("connection terminated");
				int a = 0;
				while(SavedSockets[a] != Events[i].data.fd) a++;
				for(int k = a; k < index - 1; k++) SavedSockets[k] = SavedSockets[k + 1];
				index--;
				shutdown(Events[i].data.fd, SHUT_RDWR);
				close(Events[i].data.fd);
            } else if(Events[i].data.fd == MasterSocket) {
				SlaveSocket = accept(MasterSocket, 0, 0);
				set_nonblock(SlaveSocket);
				struct epoll_event Event;
				Event.data.fd = SlaveSocket;
				Event.events = EPOLLIN | EPOLLET;
				epoll_ctl(EPoll, EPOLL_CTL_ADD, SlaveSocket, &Event);
				std::cout << "accepted connection" << "\n";
				send(Event.data.fd, "Welcome", 1024, MSG_NOSIGNAL);
				SavedSockets[index++] = SlaveSocket;
			} else {
                int x = recv(Events[i].data.fd, str, 1024, MSG_NOSIGNAL);
				str[x] = 0;
				puts(str);
				for(auto k = 0; k < index; k++) {
                    send(SavedSockets[k], str, 1024, MSG_NOSIGNAL);
				}
			}
        }
    }
	shutdown(SlaveSocket, SHUT_RDWR);
	close(SlaveSocket);
    close(MasterSocket);
    return 0;
}
