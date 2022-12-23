
#include <field_msg.h>
#include <iostream>

using std::cout; using std::endl;
using std::string; using std::hex;
using std::stringstream;

int main(){
  FieldMessage test;
  test.SetIntField(FieldMessage::Fields::enemy_count, 5);
  auto s1 = test.to_string();
  cout << "string: " << s1 << endl;
    cout << "hexval: ";
    for (const auto &item : s1) {
        cout << std::dec << int(item) << " ";
    }
    cout << endl;

  return 0x0;
};