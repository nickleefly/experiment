package main

import "fmt"

func main() {
	c := make(chan int, 2)
	fmt.Println("cap", cap(c))
	fmt.Println("len", len(c))
	c <- 1
	c <- 2
	fmt.Println("len", len(c))
	fmt.Println(<-c)
	fmt.Println(<-c)
	fmt.Println("len", len(c))
}
