#include<field_msg.h>
#include<random>
#include<stdexcept>
#include<sstream>
#include<bitset>

FieldMessage::FieldMessage() 
: msg_bitmap(0b11),
  msg_size(8+8+5) //default size 8 message_size 8 bitmask and 5 for corrId
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int32_t> dist(INT32_MIN, INT32_MAX);
    (*this)[e_fields::message_size]       = {0, field_type::int32, true}; // dont use it anyway, msg size variable is more comfortable
    (*this)[e_fields::corrId]             = {dist(mt), field_type::int32, true};     // replace with random value
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

void FieldMessage::_throw(const char * str)
{
    throw std::invalid_argument(str);
}

bool FieldMessage::is_constant(e_fields fld){
    return fld == e_fields::message_size ||
           fld == e_fields::corrId ||
           fld == e_fields::field_cnt;
}

void FieldMessage::throw_on_constant(e_fields fld) {
    if(is_constant(fld)) _throw("this value cant be modified");
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
    if(val.size()>=UINT16_MAX) _throw("string size cant be more or eq UINT16_MAX");
    auto& curr = (*this)[fld];
    int size_before_assign = curr.size();
    if(curr.type==field_type::str) curr.value = val;
    else _throw("cant set string for int field");
    if(curr.active) {
        msg_size-=size_before_assign+3; // adjust message size value for new size
    }
    curr.active = true;
    set_bit<true>(cast(fld));
    msg_size+=curr.size()+3;
}

void FieldMessage::set(e_fields fld, int val)
{
    throw_on_constant(fld);
    auto& curr = (*this)[fld];
    if(curr.type==field_type::int32) curr.value = val;
    else _throw("cant set int for string field");
    if(curr.active) return;
    curr.active = true;
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

std::string FieldMessage::to_string() const
{
    std::stringstream ss;
    const int fld_cnt = cast(e_fields::field_cnt);
    ss.write(reinterpret_cast<const char*>(&msg_size), sizeof(uint64_t)); 
    ss.write(reinterpret_cast<const char*>(&msg_bitmap), sizeof(uint64_t)); 

    auto temp_bitmap = msg_bitmap >> 1;

    for(int i=1;temp_bitmap && i<fld_cnt;temp_bitmap>>=1,i++){
        if(!(temp_bitmap & 1)) continue;
        auto& curr = els[i];
        auto type = curr.type; 
        if(type == field_type::int32){
            int temp = std::get<int32_t>(curr.value);
            ss.write(reinterpret_cast<const char*>(&type), sizeof(unsigned char));
            ss.write(reinterpret_cast<const char*>(&temp), sizeof(int32_t));
        }else{
            const std::string& temp = std::get<std::string>(curr.value);
            ss.write(reinterpret_cast<const char*>(&type), sizeof(unsigned char));
            uint16_t temp_size = temp.size();
            ss.write(reinterpret_cast<const char*>(&temp_size), sizeof(uint16_t));
            ss.write(reinterpret_cast<const char*>(temp.begin().base()), temp_size);
        }
    }

    return ss.str();
}

void FieldMessage::from_string(const std::string & s)
{
    if(s.size()<8+8+5) _throw("the size is to small, no requared fields here");
    const int fld_cnt = cast(e_fields::field_cnt);
    std::stringstream ss;
    ss << s;

    FieldMessage new_msg;

    ss.read(reinterpret_cast<char*>(&new_msg.msg_size), sizeof(uint64_t)); 
    ss.read(reinterpret_cast<char*>(&new_msg.msg_bitmap), sizeof(uint64_t)); 

    if(new_msg.msg_size!=s.size()) _throw("the size of the message is wrong");

    auto temp_bitmap = new_msg.msg_bitmap >> 1;

    for(int i=1;temp_bitmap;temp_bitmap>>=1,i++){
        if(!(temp_bitmap & 1)) continue;
        if(i>=fld_cnt) _throw("the bitmap field gives the wrong data");
        auto& curr = new_msg.els[i];
        auto& exp_type = curr.type;
        unsigned char type;
        ss.read(reinterpret_cast<char*>(&type), sizeof(unsigned char));

        if(static_cast<unsigned char>(exp_type)!=type) _throw("the type of the fields do not match");

        if(exp_type == field_type::int32){
            int av = ss.tellp();
            if(av==-1) _throw("contact developer, this is insane - tellp returned -1");
            if(s.size()-av-1<4) _throw("there is not enough space in the buffer to read int value");
            int temp;
            ss.read(reinterpret_cast<char*>(&temp), sizeof(int32_t));
            curr.value = temp;
        }else{
            int av = ss.tellp();
            if(av==-1) _throw("contact developer, this is insane - tellp returned -1");
            if(s.size()-av-1<2) _throw("there is not enough space in the buffer to get string size");
            uint16_t temp_size;
            ss.read(reinterpret_cast<char*>(&temp_size), sizeof(uint16_t));
            if(s.size()-ss.tellp()-1<temp_size) _throw("there is not enough space in the buffer to read string");
            std::string temp(temp_size, 0);
            ss.read(reinterpret_cast<char*>(temp.begin().base()), temp_size);
            curr.value = temp;
        }
        curr.active = true;
    }

/* Yeah we dont have own move constructor, 
but default one takes it pretty right - copying array and moving variants(at list on my compiler). 
Can be better way to solve this with pointers and memory allocated array.
But this will be totally overhead for this tiny array, which max size is 64 els.*/

(*this) = std::move(new_msg); 
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
