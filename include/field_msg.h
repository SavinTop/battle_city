#pragma once

#include <string>
#include <variant>

class FieldMessage
{
private:
    static const uint64_t type_int = static_cast<int64_t>(1)<<63;
    static const uint64_t type_str = type_int>>1;
    static const uint64_t mod_const = type_str>>1;
    static const uint64_t flush_additional = ~(type_int | type_str | mod_const);
public:
    enum class Fields : uint64_t
    {
        message_size =          0 | type_int | mod_const,
        corrId =                1 | type_int | mod_const,
        msg_to_player =         2 | type_str,
        name_of_main_block =    3 | type_str,
        enemy_count =           4 | type_int,
        bullet_speed =          5 | type_int,
        field_cnt =             6
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
    uint64_t getRow(uint64_t) const;
    uint64_t cast(Fields) const;

    uint64_t msg_bitmap;
    struct{std::string str; int32_t i;bool is_active;} msgs[static_cast<int64_t>(Fields::field_cnt)];
};
