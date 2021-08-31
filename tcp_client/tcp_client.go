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
	conn, err := net.Dial("tcp", address)
	if err != nil {
		log.Fatalf("Failed to connect %s. err = %v", address, err)
	}
	fmt.Printf("Connected to %s.\n", conn.RemoteAddr())
	fmt.Println("Wating 1 seconds...")
	time.Sleep(1 * time.Second)
	buf := make([]byte, 1)
	_, err = conn.Read(buf)
	if err != nil {
		log.Fatalf("Failed to read from socket. err = %v", err)
	}
	fmt.Printf("New payload from server. payload = \"%s\"\n", string(buf))
}
