#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define SERVER_PORT_H 12345
#define SERVER_IP_S "127.0.0.1"
#define CLIENT_PORT_H 54321
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    struct udphdr *udph;
    char *data;

    // Создание RAW сокета
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Буфер для пакета
    char packet[BUFFER_SIZE];
    memset(packet, 0, BUFFER_SIZE);

    // Строка
    const char text[] = "hello";

    // Заполнение UDP заголовка
    udph = (struct udphdr *) packet;
    udph->source = htons(CLIENT_PORT_H);
    udph->dest = htons(SERVER_PORT_H);
    udph->len = htons(sizeof(struct udphdr) + strlen(text));
    udph->check = 0; // Контрольная сумма не рассчитывается

    // Заполнение данных
    data = packet + sizeof(struct udphdr);
    strcpy(data, text);

    // Установка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT_H);
    inet_pton(AF_INET, SERVER_IP_S, &(server_addr.sin_addr));

    // Отправка пакета
    if (sendto(sockfd, packet, sizeof(struct udphdr) + strlen(text), 0,
               (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("sendto()");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Packet sent to %s:%d\n", inet_ntoa(server_addr.sin_addr),
           ntohs(server_addr.sin_port));

    // Ожидание ответа от сервера
    while (1) {
        memset(buffer, 0, BUFFER_SIZE); // Обнуление буфера перед приемом данных
        socklen_t len = sizeof(server_addr);
        int recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                                (struct sockaddr *) &server_addr, &len);
        if (recv_len < 0) {
            perror("recvfrom()");
            continue;
        }

        // Пропуск IP заголовка
        struct udphdr *recv_udph = (struct udphdr *) (buffer + sizeof(struct iphdr));

        // Вывод полученных пакетов
        printf("Received from source port: %d dest port: %d\n",
               ntohs(recv_udph->source), ntohs(recv_udph->dest));

        // Проверка порта источника и назначения на соответствие ожидаемым
        if (ntohs(recv_udph->source) == SERVER_PORT_H && ntohs(recv_udph->dest) == CLIENT_PORT_H) {
            // Получаем указатель на данные, пропуская IP и UDP заголовок
            char *recv_data = buffer + sizeof(struct iphdr) + sizeof(struct udphdr);
            // Вычисление длины данных
            int data_len = recv_len - (sizeof(struct iphdr) + sizeof(struct udphdr));
            // Установка нулевого символа в конце строки
            recv_data[data_len] = '\0';
            // Вывод полученных данных
            printf("Received from server: %s\n", recv_data);
            break;
        }
    }

    close(sockfd);
    return 0;
}
