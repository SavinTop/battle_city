#include <field_msg.h>
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
    test.set(Message::e_fields::enemy_count, 5);
    test.set(Message::e_fields::msg_to_player, "hello from message out there heeey youu");
    auto s1 = msg::deser::ser(test);

    Message test2;

    test2 = msg::deser::deser(s1);
    s1 = msg::deser::ser(test2);
    cout << "string: " << s1 << endl;
    cout << "hexval: ";
    for (const auto &item : s1)
    {
      cout << std::dec << int(item) << " ";
    }
    cout << endl;
  }
  catch (const char *err)
  {
    std::cerr << err << '\n';
  }

  return 0x0;
};