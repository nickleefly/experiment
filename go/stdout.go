package main

import (
    "fmt"
    "time"
)

func main() {
    for i := 0; i < 1000; i++ {
        fmt.Printf("%d ", i )
        time.Sleep(time.Second)
    }
}
