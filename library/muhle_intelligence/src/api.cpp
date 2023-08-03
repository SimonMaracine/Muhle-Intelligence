#include "muhle_intelligence/muhle_intelligence.hpp"
#include "muhle_intelligence/internal/implementation.hpp"

#ifdef _WIN32
    #define MUHLE_API __declspec(dllexport)
#else
    #define MUHLE_API
#endif

extern "C" {
    MUHLE_API muhle::MuhleIntelligence* muhle_intelligence_create() {
        return new muhle::MuhleImpl;
    }

    MUHLE_API void muhle_intelligence_destroy(muhle::MuhleIntelligence* instance) {
        delete instance;
    }

    MUHLE_API const char* muhle_intelligence_name() {
        return "a_bit_faster";
    }
}
