#include <gtest/gtest.h>
#include "msg/field_msg.h"
#include <string.h>

using msg::Message;

TEST(Message, TestValues)
{
    auto msg_size = 8+8+1+4;
    Message test;
    const char test_str[] = {"Test message"};
    test.set<std::string>(Message::e_fields::msg_to_player, test_str); 
    EXPECT_STREQ(test_str, test.get<std::string>(Message::e_fields::msg_to_player).c_str());
    msg_size+=3+sizeof(test_str)-1;
    EXPECT_EQ(test.get<int32_t>(Message::e_fields::message_size), msg_size);
    test.set<std::int32_t>(Message::e_fields::bullet_speed, 1337);
    EXPECT_EQ(test.get<int32_t>(Message::e_fields::bullet_speed), 1337);
    msg_size+=5;
    EXPECT_EQ(test.get<int32_t>(Message::e_fields::message_size), msg_size);
    test.del(Message::e_fields::msg_to_player);
    msg_size-=3+sizeof(test_str)-1;
    EXPECT_EQ(test.get<int32_t>(Message::e_fields::message_size), msg_size);
    EXPECT_TRUE(test.has(Message::e_fields::message_size));
    EXPECT_TRUE(test.has(Message::e_fields::bullet_speed));
    EXPECT_FALSE(test.has(Message::e_fields::msg_to_player));
}

TEST(Message, SERDE)
{
    Message test;
    const char test_str[] = {"Test message"};
    test.set<std::string>(Message::e_fields::msg_to_player, test_str);
    test.set<std::string>(Message::e_fields::name_of_main_block, test_str);
    test.set<int32_t>(Message::e_fields::enemy_count, 1);
    test.set<int32_t>(Message::e_fields::bullet_speed, 2);
    auto msg_size = 8+8+1+4 + (3+sizeof(test_str)-1)*2 + 5 + 5;
    EXPECT_EQ(test.get<int32_t>(Message::e_fields::message_size), msg_size);
    auto str = msg::serde::ser(test);
    Message test2 = msg::serde::deser(str);
    auto str1 = msg::serde::ser(test);
    auto str2 = msg::serde::ser(test2);
    EXPECT_STREQ(str1.c_str(), str2.c_str());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}