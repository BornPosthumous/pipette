#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "line.h"

#define die(e) do { fprintf(stderr, "%s\n",e); exit(EXIT_FAILURE); } while (0);

#define TWO_DIM_ARR_BUFSIZE 800

// Find the number of pipes and add one~
int get_number_of_programs(char** tokens, int total_tokens){
  int number_of_programs = 0;

  for(int i = 0; i < total_tokens; i++){
    if(strcmp(tokens[i], "|") == 0){
      number_of_programs++;
    }
  }
  return number_of_programs + 1;
}
// This code is for generating a 2 dimensional array. Keeping it here for reference
void seed_command_list2
(
  int     number_of_programs,
  char*** command_list,
  char**  tokens,
  int     total_tokens
){
  int buffer_size_in_bytes    = TWO_DIM_ARR_BUFSIZE * sizeof(char*);
  int token_position_in_line  = 0;

  for(int program_index = 0; program_index < number_of_programs; program_index++){
    // TODO check against the bufsize
    command_list[program_index] = (char**)realloc(command_list[program_index], buffer_size_in_bytes);

    if( !command_list[program_index] ){
      fprintf(stderr, "Error allocating %d", program_index);
      die("Error Allocating program_index");
    }

    int token_position_in_command  = 0;
    for(int j = token_position_in_line; j < total_tokens; j++){

      if(strcmp(tokens[token_position_in_line], "|")){

        command_list[program_index][token_position_in_command] = tokens[token_position_in_line];
        token_position_in_command++;
        token_position_in_line++;

      } else {

        token_position_in_command = 0;
        token_position_in_line++;
        break;

      }
    }
  }
}

void seed_command_list
(
  int     number_of_programs,
  ArrayWithLength* command_list,
  char**  tokens,
  int     total_tokens
){

  int buffer_size_in_bytes    = TWO_DIM_ARR_BUFSIZE * sizeof(char*);
  // Initialize command list
  for(int i = 0; i < number_of_programs; i++){
    command_list[i].size = 0;
    command_list[i].arr = malloc(buffer_size_in_bytes);
    if( !command_list[i].arr ){
      fprintf(stderr, "Error allocating %d", i);
      die("Error Allocating program_index");
    }

  }
  int token_position_in_line  = 0;

  for(int program_index = 0; program_index < number_of_programs; program_index++){

    int token_position_in_command  = 0;
    for(int j = token_position_in_line; j < total_tokens; j++){

      if(strcmp(tokens[token_position_in_line], "|")){

        command_list[program_index].arr[token_position_in_command] = tokens[token_position_in_line];
        token_position_in_command++;
        token_position_in_line++;
        command_list[program_index].size++;
      } else {

        token_position_in_command = 0;
        token_position_in_line++;

        break;

      }
    }
  }
}

void run_cmd(int input_fd, int output_fd, ArrayWithLength command_list_item){
    pid_t pid;
    pid = fork();
    printf("Run Command: %s \t PID: %d \n", command_list_item.arr[0], pid);

    switch(pid){
    case 0:
      if(input_fd != 0){
        dup2(input_fd, STDIN_FILENO);
      }
      dup2(output_fd, STDOUT_FILENO);

      close(input_fd);
      close(output_fd);

      execvp(command_list_item.arr[0], command_list_item.arr);
      perror("Running mid command");
      die("mid");

    default:
      break;
    case -1:
      perror("Couldnt fork in dest");
      exit(1);
  };
}

void print_command_list( ArrayWithLength* _command_list, int number_of_programs ){
  for(int i = 0; i < number_of_programs; i++){
    for(int j = 0; j < _command_list[i].size; j++){
      fprintf(stdout, "Index: %d, Size: %d, String: %s \n",
              j,
              _command_list[i].size,
              _command_list[i].arr[j] );
    }
  }
}
char* get_arguments( int argc, char **argv, char* argument_buffer ){
  for(int i = 1; i < argc; i++ ){
    strcat(argument_buffer, " ");
    strcat(argument_buffer, argv[i]);
  }

  return argument_buffer;
}

int main( int argc , char **argv ) {
    /*
    This code gets the args and parses the arguments
   */
  char argument_buffer[1028];
  char* arguments = get_arguments(argc, argv, argument_buffer);

  //ArrayWithLength is just a vector but I wanted to do it myself
  ArrayWithLength* split_input = lsh_split_line(arguments);

  int     total_tokens       = split_input->size;
  char**  tokens             = split_input->arr;
  int     number_of_programs = get_number_of_programs(tokens, total_tokens);

  // This code is for generating a 2 dimensional array. Keeping it here for reference
  /* char*** command_list       = malloc(number_of_programs * sizeof(char**)); */
  /* seed_command_list2( number_of_programs, command_list, tokens, total_tokens); */

  ArrayWithLength _command_list[number_of_programs];
  seed_command_list( number_of_programs, _command_list, tokens, total_tokens);

  // Make sure we got what we expected
  print_command_list(_command_list, number_of_programs);

  /*
    This code creates the child processes and pipes em together
   */
  pid_t pid;
  int   status;
	int   link[2];
  int   nbytes = 0;
  int   input_fd = 0;
  char  readbuffer[4096];

  for(int i = 0; i < number_of_programs; ++i){
    // Nifty trick is to just reuse the pipes instead of making n-1 pipes
    pipe(link);

    run_cmd(input_fd, link[1], _command_list[i]);
    close(link[1]);

    input_fd = link[0];

  };

  while ((pid = wait(&status)) != -1){
		fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
  }
  // input_fd should point to the last pipes file descriptor
  nbytes = read(input_fd, readbuffer, sizeof(readbuffer));
  printf("Output: (%.*s)\n", nbytes, readbuffer);

  free(tokens);

  exit(0);
  return 0;
}
