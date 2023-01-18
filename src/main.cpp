#include <msg/field_msg.h>
#include <iostream>

using msg::Message;
using std::cout;
using std::endl;
using std::hex;
using std::string;
using std::stringstream;

int main()
{
  try
  {
    Message test;
    test.set<int32_t>(Message::e_fields::enemy_count, 5);
    test.set<std::string>(Message::e_fields::msg_to_player, "hello from message out there heeey youu");
    auto s1 = msg::serde::ser(test);

    cout << "string: " << s1 << endl;
    cout << "hexval: ";
    for (const auto &item : s1)
    {
      cout << std::dec << int(item) << " ";
    }
    cout << endl;

    Message test2;

    test2 = msg::serde::deser(s1);
    s1 = msg::serde::ser(test2);
    
  }
  catch (std::exception& exp)
  {
    std::cerr << exp.what() << '\n';
  }

  return 0x0;
};