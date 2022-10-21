/**
 * (c) Ivan Janevski
 */
package main

import ( "fmt" )

const RED = "\x1b[31m"
const GREEN = "\x1b[32m"
const NOCOLOR = "\x1b[0m"

// channels
var aChannel, bChannel chan string
var aFinished, bFinished chan bool

// routines
func a() {
    for {
        msg := <- aChannel
        fmt.Print(GREEN + msg + NOCOLOR)
        bChannel <- "b"
    }
    aFinished <- true
}

func b() {
    for {
        msg := <- bChannel
        fmt.Print(RED + msg + NOCOLOR)
        aChannel <- "a"
    }
    bFinished <- true
}

// main
func main() {
    // initialize channels
    aChannel = make(chan string, 1)
    bChannel = make(chan string, 1)
    aFinished = make(chan bool)
    bFinished = make(chan bool)
    aChannel <- "a"

    // start go routines
    go a()
    go b()

    // wait for go routines
    <- aFinished
    <- bFinished
}

