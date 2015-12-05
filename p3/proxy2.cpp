#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <ev.h>

#include <errno.h>
#include <string.h>

#define MAX_EVENTS 32

struct my_io {
    struct ev_io io;
    int fdin;
    int fdout;
};

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

std::map <int,std::set <char *>> addrs;
int port;
std::set<char *>::const_iterator it;
std::map <int,std::set <char *>>::const_iterator addr_it;
 
char data[1000] = {};
int RecvSize;
    
static void read_cb(struct ev_loop *, struct ev_io *, int);

static void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    struct my_io *w = (struct my_io *)watcher;
    ev_io_stop(loop, watcher);
    if (watcher->fd == w->fdin) {
        send(w->fdout, data, RecvSize, MSG_NOSIGNAL);
    } else {
        send(w->fdin, data, RecvSize, MSG_NOSIGNAL);
    }
    ev_io_init((ev_io*)watcher, read_cb, watcher->fd, EV_READ);
    ev_io_start(loop, watcher);
    // потом в врайте отправляем
}

static void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    ev_io_stop(loop, watcher);
    RecvSize = recv(watcher->fd, data, 1000, MSG_NOSIGNAL);
    std::cout << data;
    if(RecvSize <= 0)
    {
        free(watcher);
    }
    ev_io_init((ev_io*)watcher, write_cb, watcher->fd, EV_WRITE);
    ev_io_start(loop, watcher);
    //ставим ватчер на райт нужен буфер, stl
    // потом в врайте отправляем
}

static void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    char addr[100] = {};
    int pp;
    char *getpp = strchr(*(it), ':');
    getpp++;
    pp = atoi(getpp++);
    getpp = strdup(*(it));
    getpp[strchr(getpp, ':') - &getpp[0]] = 0;
    strcpy(addr,getpp);
    if (it == addr_it->second.end()) {
        it = addr_it->second.begin();
    } else {
        std::advance(it, 1);
    }
    printf("%d\n", pp);
    printf("%s\n", addr);
    
    struct my_io *watcher_client = (struct my_io*)malloc(sizeof(struct my_io));
    int SocketIn = accept(watcher->fd, 0, 0);
    if(SocketIn < 0) {
        std::cout << "Error accept";
    }
    set_nonblock(SocketIn);
    watcher_client->fdin = SocketIn;
    
    int SocketOut = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(pp);
    inet_pton(AF_INET, addr, &(SockAddr.sin_addr));
    if (connect(SocketOut, (sockaddr *) &SockAddr, sizeof(SockAddr)) < 0) {
         std::cout << "Error connecting\n";
    }
    set_nonblock(SocketOut);
    watcher_client->fdout = SocketOut;
    
    ev_io_init((ev_io*)watcher_client, read_cb, SocketOut, EV_READ);
    ev_io_init((ev_io*)watcher_client, read_cb, SocketIn, EV_READ);
    
    ev_io_start(loop, (ev_io*)watcher_client);
}


int main(int argc, const char * argv[])
{
    struct ev_loop *loop = ev_default_loop(0);
    struct ev_io watcher;
    FILE *f = fopen("config.txt", "r");
    int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in SockAddr;
    char str[1000];
    char tmp[20];
    int index = 0, p = 0;
    std::set<char *> rcvs;
    
    while (fgets(str,1000, f)) {
        p = 0;
        while(str[index] != ',') {
            tmp[index - p] = str[index++];
        }
        tmp[index - p] = 0;
        p = ++index;
        port = atoi(tmp);
        p--;
        while(str[index] != '\0') {
            p++;
            while(str[index] != ',' && str[index] != '\0') {
                tmp[index - p] = str[index++];
            }
            tmp[index - p] = 0;
            char *elem = strdup(tmp);
            rcvs.insert(elem);
            p = ++index;
        }
        addrs.insert(std::pair<int, std::set<char *>>(port, rcvs));
    }
    addr_it = addrs.begin();
    std::advance(addr_it, addrs.size() * rand()/(RAND_MAX + 1.0));
    printf("%d\n", addr_it->first);
    port = addr_it->first;
    it = addr_it->second.begin();
    //inet_pton
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(port);
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
    ev_io_init(&watcher, accept_cb, MasterSocket, EV_READ);
    ev_io_start(loop, &watcher);
    ev_loop(loop, 0);
    return 0;
}
