#include "internal/implementation.hpp"

namespace muhle {
    struct MuhleIntelligence;
}

extern "C" {
    muhle::MuhleIntelligence* muhle_intelligence_create() {
        return new muhle::MuhleImpl;
    }

    void muhle_intelligence_destroy(muhle::MuhleIntelligence* instance) {
        delete instance;
    }
}
