#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>



uint8_t FindMotors(int file, uint8_t addr) {
  uint8_t i=1;
  uint8_t buf[5];

	//See if there are motors already assigned
	printf("Looking for preassigned motors on the bus...\n");
	while(1){
		if (ioctl(file,I2C_SLAVE,addr+i) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }   

    buf[0] = 0x09; //get enc, change to //CMD_GET_STATUS
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;

    if (write(file,buf,5) != 5) {
        /* ERROR HANDLING: i2c transaction failed */
        printf("No motor found at addr: 0x%00x, stopping search\n",addr+i);
        break;
    } 
		//Now read the data to not make controllers angry
    if (read(file,buf,4) != 4) {
      /* ERROR HANDLING: i2c transaction failed */
      printf("Failed to read from the i2c bus.\n");
      printf("\n\n");
    } 
		printf("Found Motor[%d] at addr: 0x%00x\n", i, addr+i);
		i++;
	}

	printf("Looking for any new motors on the bus...\n");
  while(1) {
    if (ioctl(file,I2C_SLAVE,addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }   

    buf[0] = 0x00; //Change Address
    buf[1] = (addr+i)<<1;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;

    if (write(file,buf,5) != 5) {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Failed to find any more motors\n");
        break;
    }   

    if (ioctl(file,I2C_SLAVE,addr+i) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }   

    if (read(file,buf,4) != 4) {
      /* ERROR HANDLING: i2c transaction failed */
      printf("Failed to read from the i2c bus.\n");
      printf("\n\n");
    }   
    else {
      printf("Motor[%d] found, assigned addr 0x%00x\n",i,addr+i); 
    }   
    i++;
  }
  return i-1;
}

void main(void) {
    int file;
    char filename[40];
    int addr = 0x48 >> 1;        // The I2C address of the ADC
    sprintf(filename,"/dev/i2c-2");
    if ((file = open(filename,O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }
    //Find motors at base address addr
    printf("%d Motors Found!\n", FindMotors(file,addr));
}
