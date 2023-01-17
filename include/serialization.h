#pragma once

#include<string>

namespace msg
{
    class Message;

    enum class field_type : int8_t
    {
        str = 1 << 0,
        int32 = 1 << 1,
    };

    namespace deser{

        std::string ser(const Message&);
        Message deser(const std::string str);

    }
} // namespace msg
