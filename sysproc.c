#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "uproc.h"


extern struct proc * getptable_proc(void);

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_trace(void){
  int n;
  struct proc *p = myproc();
  argint(0, &n);
  if(n==1)
    {
      cprintf("\nTrace turned on for %s(pid: %d) \n ",p->name,p->pid);
      p->trace = 1;
    }
    else
    {
      cprintf("\nTrace turned off for %s(pid: %d) \n",p->name,p->pid);
      p->trace = 0;
      return p->num_of_sys_calls; 
    }
  return 0;
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

extern struct proc * getptable_proc(void);


int sys_getprocs(void){
  
  int size;
  struct uproc *buf;
  struct proc *ptable = '\0';
  char *pbuf;
  //get the max size of the buffer
  if (argint(0, &size) <0){
    //return -1 if the size is less than 0
    return -1;
  }
  //get the pointer to the buffer
  if (argptr(1, &pbuf,size*sizeof(struct uproc)) <0){
    //return -1 if the pointer is null
    return -1;
  }
//cast the pointer to the buffer
buf = (struct uproc *)pbuf;
//get the process table
ptable = getptable_proc();
int count = 0;
//loop through the process table and copy the data to the buffer
for(int i=0; i<size; ptable++, i++){
  //check if the process is in use
  if(ptable->state != UNUSED){
    buf[count].pid = ptable->pid;
    buf[count].state = ptable->state;
    buf[count].sz = ptable->sz;
    //check if a parent process exists
    buf[count].ppid = ptable->parent->pid > 0 ? ptable->parent->pid : 0;
    //copy the name of the process
    strncpy(buf[count].name,ptable->name,sizeof(ptable->name));
    count++;
  }
}
//return the number of processes copied to the buffer
return count;
  
}
