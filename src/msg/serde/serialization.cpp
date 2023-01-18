#include "msg/serde/serialization.h"
#include "msg/field_msg.h"
#include <sstream>
#include "msg/exceptions/msg_exceptions.h"

namespace msg
{
    namespace serde
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
            write<uint64_t>(ss, m.msg_bitset.to_ullong());

            auto temp_bitmap = m.msg_bitset >> 1;

            for (int i = 1; temp_bitmap.any() && i < fld_cnt; temp_bitmap >>= 1, i++)
            {
                if (!(temp_bitmap.test(0)))
                    continue;
                auto &curr = m.list[i];
                if (curr.is_type<int32_t>())
                    write<int32_t>(ss, curr.get<int32_t>());
                else
                    write<std::string>(ss, curr.get<std::string>());
            }

            return ss.str();
        }

        template <typename T>
        T read(std::stringstream &os)
        {
            T temp;
            os.read(reinterpret_cast<char *>(&temp), sizeof(T));
            if (!os.good())
                throw serde_error("error on reading from stream");
            return temp;
        }

        template <>
        std::string read<std::string>(std::stringstream &os)
        {
            uint16_t size = read<uint16_t>(os);
            std::string temp(size, 0);
            os.read(temp.begin().base(), size);
            if (!os.good())
                throw serde_error("error on reading from stream");
            return temp;
        }

        Message deser(const std::string str)
        {
            if (str.size() < Message::def_msg_size)
                throw  serde_error("the size is to small, no requared fields here") ;
            auto fld_cnt = static_cast<size_t>(Message::e_fields::field_cnt);
            std::stringstream ss;
            ss << str;

            Message new_msg;

            new_msg.msg_size = read<uint64_t>(ss);
            new_msg.msg_bitset = read<uint64_t>(ss);

            if (new_msg.msg_size != str.size())
                throw serde_error("the size of the message is wrong");

            auto temp_bitmap = new_msg.msg_bitset >> 1;

            for (int i = 1; temp_bitmap.any(); temp_bitmap >>= 1, i++)
            {
                if (!(temp_bitmap.test(0)))
                    continue;
                if (i >= fld_cnt)
                    throw serde_error("the bitmap field gives the wrong data");

                auto &curr = new_msg.list[i];

                msg::field_type type = static_cast<msg::field_type>(read<int8_t>(ss));

                if(type==msg::field_type::int32 && curr.is_type<int32_t>())
                    curr.set(read<int32_t>(ss));
                else if(type==msg::field_type::str && curr.is_type<std::string>())
                    curr.set(read<std::string>(ss));
                else throw serde_error("the type of the fields do not match");

                curr.set_active(true);
            }

            return new_msg;
        }

    template <typename T>
        size_t type_size(const T &) {
        return pre_type_info + sizeof(T);
    }

    template<>
    size_t type_size<std::string>(const std::string& val) {
        return pre_info_string+val.size();
    }

    template<>
    size_t type_size<int32_t>(const int32_t& val) {
        return pre_info_int32+sizeof(val);
    }

    template <>
        bool fit_max_size<std::string>(const std::string & val) {
        return val.size() <= UINT16_MAX;
    }

    template <>
        bool fit_max_size<int32_t>(const int32_t & val) {
        return true;
    }

    }

} // namespace msg