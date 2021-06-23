#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>

#define PORT htons(22212)
#define BUFSIZ 16
#define FILE_TO_SEND "kasa.png"

void send_file(char *file_to_send, int s_socket, int c_socket){
    ssize_t len;
    int fd;
    struct stat file_stat;
    int fsize;
    off_t offset;
    int remain_data;
    int sent_bytes = 0;


    fd = open(file_to_send, O_RDONLY);

    if (fd == -1) {
            printf("Error opening file\n");
            exit(1);
    }

    if (fstat(fd, &file_stat) < 0) {
            printf("Error fstat\n");
            exit(1);
    }

    fsize = (int)file_stat.st_size;
    printf("File Size:\n%d bytes\n", fsize);

    fsize = htonl(fsize);

    len = send(c_socket, &fsize, sizeof(fsize), 0);
    if (len < 0) {
          printf("Error on sending file size\n");
          exit(1);
    }

    printf("Server sent %d bytes for the size\n", ntohl(fsize));

    offset = 0;
    remain_data = file_stat.st_size;
    while (((sent_bytes = sendfile(c_socket, fd, &offset, 1024)) > 0) && (remain_data > 0)) {
            printf("1. Server sent %d bytes from file's data, offset is now : %ld and remaining data = %d\n", sent_bytes, offset, remain_data);
            remain_data -= sent_bytes;
            printf("2. Server sent %d bytes from file's data, offset is now : %ld and remaining data = %d\n", sent_bytes, offset, remain_data);
    }
    fprintf(stdout, "Last sent:%d\nRemain:%d\n", sent_bytes, remain_data);


}

int atoi_(char buffer[]){
    int a = 0;
    int i = 0;
    while(buffer[i] != 0){
        a *= 10;
        a += buffer[i] - 48;
        i++;
    }

    return a;
}

short deal_cards(bool busy_cards[52]){
    short number;
    srand(time(NULL));
    while(1){
        number = rand() % 52;
        if(busy_cards[number]){
            busy_cards[number] = false;
            return number;
        }
    }
}

