#ifndef WOOSH_H_
#define WOOSH_H_

/*************************
    debug flags and meta
**************************/

#define WOOSH_VERSION "0.0.1"

// Toggle the DEBUG print statements on or off. 1 = print debug 0 = not.
// Credit to https://stackoverflow.com/a/20601536 for neat approach.
#define DEBUG 0
#if DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT(...)
#endif

/*************************
    constants
**************************/

// Some constants to limit the max size of arguments and inputs.
#define MAX_ARGS 64
#define MAX_ARG_LENGTH 128
#define MAX_INPUT_LENGTH 256

// Define exit statuses and return values.
#define STATUS_EXIT 1661
#define STATUS_EXEC_FAIL 666
#define STATUS_INVALID_CMD -1
#define STATUS_TOO_MANY_ARGS 2

/*************************
    function prototypes
**************************/

// calloc_char_or_die returns a pointer to empty string of length @howlong or exits on failure.
char *calloc_char_or_die(int howlong);

// build_argv split from_string on spaces into char** array, storing pointer to array in out_result.
// Returns the number of arguments parsed, -1 on failure. Caller must free out_result.
// For example, build_argv("ls -ahlrt") will allocate the list ["ls", "-ahlrt", NULL].
int build_argv(char *from_string, char ***out_result);

// prompt prints the shell's prompt, displayed before each line of user input.
void prompt();

// print_args prints the args in parsed_args as numbered, newline-delimited list to stdout.
void print_args(char **parsed_args);

// free_args frees the memory allocated for each string in parsed_args and parsed_args itself.
void free_args(char **parsed_args);

// fork_exec_command forks the current process, running the command defined in argv in the child
// process using execvp. The parent waits for the child to complete and reports on the result.
// The method returns the return value of the child process or exits completely on failed fork.
int fork_exec_command(char **argv);

// process_command_from_argv processes the command at *argv with possible arguments.
// It implements two built-in commands, "exit" to quit and "cd" to change directories.
// Otherwise it forks and executes the command in a child process.
int process_command_from_argv(char **argv);

// trim_right trims a single to_trim char from the end of str, replacing with replace_with.
void trim_right(char *str, const char to_trim, const char replace_with);

// cycle represents a single "cycle" of the shell. A cycle has the following steps:
// 1) Fetch the user input
// 2) Parse the line
// 3) Execute the command
// 4) Outputs the result.
// Cycle uses the file pointer @in_file for input. This can be stdin or a file.
int cycle(FILE *in_file);

// usage prints a usage message on how to use the shell then exits 1.
void usage();

// get_input_file parses the command line arguments, printing usage if necessary
// due to an invalid number of arguments or the presence of "help" or "--help".
FILE *get_input_file(int argc, char **argv);

#endif // WOOSH_H_
