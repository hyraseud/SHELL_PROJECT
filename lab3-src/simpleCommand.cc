/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "simpleCommand.hh"

//extern int find;
int out_count;
SimpleCommand::SimpleCommand() {
  _arguments = std::vector<std::string *>();
  find = 0;
  out = 0;
}

SimpleCommand::~SimpleCommand() {
  // iterate over all the arguments and delete them
  find = 0;
  for (auto & arg : _arguments) {
     //printf("%s\n", arg->c_str());
     delete arg;
  }
}

void SimpleCommand::insertArgument( std::string * argument ) {
  // simply add the argument to the vector
  //printf("%s\n", argument->c_str());
  if ( !strcmp( argument->c_str(), "exit" ) ) {
     printf( "Good Bye!!\n");
     exit( 1 );
   }
  //if ( strcmp( argument->c_str(), "echo") ) {
  //uscore = argument->c_str();
  //}
  _arguments.push_back(argument);
}

// Print out the simple command
void SimpleCommand::print() {
  for (auto & arg : _arguments) {
    std::cout << "\"" << arg << "\" \t";
  // effectively the same as printf("\n\n");
  std::cout << std::endl;

  }
}
