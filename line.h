#ifndef __LINE_H_
#define __LINE_H_
// ^ This is callled a guard. It makes sure that the C preprocesor doesn't
// include the header file twice.
typedef struct {
    char** arr;
    int size;
} ArrayWithLength;
//
ArrayWithLength* newLenArr( int, char**);
// Function declaration, this is used if doing imports from header files
ArrayWithLength* lenArr( int , char** );
// This function returns n array of character pointers
ArrayWithLength* lsh_split_line(char*);

#endif // __LINE_H_
