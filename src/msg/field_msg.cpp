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

        list[e_fields::message_size].assign<int32_t>(0, true);//{0, field_type::int32, true}; // dont use it anyway, msg size variable is more comfortable
        list[e_fields::corrId].assign<int32_t>(dist(mt), true);//{dist(mt), field_type::int32, true};
        list[e_fields::msg_to_player].assign<std::string>();//{"", field_type::str, false};
        list[e_fields::name_of_main_block].assign<std::string>();//{"", field_type::str, false};
        list[e_fields::enemy_count].assign<int32_t>();//{0, field_type::int32, false};
        list[e_fields::bullet_speed].assign<int32_t>();//{0, field_type::int32, false};
        list[e_fields::field_cnt].assign<int32_t>(static_cast<int32_t>(e_fields::field_cnt), true);//{static_cast<int32_t>(e_fields::field_cnt), field_type::int32, true};
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

    void Message::del(e_fields fld)
    {
        throw_on_constant(fld);
        auto &curr = list[fld];
        if (!curr.is_avaliable())
            return;
        curr.set_active(false);
        msg_bitset.set(cast(fld), false);
        msg_size -= curr.size();
    }

    bool Message::has(e_fields fld) const
    {
        auto &curr = list[fld];
        return curr.is_avaliable();
    }

    template <>
    std::string Message::get(e_fields fld) const
    {
        const auto &curr = list[fld];
        if (curr.is_type<std::string>())
            return curr.get<std::string>();
        throw type_error("expected int, got string request");
    }

    template <>
    int32_t Message::get(e_fields fld) const
    {
        if (fld == e_fields::message_size)
            return msg_size;
        const auto &curr = list[fld];
        if (curr.is_type<int32_t>())
            return curr.get<int32_t>();
        throw type_error("expected string, got int request");
    }

}