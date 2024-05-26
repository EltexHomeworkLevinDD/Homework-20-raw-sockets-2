CFLAGS := -Wall -Wextra -g

.PHONY: all clean

all: udp_server udp_client

udp_server: udp_server.c
	gcc $(CFLAGS) $^ -o $@

udp_client: udp_client.c
	gcc $(CFLAGS) $^ -o $@

clean:
	rm -rf udp_server
	rm -rf udp_client