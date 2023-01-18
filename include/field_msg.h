#pragma once

#include <variant>
#include <string>
#include <sstream>
#include <algorithm>
#include <serialization.h>
#include <bitset>

namespace msg
{
    
class Message
{
public:
    enum class e_fields
    {
        message_size,
        corrId,
        msg_to_player,
        name_of_main_block,
        enemy_count,
        bullet_speed,
        field_cnt
    };

    Message();

    void set(e_fields, const std::string &);
    void set(e_fields, int);
    void del(e_fields);
    bool has(e_fields) const;
    int get_int(e_fields) const;
    const std::string &get_str(e_fields) const;

private:
    size_t cast(e_fields) const;

    bool is_constant(e_fields);

    void throw_on_constant(e_fields);

    uint64_t msg_size;
    std::bitset<64> msg_bitset;
    //msg size, bitset, 1(type) + unique msg id
    static const int def_msg_size = sizeof(uint64_t)+sizeof(uint64_t)+1+sizeof(int32_t);

    struct fld_el
    {
        std::variant<int32_t, std::string> value;
        field_type type;
        bool active;
        size_t size();
    };

    template<size_t SIZE>
    struct fld_list{
        template<typename T>
        const fld_el &operator[](T fld) const{
            return els[static_cast<size_t>(fld)];
        }

        template<typename T>
        fld_el &operator[](T fld){
            return els[static_cast<size_t>(fld)];
        }

        fld_el els[SIZE];
    };

    fld_list<static_cast<size_t>(e_fields::field_cnt)+1> list;

    friend std::string serde::ser(const Message&);
    friend Message serde::deser(std::string);
};

} // namespace msg
