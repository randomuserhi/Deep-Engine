#pragma once

#include "../../Deep.h"

#include "../NonCopyable.h"

#include <vector>

namespace Deep {
    struct ArchetypeBitField final : private NonCopyable {
    public:
        ArchetypeBitField();

    private:
        std::vector<int64> bits;
    };
} // namespace Deep