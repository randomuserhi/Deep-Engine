#ifndef h_Deep_String
#define h_Deep_String

#include "Deep.h"
#include "Deep_Containers.h"

#ifndef Deep_DynArray_Decl_char
#define Deep_DynArray_Decl_char
Deep_DynArray_Decl(char, char)
#endif

struct Deep_String
{
	struct Deep_DynArray(char) str;
};

Deep_Inline void Deep_String_Create(struct Deep_String* string, const char* str)
{
	Deep_DynArray_Create(char)(&string->str);
	size_t strLength = strlen(str) + 1;
	Deep_DynArray_Reserve(char)(&string->str, strLength);
	memcpy(string->str.values, str, strLength);
}

#endif

