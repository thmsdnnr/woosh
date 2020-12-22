#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "woosh.h"

// calloc_char_or_die returns a pointer to empty string of length @howlong or exits on failure.
char *calloc_char_or_die(int howlong)
{
	if (howlong < 1)
		return NULL;
	char *result = calloc(howlong, sizeof(char));
	if (result == NULL)
	{
		perror("calloc_char_or_die");
		exit(EXIT_FAILURE);
	}
	return result;
}

// build_argv splits from_string on spaces into char** array, storing pointer to array in out_result.
// Returns the number of arguments parsed, -1 on failure. Caller must free out_result.
// For example, build_argv("ls -ahlrt") will allocate the list ["ls", "-ahlrt", NULL].
int build_argv(char *from_string, char ***out_result)
{
	if (from_string == NULL || out_result == NULL)
		return -1;
	char *save_ptr = NULL;
	char *this_piece = strtok_r(from_string, " ", &save_ptr);
	char **result = calloc(MAX_ARGS, sizeof(char *));
	if (result == NULL)
	{
		perror("build_argv");
		exit(EXIT_FAILURE);
	}
	char **res_head = result;

	while (this_piece != NULL)
	{
		if (strlen(this_piece) == 0)
			continue;
		*result = calloc_char_or_die(MAX_ARG_LENGTH);
		strncat(*result, this_piece, MAX_ARG_LENGTH - 1);
		result++;
		this_piece = strtok_r(NULL, " ", &save_ptr);
	}

	// assign the parsed result
	*out_result = res_head;
	// return the number of args parsed
	return (int)(result - res_head);
}

// print_args prints the args in parsed_args as numbered, newline-delimited list to stdout.
void print_args(char **parsed_args)
{
	char **cursor = parsed_args;
	int arg_ct = 0;
	while (*cursor != NULL)
	{
		printf("argv[%d]: %s\n", arg_ct, *cursor);
		arg_ct++;
		cursor++;
	}
}

// free_args frees the memory allocated for each string in parsed_args and parsed_args itself.
void free_args(char **parsed_args)
{
	char *tmp = NULL;
	char **cursor = parsed_args;
	while (*cursor != NULL)
	{
		tmp = *cursor;
		cursor++;
		free(tmp);
	}
	free(parsed_args);
}

// fork_exec_command forks the current process, running the command defined in argv in the child
// process using execvp. The parent waits for the child to complete and reports on the result.
// The method returns the return value of the child process or exits completely on failed fork.
int fork_exec_command(char **argv)
{
	// fork returns < 0 on error, 0 in the child process, and > 0 (the PID of the child) in the parent process
	int ret = fork();
	if (ret < 0)
	{
		perror("process_command_from_argv fork");
		return STATUS_FORK_FAIL;
	}
	else if (ret == 0)
	{
		DEBUG_PRINT("CHILD self pid: %d parent pid: %d\n", getpid(), getppid());
		execvp(*argv, argv);
		return STATUS_EXEC_FAIL; // exec only returns on failure
	}
	else
	{
		int exec_status;
		DEBUG_PRINT("PARENT self pid: %d child pid %d\n", getpid(), ret);
		waitpid(ret, &exec_status, 0);
		if (WIFEXITED(exec_status))
		{
			if (WEXITSTATUS(exec_status) != 0)
				printf("child exited %d\n", WEXITSTATUS(exec_status));
			return WEXITSTATUS(exec_status);
		}
		else if (WIFSIGNALED(exec_status))
		{
			printf("child caught signal %d\n", exec_status);
		}
		else
		{
			printf("child terminated abnormally %d\n", exec_status);
		}
		return exec_status;
	}
}

