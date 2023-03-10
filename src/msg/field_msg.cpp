#include <random>
#include <stdexcept>
#include <sstream>
#include <bitset>
#include <ctime>
#include "msg/field_msg.h"
#include "msg/exceptions/msg_exceptions.h"

namespace msg
{

    Message::Message()
        : msg_bitset(0b11),
          msg_size(def_msg_size) // default size 8 message_size 8 bitmask and 5 for corrId
    {
        static std::mt19937 mt(std::time(nullptr));
        std::uniform_int_distribution<int32_t> dist(INT32_MIN, INT32_MAX);

        list[e_fields::message_size] = {0, field_type::int32, true}; // dont use it anyway, msg size variable is more comfortable
        list[e_fields::corrId] = {dist(mt), field_type::int32, true};
        list[e_fields::msg_to_player] = {"", field_type::str, false};
        list[e_fields::name_of_main_block] = {"", field_type::str, false};
        list[e_fields::enemy_count] = {0, field_type::int32, false};
        list[e_fields::bullet_speed] = {0, field_type::int32, false};
        list[e_fields::field_cnt] = {static_cast<int32_t>(e_fields::field_cnt), field_type::int32, true};
    }

    size_t Message::cast(e_fields fld)
    {
        return static_cast<size_t>(fld);
    }

    bool Message::is_constant(e_fields fld)
    {
        return fld == e_fields::message_size ||
               fld == e_fields::corrId ||
               fld == e_fields::field_cnt;
    }

    void Message::throw_on_constant(e_fields fld)
    {
        if (is_constant(fld))
            throw immutable_error("this value cant be modified");
    }

    size_t Message::fld_el::size()
    {
        auto &curr = *this;
        if (curr.type == field_type::int32)
            return sizeof(int32_t);
        return std::get<std::string>(curr.value).size();
    }

    template <typename T>
    bool Message::fld_el::is_type()
    {
        return std::holds_alternative<T>(value);
    }

    void Message::set(e_fields fld, const std::string &val)
    {
        throw_on_constant(fld);
        if (val.size() >= UINT16_MAX)
            throw length_error("string size cant be more or eq UINT16_MAX");
        auto &curr = list[fld];
        int size_before_assign = curr.size();
        if (curr.type == field_type::str)
            curr.value = val;
        else
            throw type_error("cant set string for int field");
        if (curr.active)
        {
            msg_size -= size_before_assign + 3; // adjust message size value for new size
        }
        curr.active = true;
        msg_bitset.set(cast(fld), true);
        msg_size += curr.size() + 3;
    }

    void Message::set(e_fields fld, int val)
    {
        throw_on_constant(fld);
        auto &curr = list[fld];
        if (curr.type == field_type::int32)
            curr.value = val;
        else
            throw type_error("cant set int for string field");
        if (curr.active)
            return;
        curr.active = true;
        msg_bitset.set(cast(fld), true);
        msg_size += curr.size() + 1;
    }

    void Message::del(e_fields fld)
    {
        throw_on_constant(fld);
        auto &curr = list[fld];
        if (!curr.active)
            return;
        curr.active = false;
        msg_bitset.set(cast(fld), false);
        msg_size -= curr.size() + (curr.type == field_type::int32 ? 1 : 3); // reduce msg_size value according to type
    }

    bool Message::has(e_fields fld) const
    {
        auto &curr = list[fld];
        return curr.active;
    }

    template <>
    std::string Message::get(e_fields fld) const
    {
        const fld_el &curr = list[fld];
        if (curr.type == field_type::str)
            return std::get<std::string>(curr.value);
        throw type_error("expected int, got string request");
    }

    template <>
    int32_t Message::get(e_fields fld) const
    {
        if (fld == e_fields::message_size)
            return msg_size;
        const fld_el &curr = list[fld];
        if (curr.type == field_type::int32)
            return std::get<int>(curr.value);
        throw type_error("expected string, got int request");
    }

}