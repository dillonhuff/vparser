#include "tokenize.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace vparser {

  
  struct parse_state {
    const std::string& code;
    int i;
    int lineNo;

  public:
    parse_state(const std::string& str) : code(str), i(0), lineNo(1) {}

    bool chars_left() const {
      return code.size() - i > 0;
    }

    int index() const { return i; }
    int lineNumber() const { return lineNo; }

    parse_state operator++(int) {
      if (next() == '\n') {
      	lineNo++;
      }

      i++;
      return *this;
    }

    parse_state operator--(int) {
      i--;
      return *this;
    }
    
    char next() const { return code[i]; }

    char next(const int off) const { return code[i + off]; }
  };

  string parse_name(parse_state& ps) {
    string n = "";
    while ((isalpha(ps.next()) || (ps.next() == '_')) && ps.chars_left()) {
      n += ps.next();
      ps++;
    }
    return n;
  }

  bool is_separator(const char c) {
    return (c == '(') ||
      (c == ')') ||
      (c == '{') ||
      (c == '}') ||
      (c == '"') ||
      (c == '.') ||
      (c == '[') ||
      (c == ']') ||
      (c == ';') ||
      (c == '`') ||
      (c == ',') ||
      (c == ':') ||
      (c == '$') ||
      (c == '\'') ||
      (c == '?') ||
      (c == '@');
  }

  bool comment_start(const parse_state& ps) {
    char c = ps.next();
    char cd = ps.next(1);

    if ((c == '/') && (cd == '/')) {
      return true;
    }

    return false;
  }

  void ignore_comment(parse_state& ps) {
    assert(comment_start(ps));

    while (ps.chars_left() && (ps.next() != '\n')) {
      ps++;
    }
  }

  void ignore_whitespace(parse_state& ps) {
    while (ps.chars_left() && isspace(ps.next())) {
      ps++;
    }

    // if (!ps.chars_left()) {
    //   return;
    // }

    // char c = ps.next();
    // char cd = ps.next(1);

    // if ((c == '/') && (cd == '/')) {
    //   cout << "Parsing comment" << endl;
    //   ps++;
    //   ps++;
    //   while (ps.next() != '\n') {
    // 	ps++;
    //   }
    // }

    // while (ps.chars_left() && isspace(ps.next())) {
    //   ps++;
    // }
    
  }

  string parse_digits(parse_state& ps) {
    string s = "";
    while (ps.chars_left() && isdigit(ps.next())) {
      s += ps.next();
      ps++;
    }
    return s;
  }

  bool is_boolop(const char c) {
    return (c == '&') || (c == '|') || (c == '-') || (c == '+') ||
      (c == '~') ||
      (c == '*');
  }

  string parse_gt(parse_state& ps) {
    assert(ps.next() == '>');

    string res = ">";
    ps++;

    if (ps.next() == '>') {
      res += ps.next();
      ps++;
      return res;
    }

    return res;
  }

  string parse_lt(parse_state& ps) {
    assert(ps.next() == '<');

    if ((ps.next(1) == '=') || (ps.next(1) == '<')) {

      string next_tok = "";
      next_tok += ps.next();
      next_tok += ps.next(1);
      ps++;
      ps++;

      return next_tok;
    }

    string next_tok(1, ps.next());
    ps++;

    return next_tok;
  }

  std::vector<std::string> tokenize(const std::string& verilog_code) {
    vector<string> tokens;
    int i = 0;

    parse_state ps(verilog_code);

    while (ps.chars_left()) {

      ignore_whitespace(ps);

      if (!ps.chars_left()) {
	break;
      }

      char c = ps.next();

      string nextTok;
      if (isalpha(c)) {
	nextTok = parse_name(ps);
      } else if (isdigit(c)) {
	nextTok = parse_digits(ps);
      } else if (is_separator(c)) {
	nextTok = string(1, c);
	ps++;
      } else if (isspace(c)) {
	ps++;
	continue;
      } else if (comment_start(ps)) {
	ignore_comment(ps);
	continue;
      } else if (c == '=') {
	if (ps.next(1) == '=') {
	  nextTok = "==";
	  ps++;
	}
	nextTok = "=";

	ps++;

      } else if (c == '<') {
	parse_lt(ps);

      } else if (c == '>') {
	nextTok = parse_gt(ps);
      } else if (is_boolop(c)) {
	nextTok = string(1, c);
	ps++;
      } else if (c == '!') {
	assert(ps.next(1) == '=');
	nextTok = "!=";
	ps++;
	ps++;
      } else {
	
	cout << "Unsupported char = " << c << " at position " << ps.index() << ", line number = " << ps.lineNumber() << endl;
	assert(false);
      }

      tokens.push_back(nextTok);

      i++;
    }

    return tokens;
  }

}
