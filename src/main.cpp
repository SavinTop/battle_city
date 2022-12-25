#include <field_msg.h>
#include <iostream>

using std::cout;
using std::endl;
using std::hex;
using std::string;
using std::stringstream;

#include <field_msg.h>
#include <iostream>

using std::cout;
using std::endl;
using std::hex;
using std::string;
using std::stringstream;

int main()
{
  FieldMessage test;
  test.set(FieldMessage::e_fields::enemy_count, 5);
  test.set(FieldMessage::e_fields::msg_to_player, "hello from message out there heeey youu");
  auto s1 = test.to_string();

  FieldMessage test2;

  test2.from_string(s1);
  s1 = test2.to_string();
  cout << "string: " << s1 << endl;
  cout << "hexval: ";
  for (const auto &item : s1)
  {
    cout << std::dec << int(item) << " ";
  }
  cout << endl;

  return 0x0;
};