#include "../pch.h"
#include <Deep.h>
#include <Deep/Entity/ECDB.h>

// TODO(randomuserhi): Split this test up
TEST(ECDB, Entity) {
    Deep::ECRegistry registry;
    Deep::ECDB database{ &registry };

    Deep::Ent ent = database.Entity();
}