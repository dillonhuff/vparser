#include "tokenize.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace vparser {

  
  struct parse_state {
    const std::string& code;
    int i;

  public:
    parse_state(const std::string& str) : code(str), i(0) {}

    bool chars_left() const { return i < code.size(); }

    int index() const { return i; }

    parse_state operator++(int) {
      i++;
      return *this;
    }

    parse_state operator--(int) {
      i--;
      return *this;
    }
    
    char next() const { return code[i]; }
  };

  string parse_name(parse_state& ps) {
    string n = "";
    while (isalpha(ps.next()) && ps.chars_left()) {
      n += ps.next();
      cout << ps.index() << endl;
      ps++;
    }
    return n;
  }

  bool is_separator(const char c) {
    return (c == '(') ||
      (c == ')') ||
      (c == '[') ||
      (c == ']') ||
      (c == ';');
  }

  std::vector<std::string> tokenize(const std::string& verilog_code) {
    vector<string> tokens;
    int i = 0;

    parse_state ps(verilog_code);

    while (ps.chars_left()) {

      char c = ps.next();

      string nextTok;
      if (isalpha(c)) {
	nextTok = parse_name(ps);
      } else if (is_separator(c)) {
	nextTok = string(1, c);
	ps++;
      } else if (isspace(c)) {
	ps++;
	continue;
      } else {
	cout << "Unsupported char = " << c << endl;
	assert(false);
      }

      tokens.push_back(nextTok);

      i++;
    }

    return tokens;
  }

}
