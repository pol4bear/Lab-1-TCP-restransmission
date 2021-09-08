all: server, client, middlebox

server:
	test -d build || mkdir build
	cd build && go build ../tcp_server/tcp_server.go

client:
	test -d build || mkdir build
	cd build && go build ../tcp_client/tcp_client.go

middlebox: middlebox/retransmit.c
	test -d build || mkdir build
	$(CC) -O3 -o build/middlebox $^ -lpcap

clean:
	rm -rf bulid
