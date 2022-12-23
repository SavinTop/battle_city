#include<field_msg.h>
#include<random>
#include<stdexcept>
#include<sstream>
#include<bitset>

FieldMessage::FieldMessage() 
: msg_bitmap(0b11) 
{
auto& message_size = msgs[get_index(Fields::message_size)];
message_size.__fld = Fields::message_size;
message_size.is_active = true;
auto& unique_id = msgs[get_index(Fields::corrId)];
message_size.__fld = Fields::corrId;
unique_id.is_active = true;
unique_id.i = 0xFF; // replace with random generator
}

int FieldMessage::calc_el_size(int index) const {
    auto& msg = msgs[index];
    auto type = get_def_type(msg.__fld);
    if(type & type_int){
            return sizeof(int32_t)+1;
    }else if(type & type_str){
            return 3 + msg.str.size();
    }

    return 0;
}

void FieldMessage::change_msg_size(int size) {
    auto& message_size = msgs[get_index(Fields::message_size)];
    message_size.i = size;
}

void FieldMessage::recalc_size() {
    uint64_t sum = 8+sizeof(msg_bitmap)+1+sizeof(int32_t);
    for(int i=1;i<get_index(Fields::field_cnt);i++)
    {
        auto& msg = msgs[i];
        if(!msg.is_active) continue;
        auto type = get_def_type(msg.__fld);
        sum+=calc_el_size(i);
    }
    change_msg_size(sum);
}

void FieldMessage::delete_all() {
    for(int i=2;i<get_index(Fields::field_cnt);i++)
    {
        auto& msg = msgs[i];
        if(msg.is_active) DeleteField(msg.__fld);
    }
}

uint64_t FieldMessage::cast(Fields fld) const {
    return static_cast<uint64_t>(fld);
}

uint64_t FieldMessage::get_def_type(Fields fld) const {
    return cast(fld) & ~(flush_additional);
}

uint64_t FieldMessage::get_index(Fields fld) const {
    return cast(fld) & flush_additional;
}

bool FieldMessage::can_modify(Fields fld) const {
    return !(cast(fld) & mod_const) || fld == Fields::field_cnt; 
}

void FieldMessage::set_msg_bmp_bit(int pos, bool val) {
    if(val)
        msg_bitmap |= static_cast<int64_t>(1)<<pos;
    else
        msg_bitmap &= ~(static_cast<int64_t>(1)<<pos);
}

bool FieldMessage::Has(Fields fld) const {
    auto index = get_index(fld);
    return msgs[index].is_active;
}

int FieldMessage::GetInt(Fields fld) const {
    auto index = get_index(fld);
    auto type = get_def_type(fld);
    auto& msg = msgs[index];
    if(type & type_str) throw std::invalid_argument("Wrong field type passed. Expected int field");
    if(!msg.is_active) throw std::invalid_argument("The field is none");
    return msgs[index].i;
}

std::string FieldMessage::GetString(Fields fld) const {
    auto index = get_index(fld);
    auto type = get_def_type(fld);
    auto& msg = msgs[index];
    if(type & type_int) throw std::invalid_argument("Wrong field type passed. Expected string field");
    if(!msg.is_active) throw std::invalid_argument("The field is none");
    return msgs[index].str;
}

void FieldMessage::DeleteField(Fields fld) {
    if(!can_modify(fld)) throw std::invalid_argument("You cant delete this value");
    auto index = get_index(fld);
    auto& msg = msgs[index];
    msg.is_active = false;
    set_msg_bmp_bit(index, false);
}

void FieldMessage::SetIntField(Fields fld, int val) {
    if(!can_modify(fld)) throw std::invalid_argument("You cant modify this value");
    auto index = get_index(fld);
    auto type = get_def_type(fld);
    auto& msg = msgs[index];
    if(type & type_str) throw std::invalid_argument("Wrong field type passed. Expected int field");
    msg.__fld = fld;
    msg.i = val;
    msg.is_active = true;
    set_msg_bmp_bit(index, true);
}

void FieldMessage::SetStringField(Fields fld, std::string val) {
    if(!can_modify(fld)) throw std::invalid_argument("You cant modify this value");
    auto index = get_index(fld);
    auto type = get_def_type(fld);
    auto& msg = msgs[index];
    if(type & type_int) throw std::invalid_argument("Wrong field type passed. Expected string field");
    msg.__fld = fld;
    msg.str = val;
    msg.is_active = true;
    set_msg_bmp_bit(index, true);
}

std::string FieldMessage::to_string() {
    std::stringstream ss;
    recalc_size();
    uint64_t msg_size = GetInt(Fields::message_size);
    ss.write(reinterpret_cast<const char*>(&msg_size), sizeof(uint64_t)); // message size
    ss.write(reinterpret_cast<const char*>(&msg_bitmap), sizeof(uint64_t)); // bitmask

    for(int i=1;i<get_index(Fields::field_cnt);i++)
    {
        auto& msg = msgs[i];
        if(!msg.is_active) continue;
        auto type = get_def_type(msg.__fld);
        if(type & type_int){
            ss.write(reinterpret_cast<const char*>(&msg_int), sizeof(unsigned char));
            ss.write(reinterpret_cast<const char*>(&msg.i), sizeof(int32_t));
        }else if(type & type_str){
            ss.write(reinterpret_cast<const char*>(&msg_str), sizeof(unsigned char));
            uint16_t temp_size = msg.str.size();
            ss.write(reinterpret_cast<const char*>(&temp_size), sizeof(uint16_t));
            ss.write(reinterpret_cast<const char*>(msg.str.begin().base()), temp_size);
        }
    }

    return ss.str();
}

void FieldMessage::from_string(const std::string & str) { // im done

}