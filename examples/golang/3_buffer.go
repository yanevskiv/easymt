/**
 * (c) Ivan Janevski
 */
package main

import ( 
    "fmt" 
    "math/rand" 
)

const REV = "\x1b[7m"
const RED = "\x1b[31m"
const GREEN = "\x1b[32m"
const NOCOLOR = "\x1b[0m"

// constants 
const B = 10 // buffer size

// channels
var pFinished, cFinished chan bool
var buffer chan int

// routines
func producer() {
    for {
        // Produce data...
        data := rand.Intn(1000);

        // Put data into buffer
        buffer <- data
    }
    pFinished <- true
}

func consumer() {
    for {
        // Get data from buffer
        data := <- buffer

        // Consume data...
        fmt.Print(data, " ");
    }
    cFinished <- true
}

// main
func main() {
    // initialize channels
    buffer = make(chan int, B) // buffered channel
    pFinished = make(chan bool)
    cFinished = make(chan bool)

    // start go routines
    go producer()
    go consumer()

    // wait for go routines
    <- pFinished
    <- cFinished
}

