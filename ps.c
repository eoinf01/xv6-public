#include "types.h"
#include "stat.h"
#include "user.h"
#include "uproc.h"

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };


#define MAX_PROC 10


static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };

int
main(int argc, char *argv[]){
	//array of uproc structs of size MAX_PROC
	struct uproc ptable[MAX_PROC];
	int err;
	//call getprocs system call
	err = getprocs(10,ptable);
	//check for error
	if(err < 0){
		printf(1,"Error getting ptable");
		//exit with error
		exit();
	}
	//print the header
	printf(1,"PID PPID STATE    SIZE   NAME\n");
	//loop through the ptable and print the data
	for(int i=0;i<err;i++){
		printf(1,"%d   %d   %s   %d    %s \n",
		ptable[i].pid, ptable[i].ppid, states[ptable[i].state], ptable[i].sz, ptable[i].name);
	}
	//exit with no error
	exit();
}
