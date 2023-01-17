#include "serialization.h"
#include "field_msg.h"
#include <sstream>

namespace msg
{
    namespace deser
    {
        template <typename T>
        auto write(std::stringstream &os, T value) -> std::size_t
        {
            os.write(reinterpret_cast<const char *>(&value), sizeof(value));
            return sizeof(value);
        }

        template <>
        auto write<int32_t>(std::stringstream &os, int32_t value) -> std::size_t
        {
            write<uint8_t>(os, static_cast<uint8_t>(field_type::int32));
            os.write(reinterpret_cast<const char *>(&value), sizeof(value));
            return sizeof(value);
        }

        template <>
        auto write<std::string>(std::stringstream &os, std::string value) -> std::size_t
        {
            write<uint8_t>(os, static_cast<uint8_t>(field_type::str));
            write<uint16_t>(os, static_cast<uint16_t>(value.size()));
            os.write(value.c_str(), value.length());
            return value.length();
        }

        std::string ser(const Message &m)
        {
            std::stringstream ss;
            auto fld_cnt = static_cast<size_t>(Message::e_fields::field_cnt);

            write<uint64_t>(ss, m.msg_size);
            write<uint64_t>(ss, m.msg_bitmap);

            auto temp_bitmap = m.msg_bitmap >> 1;

            for (int i = 1; temp_bitmap && i < fld_cnt; temp_bitmap >>= 1, i++)
            {
                if (!(temp_bitmap & 1))
                    continue;
                auto &curr = m.els[i];
                if (curr.type == field_type::int32)
                    write<int32_t>(ss, std::get<int32_t>(curr.value));
                else
                    write<std::string>(ss, std::get<std::string>(curr.value));
            }

            return ss.str();
        }

        template <typename T>
        T read(std::stringstream &os)
        {
            T temp;
            os.read(reinterpret_cast<char *>(&temp), sizeof(T));
            if (!os.good())
                throw "error on reading from stream";
            return temp;
        }

        template <>
        std::string read<std::string>(std::stringstream &os)
        {
            uint16_t size = read<uint16_t>(os);
            std::string temp(size, 0);
            os.read(temp.begin().base(), size);
            if (!os.good())
                throw "error on reading from stream";
            return temp;
        }

        Message deser(const std::string str)
        {
            if (str.size() < Message::def_msg_size)
                throw  "the size is to small, no requared fields here" ;
            auto fld_cnt = static_cast<size_t>(Message::e_fields::field_cnt);
            std::stringstream ss;
            ss << str;

            Message new_msg;

            new_msg.msg_size = read<uint64_t>(ss);
            new_msg.msg_bitmap = read<uint64_t>(ss);

            if (new_msg.msg_size != str.size())
                throw "the size of the message is wrong";

            auto temp_bitmap = new_msg.msg_bitmap >> 1;

            for (int i = 1; temp_bitmap; temp_bitmap >>= 1, i++)
            {
                if (!(temp_bitmap & 1))
                    continue;
                if (i >= fld_cnt)
                    throw "the bitmap field gives the wrong data";

                auto &curr = new_msg.els[i];
                auto &exp_type = curr.type;

                unsigned char type = read<int8_t>(ss);

                if (static_cast<int8_t>(exp_type) != type)
                    throw "the type of the fields do not match";

                if (exp_type == field_type::int32)
                    curr.value = read<int32_t>(ss);
                else
                    curr.value = read<std::string>(ss);
                
                curr.active = true;
            }

            return new_msg;
        }

    }
} // namespace msg