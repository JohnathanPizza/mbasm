// basic functions used for error management

#ifndef ERROR_H
#define ERROR_H

#include "list.h"

// both defined in error.c
extern struct List errorList; // list used to store strings for errors
extern int errorLine; // variable to use to represent the line an error was found

// adds an error message formatted like printf with format string s to the message stack
// messages are printed in reverse order, so functions should add their error message before returning erros
// appends a newline to the end of the message
void addErrorMessage(const char* s, ...) __attribute__((format(printf, 1, 2)));

// logs an error formatted like printf with format string s to stderr and exits with EXIT_FAILURE
// appends a newline to the end of the message
void simpleError(const char* s, ...) __attribute__((format(printf, 1, 2), noreturn));

// macro function that passes args and executes simpleError if cond is true
#define testError(cond, ...) if(cond) simpleError(__VA_ARGS__)

// prints all the errors from last added to first added to stderr and exits with EXIT_FAILURE
void printErrorsExit(void) __attribute__((noreturn));

// clears the messages currently added
void clearErrors(void);

#endif
