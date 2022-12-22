#pragma once

#include <string>
#include <variant>

class FieldMessage
{
public:
    enum class Fields : uint64_t
    {
        message_size,
        corrId,
        msg_to_player,
        name_of_main_block,
        enemy_count,
        bullet_speed,
        field_cnt
    }; 
    enum class type : unsigned char{string = 0, int32=127};
    const type types[(uint64_t)Fields::field_cnt] = {
        type::int32,
        type::int32,
        type::string,
        type::string,
        type::int32,
        type::int32,
        };

    FieldMessage();
    void SetStringField(Fields, std::string);
    void SetIntField(Fields, int);
    void DeleteField(Fields);
    std::string to_string() const;
    void from_string(const std::string &);
    bool Has(Fields) const;
    int GetInt(Fields) const;
    std::string GetString(Fields) const;

private:
    void check_int(Fields) const;
    void check_str(Fields) const;
    void recalculate_size();
    struct msg{
        std::variant<int32_t,std::string> msg;
        bool isAlive = false;
    };
    msg msgs[(int64_t)Fields::field_cnt];

    const int type_check_size = 1;
    const int str_len_size = 2;
};
