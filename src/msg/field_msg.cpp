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
        : msg_bitset(0),
          msg_size(def_msg_size) // default size 8 message_size 8 bitmask and 5 for corrId
    {
        static std::mt19937 mt(std::time(nullptr));
        std::uniform_int_distribution<int32_t> dist(INT32_MIN, INT32_MAX);
        msg_id = dist(mt);
        list[e_fields::msg_to_player].assign<std::string>();
        list[e_fields::name_of_main_block].assign<std::string>();
        list[e_fields::enemy_count].assign<int32_t>();
        list[e_fields::bullet_speed].assign<int32_t>();
        list[e_fields::field_cnt].assign<int32_t>(static_cast<int32_t>(e_fields::field_cnt), true);
    }

    size_t Message::cast(e_fields fld)
    {
        return static_cast<size_t>(fld);
    }

    void Message::throw_on_constant(e_fields fld)
    {
        if (fld == e_fields::field_cnt)
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

    uint64_t Message::get_msg_bitset() const {
        return msg_bitset.to_ullong();
    }

    size_t Message::get_msg_size() const {
        return msg_size;
    }

    int32_t Message::get_msg_id() const {
        return msg_id;
    }

}