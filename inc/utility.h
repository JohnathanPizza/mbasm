#ifndef UTILITY_H
#define UTILITY_H

#include <stdbool.h>
#include <stddef.h>
#include "types.h"
#include "list.h"

// macro function for testing if a piece type counts as the end of an expression
#define IS_EXPR_END(t) ((t) == PT_LINE || (t) == PT_EXPR_DELIM)

extern struct FileData* filesArray;
struct FileData newFileData(const char* name);

extern unsigned char* memImage;
extern size_t memIdx;

//extern struct List stringCharsList;

// helper function to quickly get pointer to string in stringCharsList

//int strToInt(const char in[], int len);

/*
 * parses a string from array in with string length len
 * returns a nonnegative integer on success
 * returns a negative value on failure
 * can fail from incorrect characters/values used in the number
 * works for base 2, 8, 10, and 16 with, respectively, 0BN, 0N, N, and 0XN where N is the number in each base ("0B" and "0X" return 0)
 */
extern int fssize;
bool evalExpression(struct Piece p[], int* res);

/*
 * evaluate the expression from pieces from array p and store the result in *res
 * returns false on failure/true on success
 * failure can result from presently undefined values and is not always arithmetic related
 */

int exprArrayLen(const struct Piece p[]);

/*
 * return the total number of pieces in an expression from p
 * the number returned is inclusive and counts the delimiting piece too
 * returns a positive number if the expression did NOT end from a line end, negative if it did
 * the function will not return 0; overflow resulting in a would be 0 return value gives 1
 */

char* printExpr(struct Piece p[]);

/*
 * returns a string for the expression from array p
 * the returned array is statically allocated within the function
 * different formatting for different piece types
 * the returned string is nul-terminated
 */

bool commandHandler(struct Piece in[], struct FileData* f);

/*
 * NOT defined in utility.c
 * processes a command from a line of input from array in
 * returns true on success; false on failure
 */

//*
void createPieces(struct FileData* f);

/*
 * opens a file with filename, reads all of the contents and creates pieces, then closes the file
 * uses the global pieceList object to store generated pieces in
 * also adds characters to the global stringCharsList object for some pieces
 * does not delete contents in these lists on each call; subsequent calls add to the end
 * discriminates symbols based on an internal array of characters
 */

//*
int scanPieces(struct FileData* f);

/*
 * goes through and evaluates lines of pieces from the global pieceList list starting at index pieceStart
 * determines if lines are commands or instructions and prepares them and sends them to proper functions
 * also creates instructions and adds them to the memory image
 * returns 0 if successful, and nonzero to represent failure at a line number which it returns
 * the line number starts at 1 and is updated after an amount of pieces processed represents a line
 */

struct Piece* getInsLine(struct Piece p[], struct Instruction* out, struct FileData* f);

/*
 * forms an instruction from an instruction line from array p and stores resulting instruction in *out
 * returns the last piece that was processed which is the piece for the EOL
 * returns NULL on failure
 */

#endif
