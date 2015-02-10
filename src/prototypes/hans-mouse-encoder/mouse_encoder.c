// Written by: Hans Luchsinger
// Last Modified: 11/05/14
// Project: Optical Mouse Encoder

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <getopt.h>
#include <string.h>

#define MOUSE_SOURCE "usb-Logitech_Gaming_Mouse_G502_047F345C3437-event-mouse"

int fd;

void signal_handle(int signum, siginfo_t *siginfo, void *ucontext){
	printf("\n\nClosing all files\nSuccess\n\n");
	int grab = 0;
	ioctl(fd, EVIOCGRAB, &grab);
	close(fd);
	exit(signum);
}

int main(){
	struct input_event new;
//	char buffer[4096];
	int write_read;
	int device_lock;
	int grab = 1;
	double measure =0;
	
	struct sigaction s;

	s.sa_sigaction = signal_handle;
	sigemptyset(&s.sa_mask);
	s.sa_flags = SA_SIGINFO;

	sigaction(SIGINT, &s, NULL);
	sigaction(SIGQUIT, &s, NULL); 
	
	printf("program start\n");
	if((fd = open("/dev/input/by-id/usb-Logitech_Gaming_Mouse_G502_047F345C3437-event-mouse", O_RDONLY)) == -1){
		printf("Failed to open device\n");
		printf("Check device name, location, and permissions\n");
		printf("Default location: /dev/inputs/by-id/usb-Logitech_gaming_mouse_G502_047F345C3437-event-mouse\n");
		exit(EXIT_FAILURE);
	}
	
	device_lock = ioctl(fd, EVIOCGRAB, &grab);

	if (device_lock == -1){
		printf("failed to lock device\n");
		close(fd);
		exit(EXIT_FAILURE);
	}
	
	while(((write_read = read(fd,& new, sizeof(new))) != 0)){
		if(new.type == EV_REL){
		//	if( new.code == REL_X){
		//		printf("REL_X: %i\n", new.value);
		//	}
			if (new.code == REL_Y){
			//	printf("REL_Y: %i\n", new.value);
				measure += new.value;
				printf("Measure: %f\n", measure);
			}
		}
	}

	grab = 0;
	device_lock = ioctl(fd, EVIOCGRAB, &grab);		
	
	close(fd);
	return 0;
}




