package main

import (
	"fmt"
	"log"
	"net"
	"os"
	"time"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Printf("Usage: %s address\n", os.Args[0])
		os.Exit(1)
	}
	address := os.Args[1]
	listen, err := net.Listen("tcp", address)
	if err != nil {
		log.Fatalf("Failed to bind address on %s. err = %v", address, err)
	}
	fmt.Printf("Server started at %s\n", address)
	conn, err := listen.Accept()
	if err != nil {
		log.Fatalf("Failed to accept from client. err = %v", err)
	}
	fmt.Printf("New connection from %s.\n", conn.RemoteAddr())
	conn.Write([]byte("A"))
	fmt.Printf("\"A\" sent to %s.\n", conn.RemoteAddr())
	fmt.Println("Wating 0.5 seconds")
	time.Sleep(time.Millisecond * 500)
	conn.Close()
	fmt.Printf("Connection closed.\n")
}
