#include "Deep_Debug_Memory_Undef.h"

#define malloc(size) Deep::Malloc(size, Deep__File__, Deep__Line__, Deep__Function__)
#define calloc(count, typeSize) Deep::Calloc(count, typeSize, Deep__File__, Deep__Line__, Deep__Function__)
#define realloc(ptr, size) Deep::Realloc(ptr, size, Deep__File__, Deep__Line__, Deep__Function__)
#define free(ptr) Deep::Free(ptr, Deep__File__, Deep__Line__, Deep__Function__)

#define new new(Deep__File__, Deep__Line__, Deep__Function__)
#define delete \
Deep::DEBUG_MEMORY__SET__ = true; \
Deep::DEBUG_MEMORY__FILE__ = Deep__File__; \
Deep::DEBUG_MEMORY__FUNCTION__ = Deep__Function__; \
Deep::DEBUG_MEMORY__LINE__ = Deep__Line__; \
delete
