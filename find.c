#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
  return buf;
}

void directoryCrawl(char *path,char *search){
	char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  
  //fill the fd int with the file descriptor at specified path
  if((fd = open(path, 0)) < 0){
    printf(2, "find: cannot check %s\n", path);
    return;
  }

  //fill the stat struct with the stats using the file descriptor
  if(fstat(fd, &st) < 0){
    printf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  	//check the length of the path is not greater that the maximum directory character limit
	if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "find: path too long\n");
    }
    //copy the oath into the buffer
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';

    //loop through the contents of the directory
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      //if the 
      if(stat(buf, &st) < 0){
        printf(1, "find: cannot stat %s\n", buf);
        continue;
      }
      //check if the current file is a directory 
      if (st.type == T_DIR) {
      		//if the path name is not . or ..
	      if (strcmp(fmtname(buf), ".") != 0 && strcmp(fmtname(buf), "..") != 0) {
	        // get the file descriptor for the current file
	        int fd2 = open(buf, 0);
	        // checks if the file has stats
	        if(fstat(fd, &st) < 0){
	          printf(2, "find: cannot stat %s\n", path);
	          close(fd);
	          return;
	        }
	        // search the directory recursibely if it has valid stats
	        directoryCrawl(buf, search);
	        //dereference fd2 from pointing at the old file descriptor
	        close(fd2);
      }
    } else if (st.type == T_FILE) {
    //if the file is a file and the path is equal to the search paremeter print to console
      if (strcmp(fmtname(buf), search) == 0) {
        printf(1,"%s\n", buf);
      }
    }
    }

}

void
find(char *path,char *search)
{
  int fd;
  struct stat st;
  //open the file descriptor at that path
  if((fd = open(path, 0)) < 0){
    printf(2, "find: cannot check %s\n", path);
    return;
  }

  //fill the stat struct with stats using the specified file descriptor
  if(fstat(fd, &st) < 0){
    printf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  //switch statement 
  switch(st.type){
  	//if the file type is a directory crawl this directory
  	case T_DIR:
  		directoryCrawl(path,search);
  		break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    printf(1,"find: no file name specified\n");
    exit();
  }
  if(argc==2){
  	find(".",argv[1]);
  	exit();
  }
  for(i=1; i<argc; i++)
    find(argv[1],argv[i]);
  exit();
}
