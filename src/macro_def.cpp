#include "macro_def.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace vparser {

  vector<string> split_lines(const std::string& text) {

    vector<string> lines;

    if (text == "") { return lines; }

    std::stringstream ss(text);
    std::string to;
    
    while(std::getline(ss,to,'\n')) {
      lines.push_back(to);
    }

    return lines;
  }

  std::vector<macro_def>
  parse_macro_defs(const std::string& verilog_text) {

    vector<string> lines = split_lines(verilog_text);

    vector<macro_def> defs;

    for (auto& line : lines) {
      if (line[0] == '`') {
        cout << "MACRO: " << line << endl;
        defs.push_back(macro_def());
      }
    }

    return defs;
  }

}
