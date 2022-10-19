package main

import ( 
    "fmt" 
    "time" 
)

const REV = "\x1b[7m"
const RED = "\x1b[31m"
const GREEN = "\x1b[32m"
const NOCOLOR = "\x1b[0m"

// channels
var mutex chan bool
var aFinished, bFinished chan bool

// routines
func a() {
    for {
        // lock
        <- mutex
        // critical section
        fmt.Print(GREEN + "(---" + NOCOLOR)
        time.Sleep(2 * time.Millisecond);
        fmt.Print(GREEN + "---)" + NOCOLOR)
        // unlock
        mutex <- true
        // non-critical section
        time.Sleep(1 * time.Millisecond);
    }
    aFinished <- true
}

func b() {
    for {
        // lock
        <- mutex
        // critical section
        fmt.Print(REV + RED + "(---" + NOCOLOR)
        time.Sleep(2 * time.Millisecond);
        fmt.Print(REV + RED + "---)" + NOCOLOR)
        // unlock
        mutex <-  true
        // non critical section
        time.Sleep(1 * time.Millisecond);
    }
    bFinished <- true
}

// main
func main() {
    // initialize channels
    mutex = make(chan bool, 1)
    aFinished = make(chan bool)
    bFinished = make(chan bool)
    mutex <- true

    // start go routines
    go a()
    go b()

    // wait for go routines
    <- aFinished
    <- bFinished
}

