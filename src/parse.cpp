#include "parse.h"

#include "tokenize.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace vparser {

  class token_stream {
  protected:
    const vector<string>& toks;
    int i;

  public:
    token_stream(const vector<string>& toks_) : toks(toks_), i(0) {}

    bool chars_left() const {
      return toks.size() - i > 0;
    }

    int index() const { return i; }

    token_stream operator++(int) {

      i++;
      return *this;
    }

    token_stream operator--(int) {
      i--;
      return *this;
    }
    
    string next() const { return toks[i]; }

    string next(const int off) const { return toks[i + off]; }
    
  };

  void parse_token(const string& str, token_stream& ts) {
    if (ts.next() != str) {
      cout << "Error: Unexpected token: " << ts.next() << ", expected " << str << endl;
      assert(false);
    }

    ts++;
  }

  vector<string> parse_token_list(const string& start_delim,
				  const string& end_delim,
				  const string& sep,
				  token_stream& ts) {
    assert(ts.next() == start_delim);

    vector<string> strs;

    if (ts.next(1) == end_delim) {
      ts++;
      ts++;
      return {};
    }

    do {
      ts++;

      strs.push_back(ts.next());

      ts++;

    } while (ts.next() == sep);

    assert(ts.next() == end_delim);

    ts++;

    return strs;
  }

  void parse_declaration(token_stream& ts) {
    string ns = ts.next();

    vector<string> declStrs;
    while (ts.next() != ";") {
      declStrs.push_back(ts.next());
      ts++;
    }

    assert(ts.next() == ";");
    ts++;
    
  }

  void parse_statement(token_stream& ts) {
    string ns = ts.next();

    if ((ns == "input") || (ns == "output") || (ns == "reg")) {
      parse_declaration(ts);
      return;
    } else {
      cout << "Unsupported statement start token = " << ns << endl;
      assert(false);
    }
  }

  verilog_module parse_module(const string& mod_string) {
    vector<string> tokens = tokenize(mod_string);

    cout << "TOKENS" << endl;
    for (auto& t : tokens) {
      cout << t << endl;
    }

    token_stream ts(tokens);
    parse_token("module", ts);

    string modName = ts.next();
    ts++;

    vector<string> port_names =
      parse_token_list("(", ")", ",", ts);

    // Add statement parsing
    parse_token(";", ts);

    // Statement parsing
    while (ts.next() != "endmodule") {
      parse_statement(ts);
    }
    
    parse_token("endmodule", ts);

    assert(!ts.chars_left());

    return verilog_module(port_names);
  }

}
