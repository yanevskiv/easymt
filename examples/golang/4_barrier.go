/**
 * (c) Ivan Janevski
 */
package main

import ( 
    "fmt" 
)

// constants 
const REV = "\x1b[7m"
const RED = "\x1b[31m"
const GREEN = "\x1b[32m"
const NOCOLOR = "\x1b[0m"
const N = 5


// barrier
type Barrier struct {
    needed int
    count int
    mutex chan bool
    barrier chan bool
}

func newBarrier(needed int) *Barrier{
    var bar Barrier;
    bar.needed = needed
    bar.count = 0
    bar.mutex = make(chan bool, 1)
    bar.barrier = make(chan bool, bar.needed)
    bar.mutex <- true
    return &bar
}

func (bar *Barrier) bar_wait() {
    <- bar.mutex
    bar.count += 1 
    if bar.count == bar.needed {
        for i := 0; i < bar.count; i++ {
            bar.barrier <- true
        }
        bar.count = 0
    }
    bar.mutex <- true
    <- bar.barrier
}


// channels and barriers
var aFinished chan bool
var bar1, bar2 *Barrier

// routines
func a(id int) {
    for {
        fmt.Print("\x1b[", 31 + id, "m ", id, " ", " \x1b[0m")
        bar1.bar_wait()

        fmt.Print("\x1b[7m\x1b[", 31 + id, "m ", id, " \x1b[0m")
        bar2.bar_wait()
    }
    aFinished <- true
}

// main

func main() {
    aFinished = make(chan bool, 1)
    bar1 = newBarrier(N)
    bar2 = newBarrier(N)

    // start go routines
    for i := 0; i < N; i++ {
        go a(i)
    }
    // wait for routines
    for i := 0; i < N; i++ {
        <- aFinished
    }
}

