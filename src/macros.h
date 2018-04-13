#ifndef CLUE_MACROS_H
#define CLUE_MACROS_H
// macros.h

#define CAT(a,b) a##b

#define swap(a,b,T) do{T temp = a; a = b; b = temp;}while(0)

#define cp(source, dest, size) for(int i=0; i<size; i++) dest[i] = source[i]

#define isdigit(x) (x>-1 && x<10)

#endif /* end of include guard: CLUE_MACROS_H */
