#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>


#define BUFFER_SIZE 80
#define ECHO_COMMAND_PIPE  "ECHO_COMMAND_PIPE.pipe"					//pipe used to write back to command terminal
#define READ_COMMAND_PIPE  "READ_COMMAND_PIPE.pipe"					//pipe used to read manual instructions from command terminal


int main()
{
	int echo_command_pipe, read_command_pipe;
	char command_buffer[BUFFER_SIZE];
	char err_buffer[BUFFER_SIZE] = "Error, no command received \n";

	mkfifo(ECHO_COMMAND_PIPE, 0666);
	mkfifo(READ_COMMAND_PIPE, 0666);
		
	
	
	if ((echo_command_pipe = open(ECHO_COMMAND_PIPE, O_WRONLY))==-1){
		fprintf(stderr, "echo_command_pipe creation failed");
	}										//these 'if' statements create the pipes to be read or write (O_RDONLY or O_WRONLY)
	else if ((read_command_pipe = open(READ_COMMAND_PIPE, O_RDONLY))==-1){
		fprintf(stderr, "read_command_pipe creation failed");
		return 1;
	}
	int error_cnt;
	while(1){
	
		fflush(stdout);
		fflush(stdin);
		int command_read = read(read_command_pipe, command_buffer, sizeof(command_buffer)-1);	//reads received command and puts it in buffer
		
		command_buffer[command_read] = '\0';							//terminates end of buffer
										
		//fprintf(stdout, "%s", command_buffer);						//uncomment this "fprintf" lines if you want to see what is being received for debugging
		//do stuff here with jacobian
		
		if (strlen(command_buffer) < 1){
			write(echo_command_pipe, err_buffer, strlen(err_buffer));			//this if else structure just ensures data is being transferred by 
			error_cnt = error_cnt + 1;							//checking the string length in the buffer
			fflush(stdout);
		}
		else {
			write(echo_command_pipe, command_buffer,strlen(command_buffer));
			error_cnt = 0;
			fflush(stdout);
		}
		
		if (error_cnt > 7) break;
		
	}
	
	close(echo_command_pipe);
	close(read_command_pipe);
	
	unlink(ECHO_COMMAND_PIPE);
	unlink(READ_COMMAND_PIPE);	
	
	
	return 0;	

}