void game_start(int soc_client){
    int money, money_n, server_points, server_points_minus, server_points_n, client_points,client_points_n, bet;
    int size_rec_int, rec_bytes;
    short card, hidden_card;
    char size_rec, signal, hit_stand;
    bool flag_game_exists, end_part;
    char buffer[BUFSIZ];
    bool busy_cards[52];
    short  cards[52] = {2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,8,8,8,8,
                        9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11};

    flag_game_exists = true;

    //na razie
    money = 2500;

    send(soc_client, "Polaczono ze stolikiem.\n", strlen("Polaczono ze stolikiem.\n"), 0);

    //starting a particular game
    while(flag_game_exists){
        end_part = false;
        server_points = 0;
        client_points = 0;

        for(int i=0; i<52; i++)
            busy_cards[i] = true;

        send(soc_client, "Aktualny stan konta: \n", strlen("Aktualny stan konta: \n"), 0);
        money_n = htonl(money);
        send(soc_client, &money_n, sizeof(money_n), 0);
        send(soc_client, "Podaj twoja stawke: \n", strlen("Podaj twoja stawke: \n"), 0);

        //receiving bet amount
        memset(buffer, 0, BUFSIZ);


        recv(soc_client, &size_rec, 1, 0);
        size_rec_int = size_rec - 48;

        rec_bytes = 0;

        while(rec_bytes<size_rec_int)
            rec_bytes += recv(soc_client, &buffer[rec_bytes], BUFSIZ, 0);

        bet = atoi_(buffer);
        printf("%d\n", bet);

        //too small or too high bet
        if((bet < 1) || (bet > money)){
            send(soc_client, "Podales zla kwote!\n", strlen("Podales zla kwote!\n"), 0);
            //return 1;
            signal = 1;
            send(soc_client, &signal, 1, 0);
            continue;
        }
        send(soc_client, "Zaczynamy! Czas na rozdanie!\n", strlen("Zaczynamy! Czas na rozdanie!\n"), 0);
        signal = 0;
        send(soc_client, &signal, 1, 0);

        //2 cards for dealer
        card = deal_cards(busy_cards);
        server_points += cards[card];

        server_points_minus = htonl(server_points);

        send(soc_client, "Dealer otrzymuje karte: \n", strlen("Dealer otrzymuje karte: \n"), 0);
        card = htons(card);
        send(soc_client, &card, sizeof(card), 0);

        card = deal_cards(busy_cards);
        hidden_card = card;
        server_points += cards[card];
        send(soc_client, "Dealer otrzymuje druga karte: \n", strlen("Dealer otrzymuje druga karte: \n"), 0);
        send(soc_client, "Karta zakryta \n", strlen("Karta zakryta \n"), 0);


        // send dealer points
        send(soc_client, "Punty Dealeara: \n", strlen("Punty Dealeara: \n"), 0);
        send(soc_client, &server_points_minus, sizeof(server_points_minus), 0);

        //send client cards
        for(int i=0; i<2; i++){
            card = deal_cards(busy_cards);
            client_points += cards[card];
            send(soc_client, "Otrzymujesz karte: \n", strlen("Otrzymujesz karte: \n"), 0);
            card = htons(card);
            send(soc_client, &card, sizeof(card), 0);
        }

        //send client points
        send(soc_client, "Twoje punkty: \n", strlen("Twoje punkty: \n"), 0);
        client_points_n = htonl(client_points);
        send(soc_client, &client_points_n, sizeof(client_points_n), 0);

        if(client_points > 20){
            signal = 1;
            send(soc_client, &signal, 1, 0);
            send(soc_client, "BLACK JACK!!!\n", strlen("BLACK JACK!!!\n"), 0);
            money = money + 1.5 * bet;
            continue;
        }
        signal = 2;
        send(soc_client, &signal, 1, 0);

        send(soc_client, "HIT (1) / STAND (0)\n", strlen("HIT (1) / STAND (0)\n"), 0);

        //receiving hit or stand
        recv(soc_client, &hit_stand, 1, 0);

        while(hit_stand && client_points < 21 && !end_part){
            card = deal_cards(busy_cards);
            client_points += cards[card];
            send(soc_client, "Otrzymujesz karte: \n", strlen("Otrzymujesz karte: \n"), 0);
            card = htons(card);
            send(soc_client, &card, sizeof(card), 0);
            send(soc_client, "Twoje punkty: \n", strlen("Twoje punkty: \n"), 0);
            client_points_n = htonl(client_points);
            send(soc_client, &client_points_n, sizeof(client_points_n), 0);
            if(client_points > 21){
                money -= bet;
                send(soc_client, "Przegrales! Masz wiecej niz 21 pkt!\n", strlen("Przegrales! Masz wiecej niz 21 pkt!\n"), 0);
                signal = 1;
                send(soc_client, &signal, 1, 0);
                end_part = true;
            }
            else if(client_points == 21){
                if(server_points != 21){
                    money += bet;
                    send(soc_client, "Wygrales! Masz 21 pkt!\n", strlen("Wygrales! Masz 21 pkt!\n"), 0);
                }
                else{
                    send(soc_client, "Remis! Masz 21 pkt, ale Dealer rowniez!\n", strlen("Remis! Masz 21 pkt, ale Dealer rowniez!\n"), 0);
                }
                signal = 1;
                send(soc_client, &signal, 1, 0);
                end_part = true;
            }
            else{
                send(soc_client, "HIT (1) / STAND (0)\n", strlen("HIT (1) / STAND (0)\n"), 0);
                signal = 0;
                send(soc_client, &signal, 1, 0);
                recv(soc_client, &hit_stand, 1, 0);
            }
        }

        //taking cards till the 17
        if(!end_part){
            send(soc_client, "Twoje punkty to \n", strlen("Twoje punkty to \n"), 0);
            client_points_n = htonl(client_points);
            send(soc_client, &client_points_n, sizeof(client_points_n), 0);

            send(soc_client, "Dealer odslania karte: \n", strlen("Dealer odslania karte: \n"), 0);
            hidden_card = htons(hidden_card);
            send(soc_client, &hidden_card, sizeof(hidden_card), 0);

            send(soc_client, "Punkty dealera \n", strlen("Punkty dealera \n"), 0);
            server_points_n = htonl(server_points);
            send(soc_client, &server_points_n, sizeof(server_points_n), 0);


            while(server_points < 17){
                card = deal_cards(busy_cards);
                server_points += cards[card];
                send(soc_client, "Dealer otrzymuje karte: \n", strlen("Dealer otrzymuje karte: \n"), 0);
                card = ntohs(card);
                send(soc_client, &card, sizeof(card), 0);
                server_points_n = htonl(server_points);
                send(soc_client, "Punkty dealera \n", strlen("Punkty dealera \n"), 0);
                server_points_n = htonl(server_points);
                send(soc_client, &server_points_n, sizeof(server_points_n), 0);

            }
            if(client_points > server_points || server_points > 21){
                money += bet;
                send(soc_client, "Wygrales \n", strlen("Wygrales \n"), 0);
            }
            else if(client_points == server_points)
                send(soc_client, "Remis \n", strlen("Remis \n"), 0);
            else{
                money -= bet;
                send(soc_client, "Przegrales \n", strlen("Przegrales \n"), 0);
            }
        }
        send(soc_client, "Twoj stan konta: \n", strlen("Twoj stan konta: \n"), 0);
        money_n = htonl(money);
        send(soc_client, &money_n, sizeof(money_n), 0);
        if(money > 0){
            send(soc_client, "Czy chcesz kontynuowac gre? Tak(1) Nie(0) \n", strlen("Czy chcesz kontynuowac gre? Tak(1) Nie(0) \n"), 0);
            recv(soc_client, &signal, 1, 0);
            if(signal == 0){
                flag_game_exists = false;
            }

        }
        else{
            flag_game_exists = false;
        }

   }


}

int main()
{
    char signal;
    int soc_listen, soc_client;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    soc_listen = socket(PF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = PORT;
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

    printf("Server is working\n");

    if(bind(soc_listen, (struct sockaddr*) &addr, addr_len) < 0){
        printf("bind returned an error\n");
        return 1;
    }

    if(listen(soc_listen, 30) < 0){
        printf("listen returned an error\n");
        return 1;
    }

    while(1){
        soc_client = accept(soc_listen, (struct sockaddr*) &addr, &addr_len);
        if(soc_client < 0){
            printf("accept returned an error\n");
            continue;
        }

        printf("Connected with: %s:%u\n",
                inet_ntoa(addr.sin_addr),
                ntohs(addr.sin_port)
                );

         /* proces potomny */
        if (fork() == 0){
            printf("Starting a new game \n");
            send(soc_client, "Polaczono z serwerem\n", strlen("Polaczono z serwerem\n"), 0);

            while(1){
                recv(soc_client, &signal, 1, 0);
                printf("%d\n", signal);
                switch(signal){
                    case 1:
                        game_start(soc_client);
                        break;
                    case 2:
                        break;
                    case 3:
                        send_file(FILE_TO_SEND, soc_listen, soc_client);
                        break;
                    default:
                        printf("One client have disconnected\n");
                        close(soc_client);
                        exit(0);
                        exit(0);
                        break;
                }
            }

        }

    }


    return 0;
}
