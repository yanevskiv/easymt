package main

import (
    "container/heap"
    "fmt"
    "time"
)

// constants
const N = 5

// Job priority queue
type Job struct {
    id int
    prio int
    index int
}

type JobQueue []*Job

// sort interface
func (jq JobQueue) Len() int {
    return len(jq)
}

func (jq JobQueue) Less(i, j int) bool {
    return jq[i].prio < jq[j].prio
}

func (jq JobQueue) Swap(i, j int) {
    jq[i], jq[j] = jq[j], jq[i]
    jq[i].index = i
    jq[j].index = j
}

// heap interface
func (jq *JobQueue) Push(x any) {
    n := len(*jq)
    job := x.(*Job)
    job.index = n
    *jq = append(*jq, job)
}

func (jq *JobQueue) Pop() any {
    old := *jq
    n := len(old)
    job := old[n - 1]
    old[n - 1] = nil
    job.index = -1
    *jq = old[0 : n-1]
    return job
}

func (jq *JobQueue) update(job *Job, ) {

}


// channels and priority queue
var aFinished chan bool
var qDelay JobQueue
var isWorking bool
var mutex chan bool
var privc [N]chan bool 


func sjn_wait(id, prio int) {
    <- mutex
    if isWorking {
        job := &Job{
            id: id,
            prio: prio,
        }
        heap.Push(&qDelay, job)
        mutex <- true
        <- privc[id]
    }
    isWorking = true
    mutex <- true
}

func sjn_signal() {
    <- mutex
    isWorking = false
    if qDelay.Len() > 0 {
        job, _ := heap.Pop(&qDelay).(*Job)
        privc[job.id] <- true
    } else {
        mutex <- true
    }
}



// routines
func a(id int) {
    prio := id
    for {
        sjn_wait(id, prio)
        fmt.Print(id)
        time.Sleep(2 * time.Microsecond)
        sjn_signal()
        time.Sleep(2 * time.Microsecond)
    }
}

// main func
func main() {
    // init
    isWorking = false
    qDelay = make(JobQueue, 0)
    heap.Init(&qDelay)
    aFinished = make(chan bool, N)
    mutex = make(chan bool, 1)
    mutex <- true
    for i := 0; i < len(privc); i++ {
        privc[i] = make(chan bool, 1)
    }


    // run
    for i := 0; i < N; i++ {
        go a(i)
    }
    for i := 0; i < N; i++ {
        <- aFinished
    }
}
