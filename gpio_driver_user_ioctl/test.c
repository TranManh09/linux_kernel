#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_FILE "/dev/gpio_driver"

#define LED_OFF 0
#define LED_ON 1

int main() {
    int fd;
    int ret;
	int num;
    // Mở thiết bị
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return EXIT_FAILURE;
    }
	while(1){
		printf("enter: ");
		scanf("%d", &num);
		if(num ==1){
			
    			ret = ioctl(fd, LED_ON, NULL);
    			if (ret == -1) {
        		perror("Failed to turn on LED");
        		close(fd);
        		return EXIT_FAILURE;
			}
		}
		else if(num == 0){
			
    			ret = ioctl(fd, LED_OFF, NULL);
    			if (ret == -1) {
        		perror("Failed to turn on LED");
        		close(fd);
        		return EXIT_FAILURE;
			}
		
		}
    		else{
			break;
		}	
		}

    printf("Turned on LED successfully\n");

    // Đóng thiết bị
    close(fd);
    

    return EXIT_SUCCESS;
}

