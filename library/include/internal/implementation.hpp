#pragma once

#include "muhle_intelligence.hpp"

namespace muhle {
    struct MuhleImpl : public MuhleIntelligence {
        virtual void search(Position position, Player turn, Result& result) override;
    };
}
