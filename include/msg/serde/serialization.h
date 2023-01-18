#pragma once

#include <string>

namespace msg
{
    class Message;

    enum class field_type : int8_t
    {
        str = 1 << 0,
        int32 = 1 << 1,
    };

    namespace serde
    {
        const size_t pre_type_info = 1;
        const size_t pre_info_string = pre_type_info + 2;
        const size_t pre_info_int32 = pre_type_info;

        std::string ser(const Message &);
        Message deser(const std::string str);

        template <typename T>
        size_t type_size(const T &);

        template <typename T>
        bool fit_max_size(const T &);
    }
} // namespace msg
