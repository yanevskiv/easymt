package main

import (
    "fmt"
    "time"
)

// constants
const N = 5

// channels
var hungry [N]bool
var avail [N]bool
var privc [N]chan bool
var mutex chan bool

// routines
func Left(id int) int {
    if id == 0 {
        return N - 1
    }
    return id - 1
}

func Right(id int) int {
    return (id + 1) % N
}

func phil(id int, done chan bool) {
    left := Left(id)
    right := Right(id)
    for {
        // Think
        fmt.Println("Philosopher", id, "is thinking")
        time.Sleep(time.Microsecond)
        // Acquire forks
        <- mutex
        if ! avail[left] || ! avail[right] {
            hungry[id] = true
            mutex <- true
            <- privc[id]
        }
        hungry[id] = false
        avail[left] = false
        avail[right] = false
        mutex <- true
        // Eat
        fmt.Println("Philosopher", id, "is eating")
        time.Sleep(time.Microsecond)
        // Release forks
        <- mutex
        avail[left] = true
        avail[right] = true
        if (hungry[left] && avail[Left(left)] && avail[Right(left)]) {
            privc[left] <- true
        } else if (hungry[right] && avail[Left(right)] && avail[Right(right)]) {
            privc[right] <- true
        } else {
            mutex <- true
        }
    }
    done <- true
}

func main() {
    // init channels
    done := make(chan bool, 1)
    mutex = make(chan bool, 1)
    mutex <- true
    for i := 0; i < N; i++ {
        privc[i] = make(chan bool, 1)
        hungry[i] = false
        avail[i] = true
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
