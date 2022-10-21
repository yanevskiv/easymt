/**
 * (c) Ivan Janevski
 */
package main

import (
    "fmt"
    "time"
)

// constants
const N = 5

// channels
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
        if id % 2 == 0 {
            <- forks[left]
            <- forks[right]
        } else {
            <- forks[right]
            <- forks[left]
        }
        // Eat
        fmt.Println("Philosopher", id, "is eating")
        time.Sleep(time.Microsecond)
        // Release forks
        forks[left] <- true
        forks[right] <- true
    }
    done <- true
}

func main() {
    // init channels
    done := make(chan bool, 1)
    for i := 0; i < N; i++ {
        forks[i] = make(chan bool, 1)
        forks[i] <- true
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
