#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>


#define MAN_BUFFER  256
#define BUFFER_SIZE 80
#define ECHOED_COMMAND_PIPE	"ECHO_COMMAND_PIPE.pipe"			//pipes need exact same .pipe name, so read_command_pipe is actually our command pipe on this side
#define COMMAND_PIPE		"READ_COMMAND_PIPE.pipe"

int main()
{
	int command_pipe, echo_pipe;
	char buffer[BUFFER_SIZE];
	char manual_buffer[MAN_BUFFER] = "Type in command: options \n turn on, turn off, accel, brake, turn_r, turn_l, reverse, go, stop \n See readme.txt to see more info on commands \n";
	

	mkfifo(COMMAND_PIPE, 0666);					//make pipes, 0666 makes them read or write capable
	mkfifo(ECHOED_COMMAND_PIPE, 0666);
	
	
	
	if ((echo_pipe = open(ECHOED_COMMAND_PIPE, O_RDONLY))==-1){
		fprintf(stderr, "echo_pipe creation failed");
	}
	else if ((command_pipe = open(COMMAND_PIPE, O_WRONLY))==-1){	//if statements opens pipes to make them read or write
		fprintf(stderr, "command_pipe creation failed");
		return 1;
	}
	int error_cnt;
	fprintf(stdout, manual_buffer);
	while(1){
		
		fflush(stdout);
		fflush(stdin);						//flushing everything at beginning of loop
									//to ensure no clogging of pipes
		
		fgets(buffer, sizeof(buffer), stdin);			//gets command typed and goes to buffer
		write(command_pipe, buffer, strlen(buffer));			//writes buffer to pipe2
		fflush(stdin);
		int b_read = read(echo_pipe, buffer, sizeof(buffer)-1);	//reads pipe 1 and puts it in buffer
		buffer[b_read] = '\0';					//terminates the end of command 
		
		if (strlen(buffer) < 1){				//making sure command was sent
			error_cnt = error_cnt + 1;
		}
		else {
			error_cnt = 0;
			printf("Command received: ");	
			fprintf(stdout, "%s", buffer);			//prints out status
			fflush(stdout);
		}
		if (error_cnt > 7) break;
			
		
	}
	
	close(command_pipe);
	close(echo_pipe);
	
	unlink(ECHOED_COMMAND_PIPE);
	unlink(COMMAND_PIPE);						//closes and deletes .pipe files
	
	return 0;	

}
