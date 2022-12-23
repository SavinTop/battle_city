#include<field_msg.h>
#include<random>
#include<stdexcept>

FieldMessage::FieldMessage() 
: msg_bitmap(0b11) 
{
auto& message_size = msgs[cast(Fields::message_size)];
message_size.is_active = true;
message_size.i = 8+sizeof(msg_bitmap)+1+sizeof(int32_t);
auto& unique_id = msgs[cast(Fields::corrId)];
message_size.is_active = true;
message_size.i = 141351451; // replace with random generator
}

uint64_t FieldMessage::cast(Fields fld) const {
    return static_cast<uint64_t>(fld);
}