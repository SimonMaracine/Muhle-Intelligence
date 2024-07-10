#include <iostream>
#include <chrono>
#include <memory>

#include <gtest/gtest.h>
#include <muhle_intelligence.h>

TEST(muhle_intelligence, initialization) {
    ASSERT_EQ(muhle_intelligence_initialize(), MUHLE_INTELLIGENCE_SUCCESS);
    ASSERT_EQ(muhle_intelligence_uninitialize(), MUHLE_INTELLIGENCE_SUCCESS);

    ASSERT_EQ(muhle_intelligence_uninitialize(), MUHLE_INTELLIGENCE_ERROR);
    muhle_intelligence_initialize();
    ASSERT_EQ(muhle_intelligence_initialize(), MUHLE_INTELLIGENCE_ERROR);
}

TEST(muhle_intelligence, receive) {
    muhle_intelligence_initialize();

    unsigned int size {};
    ASSERT_EQ(muhle_intelligence_receive_size(&size), MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE);

    char buffer[8];
    ASSERT_EQ(muhle_intelligence_receive(buffer), MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE);

    muhle_intelligence_uninitialize();

    ASSERT_EQ(muhle_intelligence_receive_size(&size), MUHLE_INTELLIGENCE_ERROR);
    ASSERT_EQ(muhle_intelligence_receive(buffer), MUHLE_INTELLIGENCE_ERROR);
}

TEST(muhle_intelligence, send_receive_correct) {
    muhle_intelligence_initialize();

    ASSERT_EQ(muhle_intelligence_send("\n"), MUHLE_INTELLIGENCE_SUCCESS);

    ASSERT_EQ(muhle_intelligence_send("init\n"), MUHLE_INTELLIGENCE_SUCCESS);

    ASSERT_EQ(muhle_intelligence_send("go\n"), MUHLE_INTELLIGENCE_SUCCESS);

    unsigned int size {};

    {
        using namespace std::chrono_literals;

        const auto start {std::chrono::steady_clock::now()};

        while (true) {
            const auto now {std::chrono::steady_clock::now()};

            if (now - start > 5s) {
                ASSERT_TRUE(false);
            }

            const int result {muhle_intelligence_receive_size(&size)};

            if (result == MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE) {
                ASSERT_EQ(size, 0);
                continue;
            }

            if (result == MUHLE_INTELLIGENCE_ERROR) {
                ASSERT_TRUE(false);
            }

            ASSERT_GT(size, 0);

            break;
        }
    }

    std::unique_ptr<char[]> buffer {std::make_unique<char[]>(size)};
    ASSERT_EQ(muhle_intelligence_receive(buffer.get()), MUHLE_INTELLIGENCE_SUCCESS);

    std::cout << "size: " << size << "  buffer: `" << buffer.get() << "`\n";

    ASSERT_EQ(muhle_intelligence_send("quit\n"), MUHLE_INTELLIGENCE_SUCCESS);

    muhle_intelligence_uninitialize();
}

TEST(muhle_intelligence, send_wrong) {
    muhle_intelligence_initialize();

    ASSERT_EQ(muhle_intelligence_send("whatever\n"), MUHLE_INTELLIGENCE_ERROR);

    ASSERT_EQ(muhle_intelligence_send("quit\n"), MUHLE_INTELLIGENCE_SUCCESS);

    muhle_intelligence_uninitialize();
}
