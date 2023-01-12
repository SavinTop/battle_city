#include"msg_ser.h"
#include"field_msg.h"

namespace msg{

namespace ser{
std::string to_string(const Message& msg){
    std::stringstream ss;
    const int fld_cnt = Message::cast(Message::e_fields::field_cnt);
    ss.write(reinterpret_cast<const char *>(&msg.msg_size), sizeof(uint64_t));
    ss.write(reinterpret_cast<const char *>(&msg.msg_bitmap), sizeof(uint64_t));

    auto temp_bitmap = msg.msg_bitmap >> 1;

    for (int i = 1; temp_bitmap && i < fld_cnt; temp_bitmap >>= 1, i++)
    {
        if (!(temp_bitmap & 1))
            continue;
        auto &curr = msg.els[i];
        auto type = curr.type;
        if (type == Message::field_type::int32)
        {
            int temp = std::get<int32_t>(curr.value);
            ss.write(reinterpret_cast<const char *>(&type), sizeof(unsigned char));
            ss.write(reinterpret_cast<const char *>(&temp), sizeof(int32_t));
        }
        else
        {
            const std::string &temp = std::get<std::string>(curr.value);
            ss.write(reinterpret_cast<const char *>(&type), sizeof(unsigned char));
            uint16_t temp_size = temp.size();
            ss.write(reinterpret_cast<const char *>(&temp_size), sizeof(uint16_t));
            ss.write(reinterpret_cast<const char *>(temp.begin().base()), temp_size);
        }
    }

    return ss.str();
}

Message from_string(const std::string & s){
    static const int def_msg_size = sizeof(uint64_t)+sizeof(uint64_t)+1+sizeof(int32_t);

    if (s.size() < def_msg_size)
        Message::_throw("the size is to small, no requared fields here");
    const int fld_cnt = Message::cast(Message::e_fields::field_cnt);
    std::stringstream ss;
    ss << s;

    Message new_msg; // if we have the exception, we dont lose this* if we use this var instead

    ss.read(reinterpret_cast<char *>(&new_msg.msg_size), sizeof(uint64_t));
    ss.read(reinterpret_cast<char *>(&new_msg.msg_bitmap), sizeof(uint64_t));

    if (new_msg.msg_size != s.size())
        Message::_throw("the size of the message is wrong");

    auto temp_bitmap = new_msg.msg_bitmap >> 1;

    for (int i = 1; temp_bitmap; temp_bitmap >>= 1, i++)
    {
        if (!(temp_bitmap & 1))
            continue;
        if (i >= fld_cnt)
            Message::_throw("the bitmap field gives the wrong data");
        auto &curr = new_msg.els[i];
        auto &exp_type = curr.type;
        if (s.size() - ss.tellp() - 1 < sizeof(unsigned char))
                Message::_throw("there is not enough space in the buffer to read type");
        unsigned char type;
        ss.read(reinterpret_cast<char *>(&type), sizeof(unsigned char));

        if (static_cast<unsigned char>(exp_type) != type)
            Message::_throw("the type of the fields do not match");

        if (exp_type == Message::field_type::int32)
        {
            int av = ss.tellp();
            if (s.size() - av - 1 < sizeof(int32_t))
                Message::_throw("there is not enough space in the buffer to read int value");
            int temp;
            ss.read(reinterpret_cast<char *>(&temp), sizeof(int32_t));
            curr.value = temp;
        }
        else
        {
            if (s.size() - ss.tellp() - 1 < sizeof(uint16_t))
                Message::_throw("there is not enough space in the buffer to get string size");
            uint16_t temp_size;
            ss.read(reinterpret_cast<char *>(&temp_size), sizeof(uint16_t));
            if (s.size() - ss.tellp() - 1 < temp_size)
                Message::_throw("there is not enough space in the buffer to read string");
            std::string temp(temp_size, 0);
            ss.read(reinterpret_cast<char *>(temp.begin().base()), temp_size);
            curr.value = temp;
        }
        curr.active = true;
    }

    return new_msg;
}
}

}