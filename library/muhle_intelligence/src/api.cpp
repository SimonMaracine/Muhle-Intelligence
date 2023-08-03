#include "muhle_intelligence/muhle_intelligence.hpp"
#include "muhle_intelligence/internal/implementation.hpp"

extern "C" {
    muhle::MuhleIntelligence* muhle_intelligence_create() {
        return new muhle::MuhleImpl;
    }

    void muhle_intelligence_destroy(muhle::MuhleIntelligence* instance) {
        delete instance;
    }

    const char* muhle_intelligence_name() {
        return "old_fixed";
    }
}
