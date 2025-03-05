#pragma once

#include "../../Deep.h"
#include "ECRegistry.h"

namespace Deep {
    class ECDB {
    public:
        ECDB(ECRegistry* registry);

    private:
        ECRegistry* const registry;
    };
} // namespace Deep