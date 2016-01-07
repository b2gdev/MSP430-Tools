/*!
*********************************************************************************************************
*
*                          	                 getAllFWVersions
*                                       (c) Copyright Zone24x7 Inc.
*
*
*********************************************************************************************************
*/

/*!
*********************************************************************************************************
*
* Filename      : getAllFWVersions.c
* Version       : 1.0.0
* Programmer(s) : AH
*
*********************************************************************************************************
* Note(s) : (1) Device Node - /dev/cp430_core
*
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "cp430_ioctl.h"

int main(int argc, char *argv[])
{
	int fd     = 0;
	int ret    = 0;
	char buffer[10];
	
	fd = open("/dev/cp430_core", O_RDWR);

	if (fd < 0)
	{
		printf("Could not open device - /dev/cp430_core \r\n");
		return -1;
	}

	memset(buffer, 0, sizeof(buffer));
	ret = ioctl(fd, CP430_CORE_GET_ALL_FW_VERSIONS, buffer);
	if(ret < 0) {
		printf("CP430_CORE_GET_ALL_FW_VERSIONS ioctl failed : %d\r\n", ret);
	}
	else {
		printf("v%d.%d\n",buffer[0],buffer[1]);
		printf("v%d.%d\n",buffer[2],buffer[3]);
	}
	
	close(fd);
	return 0;
}
