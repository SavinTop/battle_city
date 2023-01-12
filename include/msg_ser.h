#pragma once

#include<string>

namespace msg{

class Message;

namespace ser{
std::string to_string(const Message&);
Message from_string(const std::string &);
}

}