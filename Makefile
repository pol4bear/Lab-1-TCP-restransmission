all: server, client

server:
	test -d build || mkdir build
	cd build && go build ../tcp_server/tcp_server.go

client: tcp_client/tcp_client.go
	test -d build || mkdir build
	cd build && go build ../tcp_client/tcp_client.go

clean:
	rm -rf bulid
