#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//openCloseRW.exe-> crtinit -> 0 1(stdout) 2  -->open 3
// openCloseRW.exe  SRCfile  TargFile[enter]

int main( int argc, char* argv[], char* env[] )
{   
	//mknod /dev/charDrv1  c 88 0
	ssize_t retV;  char BUF[64];   char showMess[]="CYH";
	int fd = open("/dev/charDrv1", O_RDWR );
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


