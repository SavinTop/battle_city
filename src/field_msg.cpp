#include<field_msg.h>
#include<random>
#include<stdexcept>

FieldMessage::FieldMessage() 
: msg_bitmap(0b11),
  msg_size(8+8+5) //default size 8 message_size 8 bitmask and 5 for corrId
{
    (*this)[e_fields::message_size]       = {0, field_type::int32, true}; // dont use it anyway, msg size variable is more comfortable
    (*this)[e_fields::corrId]             = {0, field_type::int32, true};     // replace with random value
    (*this)[e_fields::msg_to_player]      = {"", field_type::str, false}; 
    (*this)[e_fields::name_of_main_block] = {"", field_type::str, false};     
    (*this)[e_fields::enemy_count]        = {0, field_type::int32, false}; 
    (*this)[e_fields::bullet_speed]       = {0, field_type::int32, false};     
}

const FieldMessage::fld_el& FieldMessage::operator[](e_fields fld) const {
    return els[cast(fld)];
}

FieldMessage::fld_el &FieldMessage::operator[](e_fields fld)
{
    return els[cast(fld)];
}

template<>
void FieldMessage::set_bit<false>(int ind){
    msg_bitmap &= ~(static_cast<int64_t>(1) << ind);
}

template<>
void FieldMessage::set_bit<true>(int ind){
    msg_bitmap |= static_cast<int64_t>(1) << ind;
}

int FieldMessage::cast(e_fields fld) const{
    return static_cast<int>(fld);
}

bool FieldMessage::is_constant(e_fields fld){
    return fld == e_fields::message_size ||
           fld == e_fields::corrId ||
           fld == e_fields::field_cnt;
}

void FieldMessage::throw_on_constant(e_fields fld) {
    if(is_constant(fld)) throw std::invalid_argument("this value cant be modified");
}

size_t FieldMessage::fld_el::size()
{
    auto& curr = *this;
    if(curr.type == field_type::int32) return sizeof(int32_t);  
    return std::get<std::string>(curr.value).size();
}

void FieldMessage::set(e_fields fld, const std::string& val)
{
    throw_on_constant(fld);
    if(val.size()>=UINT16_MAX) throw std::invalid_argument("string size cant be more or eq UINT16_MAX");
    auto& curr = (*this)[fld];
    int size_before_assign = curr.size();
    if(curr.type==field_type::str) curr.value = val;
    else throw std::invalid_argument("cant set string for int field");
    if(curr.active) {
        msg_size-=size_before_assign+3; // adjust message size value for new size
    }
    set_bit<true>(cast(fld));
    msg_size+=curr.size()+3;
}

void FieldMessage::set(e_fields fld, int val)
{
    throw_on_constant(fld);
    auto& curr = (*this)[fld];
    if(curr.type==field_type::int32) curr.value = val;
    else throw std::invalid_argument("cant set int for string field");
    if(curr.active) return;
    set_bit<true>(cast(fld));
    msg_size+=curr.size()+1;
}

void FieldMessage::del(e_fields fld)
{
    throw_on_constant(fld);
    auto& curr = (*this)[fld];
    if(!curr.active) return;
    curr.active = false;
    set_bit<false>(cast(fld));
    msg_size-=curr.size()+(curr.type==field_type::int32?1:3);
}

bool FieldMessage::has(e_fields fld) const
{
    auto& curr = (*this)[fld];
    return curr.active;
}

int FieldMessage::get_int(e_fields fld) const
{
    if(fld==e_fields::message_size) return msg_size;
    const fld_el& curr = (*this)[fld];
    if(curr.type==field_type::int32) return std::get<int>(curr.value);
    throw std::invalid_argument("expected string, got int request");
    return 0;
}

const std::string &FieldMessage::get_str(e_fields fld) const
{
    const fld_el& curr = (*this)[fld];
    if(curr.type==field_type::str) return std::get<std::string>(curr.value);
    throw std::invalid_argument("expected int, got string request");
}
