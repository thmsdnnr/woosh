# woosh: W[rite] O[ur] O[wn] SH[ell] 🐚🚀

> `woosh` is a fun project to learn the basics of the C programming language by writing yet another command shell, like bash, zsh and the like.
### try it out
`make clean all && ./woosh` and you're off!

`./woosh help` for help.

`make leakcheck` to run Valgrind to check for memory leaks.

`./test.sh` to run some tests.
## What Is This?

```
$ ./woosh help
                                    __
 _      __  ____   ____    _____   / /_
| | /| / / / __ \ / __ \  / ___/  / __ \
| |/ |/ / / /_/ // /_/ / (__  )  / / / / =^.^=
|__/|__/  \____/ \____/ /____/  /_/ /_/  v0.0.1

woosh [no arguments]
	Runs woosh in interactive mode.
	Enter `exit` command or press Ctrl+C to quit.

woosh ./path/to/woosh-script [optional argument]
	woosh-script is a newline-delimited file of woosh commands.
	woosh runs every line, exiting upon the first error.
```

`woosh` is much less fully-featured and not intended to become a replacement for a day-to-day shell. Features are only implemented if they serve a pedagogical purpose, not if they provide ergonomics for the user, as this is a learning project. It is written in [ANSI C99](https://en.wikipedia.org/wiki/C99) without any external libraries.

It should be reasonably portable to any Linux/Unix machines.

Just a few of the great lessons you can learn from writing a shell are:

0. What a process is
1. What are memory leaks?
2. What is a path?
3. What is gdb and why do I care?
4. The fork/exec model of spawning processes
5. What are streams and how do I implement input/output redirection?
6. What are Signals and Signal Handlers? How do we write Signal Handlers?
7. What does it mean for a function to be Async Signal-Safe? Which functions can and can't we use in a Signal Handler?
8. What is "reentrant"?
9. What is a pointer?
10. What is memory?
11. What is a system call?

etc.

Also, if you're not familiar with the C programming language, writing a shell is a great practical introduction to syntax, some of the standard library functions, and in particular pointers. You'll immediately see how they're useful in the context of parsing a line of input text to be fed to a command.

## why?

I thought it might be cool as I'm still learning myself to add details to this tutorial and make it a short introduction to "lower-level" or "systems" programming in an accessible way with a fun and motivating result. Writing your own shell that you can then use and run commands in is super fun. Maybe it can become a simple gitbook or blog post.

## this sounds hard

At first, writing your own shell might seem like a daunting task. Where to even begin? One of the key insights though is that while a shell might appear to be doing many things on its own, in reality it is just a shell or go-between for the user and the OS.

You could think of a shell in terms of a Client/Server architecture. The shell is a Client that makes requests to the Server (Operating System). The Operating System, in turn, is a Client that makes some requests to the Kernel (Server) which performs System Calls on the OS's behalf.

The interface for a shell could not be simpler. Text and input-output streams are the "Universal interface" that lies behind the great success of tools like `grep` and the entire command line interface. Small, sharp, composable tools with a uniform interface for input and output allows us to build larger programs from simple programs that do one thing well.

### Missing Features

#### Shell Features

- [ ] command pipelines `|`
- [ ] signal handling for processes
- [ ] variables and variables expansion (hashtable)
- [ ] implement `$?` to echo the last known return value
- [ ] foreground / background mode for processes with `&`
- [ ] input / output redirection between the fork and exec

#### Learning Resources

- [ ] Links to relevant chapters of [Operating Systems: Three Easy Pieces
      ](http://pages.cs.wisc.edu/~remzi/OSTEP/), an excellent free online Operating Systems textbook.
- [ ] Add some instructions for getting [Valgrind](https://valgrind.org/), [gdb](https://www.gnu.org/software/gdb/), and [gcc](https://gcc.gnu.org/) running if you don't have it locally, perhaps with the [official gcc Docker image](https://hub.docker.com/_/gcc).
- [ ] Describe how to use [gdbgui](https://www.gdbgui.com/).
