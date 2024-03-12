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
  return 0;  // not reached
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
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);

  backtrace(); // call backtrace here

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
sys_sigreturn(void)
{
	// restore registers saved in usertrap
	struct proc *p = myproc();
	*(p->trapframe) = *(p->temp_trapframe);

	p->handler_lock = 0; // unlock alarm hanlding
	usertrapret(); // call usertrapret to restore registers immediatel because a return here will modify a0

	return 0;
}

uint64
sys_sigalarm()
{
	int interval;
	uint64 handler_ptr;

	argint(0, &interval);
	argaddr(1, &handler_ptr);

	struct proc *p = myproc();

	if (interval == 0 && handler_ptr == 0) {
		p->alarm_registered = 0; // disable alarming
	} else {
		p->alarm_interval = interval;
		p->alarm_handler = (void (*)())handler_ptr;
		p->alarm_registered = 1; // turn on alarming
		p->tick_left = interval; // set the tick left to the number of ticks configured
	}

	return 0;
}
