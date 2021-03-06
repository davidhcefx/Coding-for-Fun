#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <vector>
#include <string>
#define ERRNOSTR          strerror(errno)
#define WRITE_FD(fd, msg) { write(fd, msg, strlen(msg)); }
using std::vector;
using std::string;
using std::max;

/*
 * TCP Port Multiplexer (Written by davidhcefx, 2020.11.25)
 * ---
 *   There are situations where you only have one public port, but you wanted to
 * connect to multiple TCP services. This program achieves this goal by first
 * displaying a menu, and then redirect (forward) successive traffics to the target service.
 */


struct Service {
    string name;
    int port;
};

/*********************************
 *   Add your service here!      *
 *********************************/
vector<struct Service> services {
    {"test", 11000},
    {"easy_pwn_challenge", 11001},
};


// Return -1 if read nothing
int read_int(int fd, char* buff, unsigned buf_size) {
    unsigned i, count;

    count = read(fd, buff, 1);
    if (count == 0) {
        // read nothing
        return -1;
    }
    for (i = 1; i < buf_size - 1; i++) {
        count = read(fd, buff + i, 1);  // not quite efficient though
        if (count <= 0 || !isdigit(buff[i])) {
            break;
        }
    }
    buff[i] = '\0';
    return atoi(buff);
}

// Return the s_sock Fd, or -1 if failed
int connectTCP(unsigned short port, const string& host="127.0.0.1") {
    struct sockaddr_in addr;
    int sockFd;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host.c_str());
    addr.sin_port = htons(port);
    printf("Try connecting %s:%d...\n", host.c_str(), port);

    if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Cannot create socket: %s\n", ERRNOSTR);
        return -1;
    }
    if (connect(sockFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Cannot connect to: %s\n", ERRNOSTR);
        return -1;
    }
    return sockFd;
}

// Return the m_sock Fd
int passiveTCP(unsigned short port, int qlen=128) {
    struct sockaddr_in addr;
    int sockFd;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Cannot create socket: %s\n", ERRNOSTR);
        exit(1);
    }
    if (bind(sockFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Cannot bind to port %d: %s\n", port, ERRNOSTR);
        exit(1);
    }
    if (listen(sockFd, qlen) < 0) {
        fprintf(stderr, "Cannot listen: %s\n", ERRNOSTR);
        exit(1);
    }
    printf("Listening on port %d...\n", port);
    return sockFd;
}

// Exit if either one closed
void forward_msg_to(int src_sock, int dst_sock) {
    char buff[1025];
    int count;

    count = read(src_sock, buff, 1024);
    if (count <= 0) exit(0);  // socket closed
    write(dst_sock, buff, count);  // SIGPIPE if failed
}

// Redirect messages between sockA and sockB
void redirect(int sockA, int sockB) {
    fd_set read_fds;
    int fd_num = max(sockA, sockB) + 1;

    FD_ZERO(&read_fds);
    while (1) {
        FD_SET(sockA, &read_fds);
        FD_SET(sockB, &read_fds);

        if (select(fd_num, &read_fds, 0, 0, (struct timeval*)0) < 0) {
            fprintf(stderr, "Error on select: %s\n", ERRNOSTR);
        }
        if (FD_ISSET(sockA, &read_fds)) {
            forward_msg_to(sockA, sockB);
        }
        if (FD_ISSET(sockB, &read_fds)) {
            forward_msg_to(sockB, sockA);
        }
    }
}

void handle_client(int s_sock) {
    int dst_sock;
    char buff[30];
    unsigned idx;

    for (unsigned i = 0; i < services.size(); i++) {
        snprintf(buff, sizeof(buff), "[%d] %s\n", i, services[i].name.c_str());
        WRITE_FD(s_sock, buff);
    }
    WRITE_FD(s_sock, "> Choose your service: ");
    idx = read_int(s_sock, buff, sizeof(buff));

    if (idx >= services.size()) {
        WRITE_FD(s_sock, "Wrong index.\n")
        return;
    }
    if ((dst_sock = connectTCP(services[idx].port)) < 0) {
        WRITE_FD(s_sock, "Cannot connect to that port. Please check server's log.\n");
        return;
    }
    redirect(s_sock, dst_sock);
}

void child_reaper(int sig) {
    int stat;
    while (wait3(&stat, WNOHANG, 0) > 0) {
        printf("Child exited: %d\n", WEXITSTATUS(stat));
    }
}

// SYNOPSIS: multiplex [port (default: 8080)]
int main(int argc, char* argv[]) {
    struct sockaddr_in  c_addr;  // client address
    socklen_t           c_addr_len = sizeof(c_addr);
    int m_sock, s_sock, port = 8080, pid;

    if (argc > 1) {
        port = atoi(argv[1]);
    }
    signal(SIGCHLD, child_reaper);
    m_sock = passiveTCP(port);

    while (1) {
        s_sock = accept(m_sock, (struct sockaddr*)&c_addr, &c_addr_len);
        printf("Connected: %s:%hu\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));

        if ((pid = fork()) < 0) {
            fprintf(stderr, "Fork failed: %s\n", ERRNOSTR);
        } else if (pid == 0) {
            close(m_sock);
            handle_client(s_sock);
            exit(0);
        }
        close(s_sock);
    }

    return 0;
}
