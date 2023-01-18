#pragma once

#include <variant>
#include <string>
#include <sstream>
#include <algorithm>
#include <msg/serde/serialization.h>
#include <msg/exceptions/msg_exceptions.h>
#include <bitset>
#include "field.h"

namespace msg
{

    class Message
    {
    public:
        enum class e_fields
        {
            msg_to_player,
            name_of_main_block,
            enemy_count,
            bullet_speed,
            field_cnt
        };

        Message();

        void del(e_fields);
        bool has(e_fields) const;

        template <typename T>
        T get(e_fields fld) const
        {
            const auto &curr = list[fld];
            if (curr.is_type<T>())
                return curr.get<T>();
            throw type_error("type mismatch on get");
        }

        template <typename T>
        void set(e_fields fld, T val)
        {
            {
                throw_on_constant(fld);
                if (!serde::fit_max_size<T>(val))
                    throw length_error("the max size error");
                auto &curr = list[fld];
                int size_before_assign = serde::type_size<T>(val);
                if (!curr.is_type<T>())
                    throw type_error("the field has deferent type");

                curr.set(val);

                if (curr.is_avaliable())
                    msg_size -= size_before_assign; // adjust message size value for new size

                curr.set_active(true);
                msg_bitset.set(cast(fld), true);
                msg_size += curr.size();
            }
        }

        size_t get_msg_size() const;
        uint64_t get_msg_bitset() const;
        int32_t get_msg_id() const;

    private:
        static size_t cast(e_fields);
        static void throw_on_constant(e_fields);

        uint64_t msg_size;
        std::bitset<64> msg_bitset;
        int32_t msg_id;
        // msg size, bitset, 1(type) + unique msg id
        static const int def_msg_size = sizeof(uint64_t) + sizeof(uint64_t) + 1 + sizeof(int32_t);

        fld_list<static_cast<size_t>(e_fields::field_cnt) + 1, fld_el<int32_t, std::string>> list;

        friend std::string serde::ser(const Message &);
        friend Message serde::deser(std::string);
    };

} // namespace msg
