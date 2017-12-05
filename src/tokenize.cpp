#include "tokenize.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace vparser {

  struct parse_state {
    const std::string& code;
    int i;
    int lineNo;
    int linePos;

  public:
    parse_state(const std::string& str) : code(str), i(0), lineNo(1), linePos(1) {}

    bool chars_left() const {
      return code.size() - i > 0;
    }

    int num_chars_left() const {
      return code.size() - i;
    }
    
    int index() const { return i; }
    int lineNumber() const { return lineNo; }
    int line_pos() const { return linePos; }

    parse_state operator++(int) {
      if (next() == '\n') {
      	lineNo++;
        linePos = 1;
      } else {
        linePos++;
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
    while (ps.chars_left() && (isalpha(ps.next()) ||
			       (ps.next() == '_') ||
			       isdigit(ps.next()))) {
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
    if (ps.num_chars_left() < 2) {
      return false;
    }

    char c = ps.next();
    char cd = ps.next(1);

    if ((c == '/') && (cd == '/')) {
      return true;
    }

    return false;
  }

  bool unlimited_comment_end(const parse_state& ps) {
    if (ps.num_chars_left() < 2) {
      return false;
    }

    char c = ps.next();
    char cd = ps.next(1);

    if ((c == '*') && (cd == '/')) {
      return true;
    }

    return false;
  }
  
  bool unlimited_comment_start(const parse_state& ps) {
    if (ps.num_chars_left() < 2) {
      return false;
    }

    char c = ps.next();
    char cd = ps.next(1);

    if ((c == '/') && (cd == '*')) {
      return true;
    }

    return false;
  }

  void ignore_unlimited_comment(parse_state& ps) {
    assert(unlimited_comment_start(ps));

    bool found_end = false;
    while (ps.chars_left()) {
      found_end = unlimited_comment_end(ps);

      if (found_end) {
	ps++;
	ps++;
	break;
      }
      ps++;
    }

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

  string parse_string_literal(parse_state& ps) {
    assert(ps.next() == '"');

    string tok = "";
    tok += "\"";

    ps++;


    while (ps.next() != '"') {
      tok += ps.next();
      ps++;
    }

    assert(ps.next() == '"');

    tok += "\"";
    ps++;

    return tok;
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

    if (ps.next() == '=') {
      res += ps.next();
      ps++;
      return res;
    }
    
    return res;
  }

  string parse_lt(parse_state& ps) {
    assert(ps.next() == '<');

    if (ps.next(1) == '=') {
      ps++;
      ps++;
      return "<=";
    }

    if (ps.next(1) == '<') {
      ps++;
      ps++;
      return "<<";
    }

    ps++;

    return "<";

  }

  std::vector<token> tokenize(const std::string& verilog_code) {
    vector<token> tokens;
    int i = 0;

    parse_state ps(verilog_code);

    while (ps.chars_left()) {

      ignore_whitespace(ps);

      if (!ps.chars_left()) {
	break;
      }

      char c = ps.next();

      int line_no = ps.lineNumber();
      int line_pos = ps.line_pos();
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
      } else if (unlimited_comment_start(ps)) {
	ignore_unlimited_comment(ps);
	continue;
      } else if (c == '=') {
	if (ps.next(1) == '=') {
	  nextTok = "==";
	  ps++;
          ps++;
	} else {
          nextTok = "=";
          ps++;
        }

      } else if (c == '<') {
	nextTok = parse_lt(ps);
      } else if (c == '>') {
	nextTok = parse_gt(ps);
      } else if (is_boolop(c) && !is_boolop(ps.next(1))) {
	nextTok = string(1, c);
	ps++;
      } else if (c == '#') {
        nextTok = "#";
        ps++;
      } else if (c == '&') {
        assert(ps.next(1) == '&');
        nextTok = "&&";
        ps++;
        ps++;
      } else if (c == '|') {
        assert(ps.next(1) == '|');
        nextTok = "||";
        ps++;
        ps++;
      } else if (c == '!') {
	assert(ps.next(1) == '=');
	nextTok = "!=";
	ps++;
	ps++;
      } else if (c == '"') {
        cout << "Parsing string" << endl;
        nextTok = parse_string_literal(ps);
      } else {
	
	cout << "Unsupported char = " << c << " at position " << ps.index() << ", line number = " << ps.lineNumber() << endl;
	assert(false);
      }

      tokens.push_back(token(nextTok, source_position(line_no, line_pos)));

      i++;
    }

    return tokens;
  }

}
