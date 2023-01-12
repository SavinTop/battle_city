#pragma once

#include <variant>
#include <string>
#include <sstream>
#include <algorithm>
#include "msg_ser.h"

namespace msg{

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

    enum class field_type : unsigned char
    {
        str = 1 << 0,
        int32 = 1 << 1,
    };

    Message();

    void set(e_fields, const std::string &);
    void set(e_fields, int);
    void del(e_fields);
    bool has(e_fields) const;
    int get_int(e_fields) const;
    const std::string &get_str(e_fields) const;

private:
    static int cast(e_fields);
    static void _throw(const char *);
    static bool is_constant(e_fields);
    static void throw_on_constant(e_fields);

    template <bool>
    void set_bit(int);

    uint64_t msg_size;
    uint64_t msg_bitmap;
    static const int def_msg_size = sizeof(uint64_t)+sizeof(uint64_t)+1+sizeof(int32_t);

    struct fld_el
    {
        std::variant<int32_t, std::string> value;
        field_type type;
        bool active;
        size_t size();
    };

    fld_el els[static_cast<int>(e_fields::field_cnt)+1];

    const fld_el &operator[](e_fields) const;
    fld_el &operator[](e_fields);

    friend std::string ser::to_string(const Message&);
    friend Message ser::from_string(const std::string &);
};

}