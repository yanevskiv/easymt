package main

import (
    "fmt"
)

// constants 
const (
    NB int    = 1
    NC        = 15
    MAX_SEATS = 10
)
const (
    INVALID int = -1
    BARB_ENTER  = 1
    CUST_ENTER  = 2
)


// message
type Msg struct {
    id int
    action int
}

// channels
var chServer chan Msg;
var chBarber [NB]chan Msg;
var chCustomer [NC]chan Msg;
var isDone chan bool

// go routines
func server() {
    cCount := 0 // number of entered customers
    bCount := 0 // number of entered barbers
    qCustomers := make(chan int, NB)
    qBarbers := make(chan int, NC)
    for {
        msg := <- chServer
        id := msg.id
        action := msg.action
        switch action {
            case CUST_ENTER:
                if cCount >= MAX_SEATS {
                    // no room
                    msg.id = INVALID
                    chCustomer[id] <- msg
                } else if cCount > 0 {
                    // queue customer
                    cCount += 1
                    qCustomers <- id
                } else {
                    // take barber from qBarbers if not empty
                    select {
                        case barbId := <- qBarbers:
                            bCount -= 1
                            // awake customer
                            msg.id = barbId
                            chCustomer[id] <- msg
                            // awake barber
                            msg.id = id
                            chBarber[barbId] <- msg
                        default:
                            // queue customer
                            cCount += 1
                            qCustomers <- id
                    }
                }
            case BARB_ENTER:
                if bCount > 0 {
                    // queue barber
                    bCount += 1
                    qBarbers <- id
                } else {
                    // take customer from qCustomers if not empty
                    select {
                        case custId := <- qCustomers:
                            cCount -= 1
                            // awake customer
                            msg.id = id
                            chCustomer[custId] <- msg
                            // awake barber
                            msg.id = custId
                            chBarber[id] <- msg
                        default:
                            // queue barber
                            bCount += 1
                            qBarbers <- id
                    }
                }
        }
    }
    isDone <- true
}

func barber(id int) {
    var msg Msg
    for {
        // await customer
        msg.id = id
        msg.action = BARB_ENTER;
        chServer <- msg
        msg = <- chBarber[id]

        // cut hair
        custId := msg.id
        fmt.Println("Barber", id, "is cutting hair of ", custId)
    }
    isDone <- true
}

func customer(id int) {
    var msg Msg
    for {
        // await barber
        msg.id = id
        msg.action = CUST_ENTER;
        chServer <- msg
        msg = <- chCustomer[id]

        // cut hair
        barbId := msg.id
        if barbId != INVALID {
            fmt.Println("Customer ", id, " is getting a haircut from barber ", barbId)
        } else {
            fmt.Println("No room for customer ", id)
        }
    }
    isDone <- true
}

func main() {
    // init channels
    isDone = make(chan bool, 1 + NC + NB)
    chServer = make(chan Msg, NC + NB)
    for i := 0; i < NB; i++ {
        chBarber[i] = make(chan Msg, 1)
    }
    for i := 0; i < NC; i++ {
        chCustomer[i] = make(chan Msg, 1)
    }

    // run routines
    go server();
    for i := 0; i < NB; i++ {
        go barber(i)
    }
    for i := 0; i < NC; i++ {
        go customer(i)
    }

    // wait
    for i := 0; i < 1 + NC + NB; i++ {
        <- isDone
    }
}
