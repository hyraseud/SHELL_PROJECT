/*
 * CS252: Shell project
 *
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "command.hh"
#include "shell.hh"

extern const char * uscore;
Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
    count = 0;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear() {
// deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands) {
        //printf("command: %s\n", &simpleCommand->_arguments.c_str());
        if ((simpleCommand))  {
	    delete simpleCommand;
        }
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();

    if ( _outFile ) {
        delete _outFile;
        _outFile = NULL;
    }

    else if ( _inFile ) {
        delete _inFile;
        _inFile = NULL;
    }

    else if ( _errFile ) {
        delete _errFile;
    }
    _errFile = NULL;

    _background = false;
    count = 0;
}

void Command::print() {
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simpleCommand : _simpleCommands ) {
        printf("  %-3d ", i++ );
        simpleCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO");
    printf( "\n\n" );
}

void Command::execute() {
    // Don't do anything if there are no simple commands
    if ( _simpleCommands.size() == 0 ) {
        if (isatty(0)) Shell::prompt();
        return;
    }
   
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec
    int default_in = dup(0);
    int default_out = dup(1);
    int default_err = dup(2);
    int fdin;
    if (_inFile)    {
        fdin = open(const_cast<char*> (Shell::_currentCommand._inFile->c_str()), O_RDONLY);
    }
    else {
        fdin = dup(default_in);
    }
    int fdout;
    int fderr;
    int count = 0;
    pid_t child;
    for (auto & simpleCommand : _simpleCommands) {
        count++;
        //redirect input
        dup2(fdin, 0);
        close(fdin);
        if(count == (int)_simpleCommands.size()) {
            //printf("%d", simpleCommand->find);
            if (_outFile)   {
                if (simpleCommand->find == 1)   {
                    //printf("%d", simpleCommand->find);
                    //printf("here");
		    fdout = open(const_cast<char*> (Shell::_currentCommand._outFile->c_str()), O_CREAT|O_WRONLY|O_APPEND, 0664);
		//printf("%d", fdout);
		}
                else {
		    //printf("here");
                    fdout = open(const_cast<char*> (Shell::_currentCommand._outFile->c_str()), O_CREAT|O_WRONLY|O_TRUNC, 0664);
		    //printf("%d", fdout);
                }
            }
            else {
                fdout = dup(default_out);
            }
            if (_errFile)   {
                if (simpleCommand->find == 1)   {
                    fderr = open(const_cast<char*> (Shell::_currentCommand._errFile->c_str()), O_RDWR|O_CREAT|O_APPEND, 0664);
                }
                else {
                    fderr = open(const_cast<char*> (Shell::_currentCommand._errFile->c_str()), O_WRONLY|O_CREAT|O_TRUNC, 0664);
                }
            }
            else {
                fderr = dup(default_err);
            }
        }
        else {
            int fdpipe[2];
            pipe(fdpipe);
            fdout=fdpipe[1];
            fdin=fdpipe[0];
        }
        dup2(fdout, 1);
        dup2(fderr, 2);
        close(fdout);
        close(fderr);
	size_t size = simpleCommand->_arguments.size();
        char ** args= new char* [size + 1];
        for (size_t i = 0; i < size; i++)
        {
            args[i] = const_cast<char*> (simpleCommand->_arguments[i]->c_str());
            //printf("%d, %s\n", i, args[i]);
	    uscore = args[i];
        }
        args[size] = NULL; //assign last place value to null

	    
	//check env variables
        //check for setenv and unsetenv
        if ( !strcmp( args[0], "setenv" ) ) {
            //printf("1: %s 2: %s\n", args[1], args[2]);
            setenv(args[1], args[2], 1);
        }
        if ( !strcmp( args[0], "unsetenv" ) ) {
            unsetenv(args[1]);
        }

        if ( !strcmp( args[0], "cd" ) ) {
            if (size == 1) chdir(getenv("HOME"));

	    else if ( !strcmp ( args[1], "${HOME}") ) {
		chdir(getenv("HOME"));
            }
            else if (chdir(args[1]) == 0) {
                chdir(args[1]);
            }
            else {
                perror("cd: can't cd to notfound");
                continue;
            }
        }
	
        extern char ** environ;
        child = fork();

        if (child < 0)    {
            //perror("fork");
            exit(2);
        }
        else if (child == 0) {
	    //printf("child: %s\n", args[1]);	
	    if (!strcmp(args[0], "printenv")) {
                //printf("debug");
                char **p = environ;
                while (*p != NULL)  {
                    printf("%s\n", *p);
                    p++;
                }
                fflush(stdout);
                //setenv("$", 0, 1);
                exit(0);
            }
            else {
            //printf("%s args", args[0]);
            int ex = execvp(args[0], args);
                if (ex < 0)   {
                //perror("execvp");
                exit(1);
            }
           // setenv("$", "2", 1);
            }

        }
        //delete args;
    }

    //reset
    dup2(default_in, 0);
    dup2(default_out, 1);
    dup2(default_err, 2);
    close(default_in);
    close(default_out);
    close(default_err);

    if (!_background)   {
       waitpid(child, NULL, 0);
    }
    // Clear to prepare for next command
    clear();

    // Print new prompt
    if(isatty(0)) Shell::prompt();
}


SimpleCommand * Command::_currentSimpleCommand;
