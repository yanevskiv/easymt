package main

import (
    "fmt"
    "time"
)

// constants
const (
    RED = "\x1b[31m"
    GREEN = "\x1b[32m"
    NOCOLOR = "\x1b[0m"
)
const (
    NR = 15
    NW = 5
)

// RW lock
const (
    RW_WRITER = 1
    RW_READER = 2
    RW_LOCK   = true
    RW_UNLOCK = false
) 
type RW struct {
    // counters
    wCount int
    rCount int 
    wDelay int
    rDelay int
    // sync
    mutex chan bool
    wQueue chan bool
    rQueue chan bool
}

func (rw *RW) Init() {
    rw.wCount = 0
    rw.rCount = 0
    rw.wDelay = 0
    rw.rDelay = 0
    rw.mutex = make(chan bool, 1)
    rw.wQueue = make(chan bool, NR)
    rw.rQueue = make(chan bool, NW)
    rw.mutex <- true
}

func (rw *RW) Lock(lock bool, lockType int) {
    // WAIT()
    <- rw.mutex
    if (lock)  {
        switch lockType {
            case RW_READER:
                if (rw.wCount > 0) {
                    rw.rDelay += 1
                    rw.mutex <- true
                    <- rw.rQueue
                }
                rw.rCount += 1;
            case RW_WRITER:
                if (rw.wCount > 0 || rw.rCount > 0) {
                    rw.wDelay += 1
                    rw.mutex <- true
                    <- rw.wQueue
                }
                rw.wCount += 1
        }
    } else {
        switch lockType {
            case RW_READER:
                rw.rCount -= 1;
            case RW_WRITER:
                rw.wCount -= 1;
        }
    }
    // SIGNAL()
    if (rw.rDelay > 0 && rw.wCount == 0) {
        rw.rDelay -= 1
        rw.rQueue <- true
    } else if (rw.wDelay >0 && rw.rCount ==0 && rw.wCount == 0)  {
        rw.wDelay -= 1
        rw.wQueue <- true
    } else {
        rw.mutex <- true
    }
}

// rw lock
var rw RW

// go routines
func reader(id int, done chan bool) {
    for {
        rw.Lock(RW_LOCK, RW_READER)
        fmt.Print(GREEN + "r" + NOCOLOR);
        rw.Lock(RW_UNLOCK, RW_READER)
        time.Sleep(time.Nanosecond);
    }
}

func writer(id int, done chan bool) {
    for {
        rw.Lock(RW_LOCK, RW_WRITER)
        fmt.Print(RED + "w" + NOCOLOR);
        rw.Lock(RW_UNLOCK, RW_WRITER)
        time.Sleep(time.Nanosecond);
    }
}

func main() {
    rw.Init()
    done := make(chan bool, NR + NW)
    // run
    for i := 0; i < NR; i++ {
        go reader(i, done)
    }
    for i := 0; i < NW; i++ {
        go writer(i, done)
    }
    // wait
    for i := 0; i < NR + NW; i++ {
        <- done 
    }

}
