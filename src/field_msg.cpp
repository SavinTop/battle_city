#include <field_msg.h>
#include <random>
#include <stdexcept>
#include <sstream>
#include <bitset>
#include <ctime>

namespace msg{

Message::Message()
    : msg_bitmap(0b11),
      msg_size(def_msg_size) // default size 8 message_size 8 bitmask and 5 for corrId
{
    static std::mt19937 mt(std::time(nullptr));
    std::uniform_int_distribution<int32_t> dist(INT32_MIN, INT32_MAX);

    (*this)[e_fields::message_size] = {0, field_type::int32, true};  // dont use it anyway, msg size variable is more comfortable
    (*this)[e_fields::corrId] = {dist(mt), field_type::int32, true}; 
    (*this)[e_fields::msg_to_player] = {"", field_type::str, false};
    (*this)[e_fields::name_of_main_block] = {"", field_type::str, false};
    (*this)[e_fields::enemy_count] = {0, field_type::int32, false};
    (*this)[e_fields::bullet_speed] = {0, field_type::int32, false};
    (*this)[e_fields::field_cnt] = {cast(e_fields::field_cnt), field_type::int32, true};
}

const Message::fld_el &Message::operator[](e_fields fld) const
{
    return els[cast(fld)];
}

Message::fld_el &Message::operator[](e_fields fld)
{
    return els[cast(fld)];
}

template <>
void Message::set_bit<false>(int ind)
{
    msg_bitmap &= ~(static_cast<int64_t>(1) << ind);
}

template <>
void Message::set_bit<true>(int ind)
{
    msg_bitmap |= static_cast<int64_t>(1) << ind;
}

int Message::cast(e_fields fld)
{
    return static_cast<int>(fld);
}

void Message::_throw(const char *str)
{
    throw std::invalid_argument(str);
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
        _throw("this value cant be modified");
}

size_t Message::fld_el::size()
{
    auto &curr = *this;
    if (curr.type == field_type::int32)
        return sizeof(int32_t);
    return std::get<std::string>(curr.value).size();
}

void Message::set(e_fields fld, const std::string &val)
{
    throw_on_constant(fld);
    if (val.size() >= UINT16_MAX)
        _throw("string size cant be more or eq UINT16_MAX");
    auto &curr = (*this)[fld];
    int size_before_assign = curr.size();
    if (curr.type == field_type::str)
        curr.value = val;
    else
        _throw("cant set string for int field");
    if (curr.active)
    {
        msg_size -= size_before_assign + 3; // adjust message size value for new size
    }
    curr.active = true;
    set_bit<true>(cast(fld));
    msg_size += curr.size() + 3;
}

void Message::set(e_fields fld, int val)
{
    throw_on_constant(fld);
    auto &curr = (*this)[fld];
    if (curr.type == field_type::int32)
        curr.value = val;
    else
        _throw("cant set int for string field");
    if (curr.active)
        return;
    curr.active = true;
    set_bit<true>(cast(fld));
    msg_size += curr.size() + 1;
}

void Message::del(e_fields fld)
{
    throw_on_constant(fld);
    auto &curr = (*this)[fld];
    if (!curr.active)
        return;
    curr.active = false;
    set_bit<false>(cast(fld));
    msg_size -= curr.size() + (curr.type == field_type::int32 ? 1 : 3); // reduce msg_size value according to type
}

bool Message::has(e_fields fld) const
{
    auto &curr = (*this)[fld];
    return curr.active;
}

int Message::get_int(e_fields fld) const
{
    if (fld == e_fields::message_size)
        return msg_size;
    const fld_el &curr = (*this)[fld];
    if (curr.type == field_type::int32)
        return std::get<int>(curr.value);
    _throw("expected string, got int request");
}

const std::string &Message::get_str(e_fields fld) const
{
    const fld_el &curr = (*this)[fld];
    if (curr.type == field_type::str)
        return std::get<std::string>(curr.value);
    _throw("expected int, got string request");
}

}