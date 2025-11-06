#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main( int agrc, char* argv[], char* env[] )
{ //mknod /dev/cyh_misc  c 10 88
  ssize_t retV;  char BUF[64];   char showMess[]="CYH";
  int fd = open("/dev/cyh_misc", O_RDWR ); //open("/dev/charDrv1", O_RDWR );
   //
  retV = write ( fd , (void *)showMess, strlen(showMess) ); 
  retV = read ( fd, (void *)BUF, 64 ); // "Hi " + showMess
  printf( "BUF=%s\r\n", BUF );  // "Hi CYH"
  close(fd );
  return 0;
}
/*
ssize_t read (int filedes, void *buffer, size_t size)  
ssize_t write (int filedes, const void *buffer, size_t size) 

*/

