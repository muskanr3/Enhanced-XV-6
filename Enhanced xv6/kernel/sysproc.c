#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 a1;
  int n;

  argint(0, &n);
  a1 = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return a1;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;

  argint(0, &mask);

  myproc()->mask = mask;

  return 0;
}

uint64
sys_settickets(void)
{
  int nTickets;

  if (argint(0, &nTickets) >= 0)
  {
    myproc()->ticket = nTickets;

    return nTickets;
  }

  return -1;
}

uint64
sys_set_priority()
{
  int pid, priority;
  
  if(argint(0, &priority) < 0 || argint(1, &pid) < 0)
  {
      return -1;
  }

  return set_priority(priority, pid);
}

uint64
sys_waitx(void)
{
  uint64 a1, a2, a3;
  uint waittime, runtime;
  if (argaddr(0, &a1) < 0)
    return -1;
  if (argaddr(1, &a2) < 0) // user virtual memory
    return -1;
  if (argaddr(2, &a3) < 0)
    return -1;
  int ret = waitx(a1, &waittime, &runtime);
  struct proc *p = myproc();
  if (copyout(p->pagetable, a2, (char *)&waittime, sizeof(int)) < 0)
    return -1;
  if (copyout(p->pagetable, a3, (char *)&runtime, sizeof(int)) < 0)
    return -1;
  return ret;
}
// extra system calls
uint64
sys_sigalarm(void)
{
  int intervals;
  uint64 addr;

  if (argint(0, &intervals) < 0 || argaddr(1, &addr) < 0)
  {
    return -1;
  }

  struct proc *p = myproc();

  p->alarmIntr = intervals;
  p->numAlrticks = 0;
  p->alarm_handler_addr = addr;
  p->is_sigalarm = 0;

  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();

  *(p->trapframe) = *(p->user_saved_tf);
  p->is_sigalarm = 0;

  return p->trapframe->a0;
}