// process_command_from_argv processes the command at *argv with possible arguments.
// It implements three built-in commands, "exit" to quit, "cd" to change directories
// and "help" for help. Otherwise it forks and executes the command if it is found
// in the user's PATH, in a child process.
int process_command_from_argv(char **argv)
{
	if (argv == NULL || *argv == NULL)
		return STATUS_EXIT;

	int exit_with = 0;
	if (strncmp(*argv, "exit", 4) == 0)
	{
		exit_with = STATUS_EXIT;
	}
	else if (strncmp(*argv, "cd", 2) == 0)
	{
		if (chdir(*(argv + 1)) != 0)
		{
			perror("chdir");
			exit_with = EXIT_FAILURE;
		}
	}
	else if (strncmp(*argv, "help", 4) == 0)
	{
		usage();
	}
	else
	{
		exit_with = fork_exec_command(argv);
	}
	free_args(argv);
	return exit_with;
}

// trim_right trims a single to_trim char from the end of str, replacing with replace_with.
void trim_right(char *str, const char to_trim, const char replace_with)
{
	if (str == NULL)
		return;
	int length = strlen(str);
	if (length > 0 && str[length - 1] == to_trim)
	{
		str[length - 1] = replace_with;
	}
}

// cycle represents a single "cycle" of the shell. A cycle has the following steps:
// 1) Fetch the user input
// 2) Parse the line
// 3) Execute the command
// 4) Output the result
// Cycle uses the file pointer @in_file for input. This can be stdin or a file.
int cycle(FILE *in_file)
{
	// Only print a prompt if we're running in interactive mode.
	if (isatty(fileno(in_file)))
		prompt();

	// Fetch a line of input from the user.
	char buf[MAX_INPUT_LENGTH] = {0};
	if (fgets(buf, MAX_INPUT_LENGTH, in_file) != NULL)
	{
		if (feof(in_file))
		{
			return STATUS_EXIT;
		}
		else if (ferror(in_file))
		{
			perror("fgets");
			exit(EXIT_FAILURE);
		}
	}

	// Trim any trailing newline, then run the command.
	trim_right(buf, '\n', '\0');

	char **parsed_args = NULL;
	if (build_argv(buf, &parsed_args) > MAX_ARGS)
	{
		// Warn the user they entered too many args.
		printf("Number of arguments provided greater than MAX_ARGS %d", MAX_ARGS);
		free_args(parsed_args);
		return STATUS_TOO_MANY_ARGS;
	}

	return process_command_from_argv(parsed_args);
}

// get_input_file_or_die fetches the input file for the shell, either stdin or the command
// file given in command line arguments. Prints usage if invoked with "help" or "--help".
FILE *get_input_file_or_die(int argc, char **argv)
{
	FILE *fd_to_return = stdin;
	DEBUG_PRINT("woosh invoked, arg ct: %d\n", argc);
	if (DEBUG)
		print_args(argv);
	if (argc == 2)
	{
		if (strncmp(*(argv + 1), "help", 4) == 0 || strncmp(*(argv + 1), "--help", 6) == 0)
		{
			usage();
		}
		// argv+1 is a file we're being asked to open.
		if ((fd_to_return = fopen(*(argv + 1), "r")) == NULL)
		{
			perror("get_input_file");
			exit(EXIT_FAILURE);
		}
	}
	return fd_to_return;
}

// prompt prints the shell's prompt, displayed before each line of user input.
void prompt()
{
	printf("=^.^= $ ");
}

// usage prints the usage instructions for woosh.
void usage()
{
	const char *woosh = "\
                                    __\n\
 _      __  ____   ____    _____   / /_\n\
| | /| / / / __ \\ / __ \\  / ___/  / __ \\\n\
| |/ |/ / / /_/ // /_/ / (__  )  / / / / =^.^=\n\
|__/|__/  \\____/ \\____/ /____/  /_/ /_/  v%s\n\n";
	char banner[strlen(woosh) + strlen(WOOSH_VERSION) + 1];
	if (sprintf(banner, woosh, WOOSH_VERSION) < 0)
	{
		perror("usage");
		exit(EXIT_FAILURE);
	}
	printf("%s", banner);
	printf("woosh [no arguments]\n\tRuns woosh in interactive mode.\n\tEnter `exit` command or press Ctrl+C to quit.\n\n");
	printf("woosh ./path/to/woosh-script [optional argument]\n\twoosh-script is a newline-delimited file \
of woosh commands.\n\twoosh runs every line, exiting upon the first error.\n\n");
	exit(EXIT_FAILURE);
}
