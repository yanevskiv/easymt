package main

import (
    "fmt"
    "time"
)

// constants
const N = 5

// channels
var ticket chan bool
var forks [N]chan bool

// routines
func phil(id int, done chan bool) {
    left := id
    right := (id + 1) % N
    for {
        // Think
        fmt.Println("Philosopher", id, "is thinking")
        time.Sleep(time.Microsecond)
        // Acquire forks
        <- ticket
        <- forks[left]
        <- forks[right]
        // Eat
        fmt.Println("Philosopher", id, "is eating")
        time.Sleep(time.Microsecond)
        // Release forks
        forks[left] <- true
        forks[right] <- true
        ticket <- true
    }
    done <- true
}

func main() {
    // init channels
    done := make(chan bool, 1)
    ticket = make(chan bool, N - 1)
    for i := 0; i < N; i++ {
        forks[i] = make(chan bool, 1)
        forks[i] <- true
        if i < N - 1 {
            ticket <- true
        }
    }

    // run
    for i := 0; i < N; i++ {
        go phil(i, done)
    }

    // join
    for i := 0; i < N; i++ {
        <- done
    }
}
