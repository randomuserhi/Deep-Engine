/**
 * ECDB
 */

#include <Deep/Entity/ECDB.h>
#include <Deep/Memory.h>

namespace Deep {
    ECDB::ECDB(ECRegistry* registry) :
        registry(registry) {}
} // namespace Deep

namespace Deep {
    ECDB::Archetype::Archetype(ECDB* database) :
        database(database) {}

    ECDB::Archetype::~Archetype() {
        while (chunks != nullptr) {
            Archetype::Chunk* temp = chunks;
            chunks = chunks->next;
            Free(chunks);
        }

        firstFree = nullptr;
    }
} // namespace Deep