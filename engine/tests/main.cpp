#include <gtest/gtest.h>
#include <muhle_intelligence.h>

TEST(muhle_intelligence, initialization) {
    ASSERT_EQ(muhle_intelligence_initialize(), MUHLE_INTELLIGENCE_SUCCESS);
    ASSERT_EQ(muhle_intelligence_uninitialize(), MUHLE_INTELLIGENCE_SUCCESS);

    ASSERT_EQ(muhle_intelligence_uninitialize(), MUHLE_INTELLIGENCE_ERROR);
    muhle_intelligence_initialize();
    ASSERT_EQ(muhle_intelligence_initialize(), MUHLE_INTELLIGENCE_ERROR);
}
