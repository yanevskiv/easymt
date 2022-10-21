/**
 * (c) Ivan Janevski
 */
package main

import (
    "fmt"
    "math/rand"
)

// constants
const (
    RED     = "\x1b[31m"
    GREEN   = "\x1b[32m"
    YELLOW  = "\x1b[33m"
    NOCOLOR = "\x1b[0m"
)
const (
    MATCHES = 0
    PAPER = 1
    TOBACCO = 2
)

type Table struct {
    items [3]bool
}

func (table *Table) put_two(item1, item2 int) {
    table.items[item1] = true
    table.items[item2] = true
}

func (table *Table) get_two(item1, item2 int) {
    table.items[item1] = false
    table.items[item2] = false
}

// channels
var ok, paper, matches, tobacco chan bool
var table Table

// go routines
func agent(done chan bool) {
    for {
        item := rand.Intn(3)
        switch item {
            case MATCHES: 
                fmt.Println(RED + "Producing PAPER and TOBACCO" + NOCOLOR);
                table.put_two(PAPER, TOBACCO)
                matches <- true
                <- ok
            case PAPER: 
                fmt.Println(GREEN + "Producing TOBACCO and MATCHES" + NOCOLOR);
                table.put_two(TOBACCO, MATCHES)
                paper <- true
                <- ok
            case TOBACCO: 
                fmt.Println(YELLOW + "Producing MATCHES and PAPER" + NOCOLOR);
                table.put_two(MATCHES, PAPER)
                tobacco <- true
                <- ok
        }
    }
}


func smoker(item int, done chan bool) {
    for {
        switch item {
            case MATCHES: 
                <- matches
                fmt.Println(RED + "Consuming PAPER and TOBACCO" + NOCOLOR);
                table.get_two(PAPER, TOBACCO)
                ok <- true
            case PAPER: 
                <- paper
                fmt.Println(GREEN + "Consumign TOBACCO and MATCHES" + NOCOLOR);
                table.get_two(TOBACCO, MATCHES)
                ok <- true
            case TOBACCO: 
                <- tobacco
                fmt.Println(YELLOW + "Consuming MATCHES and PAPER" + NOCOLOR);
                table.get_two(MATCHES, PAPER)
                ok <- true
        }
    }
}

func main() {
    done   := make(chan bool, 1)
    ok      = make(chan bool, 1)
    paper   = make(chan bool, 1)
    matches = make(chan bool, 1)
    tobacco = make(chan bool, 1)
    go agent(done)
    go smoker(PAPER, done);
    go smoker(MATCHES, done);
    go smoker(TOBACCO, done);
    for i := 0; i < 4; i++ {
        <- done
    }
}
