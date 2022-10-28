xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6). xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

ACKNOWLEDGMENTS

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)). See also https://pdos.csail.mit.edu/6.1810/, which provides
pointers to on-line resources for v6.

The following people have made contributions: Russ Cox (context switching,
locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin
Clements.

We are also grateful for the bug reports and patches contributed by
Takahiro Aoyagi, Silas Boyd-Wickizer, Anton Burtsev, carlclone, Ian
Chen, Dan Cross, Cody Cutler, Mike CAT, Tej Chajed, Asami Doi,
eyalz800, Nelson Elhage, Saar Ettinger, Alice Ferrazzi, Nathaniel
Filardo, flespark, Peter Froehlich, Yakir Goaron, Shivam Handa, Matt
Harvey, Bryan Henry, jaichenhengjie, Jim Huang, Matúš Jókay, John
Jolly, Alexander Kapshuk, Anders Kaseorg, kehao95, Wolfgang Keller,
Jungwoo Kim, Jonathan Kimmitt, Eddie Kohler, Vadim Kolontsov, Austin
Liew, l0stman, Pavan Maddamsetti, Imbar Marinescu, Yandong Mao, Matan
Shabtay, Hitoshi Mitake, Carmi Merimovich, Mark Morrissey, mtasm, Joel
Nider, Hayato Ohhashi, OptimisticSide, Harry Porter, Greg Price, Jude
Rich, segfault, Ayan Shafqat, Eldar Sehayek, Yongming Shen, Fumiya
Shigemitsu, Cam Tenny, tyfkda, Warren Toomey, Stephen Tu, Rafael Ubal,
Amane Uehara, Pablo Ventura, Xi Wang, WaheedHafez, Keiichi Watanabe,
Nicolas Wolovick, wxdao, Grant Wu, Jindong Zhang, Icenowy Zheng,
ZhUyU1997, and Zou Chang Wei.

The code in the files that constitute xv6 is
Copyright 2006-2022 Frans Kaashoek, Robert Morris, and Russ Cox.

ERROR REPORTS

Please send errors and suggestions to Frans Kaashoek and Robert Morris
(kaashoek,rtm@mit.edu). The main purpose of xv6 is as a teaching
operating system for MIT's 6.1810, so we are more interested in
simplifications and clarifications than new features.

BUILDING AND RUNNING XV6

You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu. Once they are installed, and in your shell
search path, you can run "make qemu".

All the programs have been compiled on a LINUX OS (ubuntu 20.04)

```
The program can be compiled using:
make qemu
```

-   Executing the file will generate a prompt to accept input from the user.
-   In case the user wishes to compile with a particular scheduler:

```
The program can be compiled using:
make qemu SCHEDULER=<scheduler_name>
```

-   It is advised to run

```
make clean
```

before executing any scheduling algorithm.

---

-   The system call trace and a user program strace.c were added to implement strace.

```
The strace syscall can be run as:
-> strace mask command [args]
```

-   The system calls sigalarm and sigreturn were added, sigalarm(interval, handler) periodically alerts a process as it uses CPU time.
-   When the function returns, another system call sigreturn() is called to reset the process state to before the handler
    was called.

```
The sigalarm and sigreturn syscalls are handled implicitly and can be verified by calling
-> alarmtests
```

---

## SCHEDULING ALGORITHMS

<br>

The default scheduler of xv6-ricv is Round Robin. We were asked to implement First Come First Serve Scheduling (FCFS), Lottery Based Scheduling (LBS), Priority Based Scheduling (PBS) and Multi-Level Feedback Queue Scheduling (MLFQ).
To support the different types of scheduling algorithms during compilation, we modified the Makefile to include the SCHEDULER macro.
<br>

```
ifndef SCHEDULER
    SCHEDULER:=RR
endif
CFLAGS+=-D$(SCHEDULER)
```

### 1. FIRST COME FIRST SERVE

<br>
First come first serve (FCFS) scheduling algorithm schedules jobs according to their arrival time. The job which comes first in the ready queue will get the CPU first. FCFS scheduling may cause the problem of starvation if the burst time of the first process is the longest among all the jobs. In this case the first process hogs all the CPU time and thus the other processes remain waiting.

1.  FCFS selects the process with the least creation time, i.e, the process that is created first. Therefore, a new field was processCreation was added to the struct proc in **kernel/proc.h**.
    <br>

2.  Initialised the creation time of the process to 0 while allocating a new process in allocproc() in **kernel/proc.h**.
    <br>

3.  Implemented the scheduling algorithm in the scheduler function in **kernel/proc.c**.
    <br>

        - Enclosed the algorithm between preprocessors #ifdef and #endif

        - Initialised a process structure called **first_proc**.

        - Iterated through all the processes in the process table to find runnable processes.

        - If the process was not the first process then the allocate first_proc the current process in the loop.

        - Otherwise, check if the creation time of the process in the loop is lesser than first_proc->processCreation.

        - If the first_proc exists then release its lock. Then allocate the process in the loop to the first_proc.

        - Allocate the cpu to  first_proc and perform context switching with the help of **swtch()**.

        - Finally reset the cpu process to 0 and release the lock on **first_proc**.

    <br>
    <br>
    <br>

### 2. PRIORITY BASED SCHEDULING

<br>
Priority Scheduling is a method of scheduling processes that is based on priority. In this algorithm, the scheduler selects the tasks to work as per the priority.
The processes with higher priority should be carried out first. Each process is allocated a static priority in the range 0 to 100. The smaller the value of static priority, the higher the priority of the process. We have implemented a non-preemptive PBS scheduler.

1.  Added variables **SP**, **run_time** , **end_time** , **start_time** , **nSchedule** , and **sleep_time** to the struct proc in **kernel/proc.h**.
    <br>

2.  Initialised the run_time , start_time , SP , end_time, nSchedule and total_runtime in allocproc() in kernel/proc.c.
    <br>

3.  Added the scheduling functionality for PBS in scheduler() in kernel/proc.c.
    <br>

        - Calculated the niceness and dynamic priority of the process using the provided formulae.

        - Then we begin by comparing the priorities of the processes.

        - When two or more processes have the same priority, the number of times the process has been scheduled is used to determine the priority. In case there is still a tie, the start-time of the processes are used to break the tie, with the processes having a lower start-time being assigned a higher priority.

        - If a process is found, we set it as the process for the CPU to execute, and perform context switching using swtch().

4.  Added a syscall set_priority by adding a user program set_priority.c which allows the user to assign a priority to a process with a specific pid. This function also resets the niceness to 5.
    <br>
    <br>
    <br>

### 3. LOTTERY BASED SCHEDULING

<br>
In this scheduling, every process has some tickets allocated to it. The scheduler picks a random ticket and the process having that ticket is the winner i.e., it is executed for a time slice and then another ticket is picked by the scheduler. These tickets represent the share of processes. A process having a higher number of tickets has a higher chance of getting chosen for execution. In other words,. the probability that the process runs in a given time slice is proportional to the number of tickets owned by it.

1.  Added variable **tickets** to the struct proc in **kernel/proc.h**.

2.  Initialised tickets in allocproc() in kernel/proc.c.

3.  Added the scheduling functionality for LBS in scheduler() in kernel/proc.c
    <br>

        - Found the total number of tickets allocated to each process.

        - Using a random number generator defined in kernel/rand.c, a random number between 0 and total tickets is generated. Let this be called **reqdLottery**.

        - Looping through all processes, we keep incrementing the number of tickets we have gone through, in each process. If this total exceeds reqdLottery, we have found the process to be executed.

        - This process is then assigned to the CPU for execution, and context switching is performed using swtch().

    <br>
    <br>
    <br>
    <br>

### 4. MULTI-LEVEL FEEDBACK QUEUE SCHEDULING

<br>

Implemented a simplified preemptive MLFQ scheduler that allows processes to move between different priority queues based on their behavior and CPU bursts.

1. A process that uses too much CPU time gets pushed to a lower priority queue.
2. To prevent starvation, aging was implemented with a limit of 30 ticks per process.
3. Added the scheduling functionality for LBS in scheduler() in kernel/proc.c
    - If the time the process spends in the queue is >=30 ticks then we remove the process from its current queue and decrease its priority.
    - Then we push the process at the end of the subsequent queue of lower priority.
    - Then we iterate through the queues and run processes in lower priority queues first.
    - A process that voluntarily relinquishes CPU control and then comes back is inserted at the end of the queue which it was previously in.

### **PERFORMANCE COMPARISON OF THE DIFFERENT SCHEDULING ALGORITHMS**

<br>

|            SCHEDULING ALGORTIHM             | AVERAGE RUNTIME | AVERAGE WAIT TIME |
| :-----------------------------------------: | :-------------: | :---------------: |
|              Round Robin (RR)               |       15        |        114        |
|        First Come First Serve (FCFS)        |       34        |        42         |
|        Lottery Based Scheduler (LBS)        |       10        |        118        |
|       Priority Based Scheduler (PBS)        |       16        |        108        |
| Multi-Level Feedback Queue Scheduler (MLFQ) |       14        |        154        |

<br>
<br>
The results for RR, FCFS, PBS and LBS are obtained by running schedulertest on 3 CPUs, whereas MLFQ is run on only 1 CPU.

<br>

**QUESTION:** If a process voluntarily relinquishes control of the CPU(eg. For doing I/O), it leaves
the queuing network, and when the process becomes ready again after the I/O, it is
inserted at the tail of the same queue, from which it is relinquished earlier. Explain how could this be exploited by a process.
<br>

**ANSWER:** A process can exploit this by leaving the CPU before its completion, and getting added to the same queue (i.e., it retains its priority), which can prevent other processes with lower priority to get executed unless aging is implemented. This can lead to starvation of the other processes.
