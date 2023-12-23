#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

int SetupConnectionClient(struct sockaddr_in* address, char* ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&*address, 0, sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &address->sin_addr) < 0) {
        perror("bad address");
        exit(1);
    } return sockfd;
}

int main(int argc, char *argv[]) {
    char ip[255] = {'\0'};
    int port = -1;
    int buf_size = -1;

    if (argc < 3) {
        printf("Too few arguments \n");
        exit(1);
    }

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"ip", required_argument, 0, 0},
                                        {"port", required_argument, 0, 0},
                                        {"buf_size", required_argument, 0, 0},
                                        {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);

        if (c == -1)
        break;

        switch (c) {
            case 0: {
                switch (option_index) {
                    case 0:
                        memcpy(ip, optarg, strlen(optarg));
                        break;
                    case 1:
                        port = atoi(optarg);
                        if (port < 0 || port > 65532) {
                            printf("Error! Invalid port!");
                            return 1;
                        } break;
                    case 2:
                        buf_size = atoi(optarg);
                        if (buf_size < 32) {
                            printf("Error! buf_size must be more than 32!");
                            return 1;
                        } break;
                    default:
                        fprintf("Index %d is out of options\n", option_index);
                }
            } break;

            case '?':
                printf("Arguments error\n");
                break;
                
            default:
                fprintf("getopt returned character code 0%o?\n", c);
        }
    }

    if (!strlen(ip) || port == -1 || buf_size == -1) {
        fprintf(stderr, "Using: %s --ip \"127.0.0.1\" --port \"num\" --buf_size \"num\" \n", argv[0]);
        return 1;
    }

    struct sockaddr_in servaddr;
    int sockfd = SetupConnectionClient(&servaddr, ip, port);

    if (connect(sockfd, (struct sockaddr_in *)&servaddr, sizeof(struct sockaddr_in)) < 0) {
        perror("connect");
        exit(1);
    }
    write(1, "Input message to send\n", 22);

    int nread;
    char buf[buf_size];
    while ((nread = read(0, buf, buf_size)) > 0) {
        if (write(sockfd, buf, nread) < 0) {
            perror("write");
            exit(1);
        }
    } close(sockfd);
    return 0;
}