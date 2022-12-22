#include<field_msg.h>
#include<random>
#include<stdexcept>


FieldMessage::FieldMessage()
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int32_t> distribution(INT32_MIN,INT32_MAX);
    msgs[(int64_t)Fields::message_size] = {8+8+1+4, true}; //def size
    msgs[(int64_t)Fields::corrId] = {distribution(generator), true};
}

bool FieldMessage::Has(Fields fld) const {
    return msgs[(int64_t)fld].isAlive;
}

void FieldMessage::DeleteField(Fields fld) {
    msgs[(int64_t)fld].isAlive = false;
}

void FieldMessage::check_int(Fields fld) const{
    if(!Has(fld)) throw std::invalid_argument("there's no such value");
    if(types[(int64_t)fld]!=type::string) throw std::invalid_argument("type missmatch: field is int32");
}

void FieldMessage::check_str(Fields fld) const{
    if(!Has(fld)) throw std::invalid_argument("there's no such value");
    if(types[(int64_t)fld]!=type::int32) throw std::invalid_argument("type missmatch: field is string");
}

int FieldMessage::GetInt(Fields fld) const {
    auto& val = msgs[(int64_t)fld];
    auto& t = types[(int64_t)fld];
    check_int(fld);
    return std::get<int32_t>(val.msg);
}

std::string FieldMessage::GetString(Fields fld) const {
    auto& val = msgs[(int64_t)fld];
    auto& t = types[(int64_t)fld];
    check_str(fld);
    return std::get<std::string>(val.msg);
}

void FieldMessage::recalculate_size() {
    auto& val = msgs[(int64_t)Fields::message_size];
    int32_t sum = 8+8+1+4; // first two fields
    for(int i=2;i<(int64_t)Fields::field_cnt;i++){
        auto t = types[i];
        auto& el = msgs[i];
        if(t==type::int32)
            sum+=sizeof(int32_t)+type_check_size;
        else if(t==type::string){
            sum+=std::get<std::string>(el.msg).size()+type_check_size+str_len_size;
        }
    }
    val.msg = sum;
}

void FieldMessage::SetIntField(Fields fld, int num) {
    auto& val = msgs[(int64_t)fld];
    auto& t = types[(int64_t)fld];
    check_int(fld);
    val.isAlive = true;
    val.msg = num;
}

void FieldMessage::SetStringField(Fields fld, std::string str) {
    auto& val = msgs[(int64_t)fld];
    auto& t = types[(int64_t)fld];
    check_str(fld);
    val.msg = str;
    val.isAlive = true;
    recalculate_size();
}

std::string FieldMessage::to_string() const {
    uint64_t message_size = std::get<int32_t>(msgs[(uint64_t)Fields::message_size].msg);
    uint64_t bitmask = 0;
    std::vector<char> out(message_size);
    int offset = 8+8;
    (*out.begin()) = message_size;
    for(int i=2;i<(int64_t)Fields::field_cnt;i++){
        auto t = types[i];
        auto& el = msgs[i];
        int size;
        if(t==type::int32){
            size=sizeof(int32_t)+type_check_size;
            int32_t val = std::get<int32_t>(el.msg);
            (*(out.begin()+offset)) = (unsigned char)type::int32;
            memcpy(out.begin().base()+offset+type_check_size, &val, sizeof(val));
        }
        else if(t==type::string){
            size=std::get<std::string>(el.msg).size()+type_check_size+str_len_size;
            std::string val = std::get<std::string>(el.msg); // thats fucked up
            int size = val.size();
            (*(out.begin().base()+offset)) = (unsigned char)type::string;
            memcpy(out.begin().base()+offset+type_check_size, &val, val.size());
        }
        //(*out.begin()+offset) = message_size;
    }
}
