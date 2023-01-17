#include <field_msg.h>
#include <iostream>

using std::cout;
using std::endl;
using std::hex;
using std::string;
using std::stringstream;
using msg::Message;

int main()
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

  return 0x0;
};