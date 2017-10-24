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

  void parse_statement(token_stream& ts);

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

  vector<string>
  parse_enclosed_tokens(const std::string& start,
			const std::string& end,
			token_stream& ts) {

    parse_token(start, ts);

    vector<string> toks;

    while (ts.next() != end) {
      toks.push_back(ts.next());
      ts++;
    }

    assert(ts.next() == end);
    ts++;

    return toks;
  }

  void parse_always(token_stream& ts) {
    string ns = ts.next();

    parse_token("always", ts);

    parse_token("@", ts);

    parse_enclosed_tokens("(", ")", ts);

    parse_token("begin", ts);

    while (ts.next() != "end") {
      parse_statement(ts);
    }

    parse_token("end", ts);
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

  void parse_if(token_stream& ts) {
    parse_token("if", ts);

    parse_enclosed_tokens("(", ")", ts);

    parse_token("begin", ts);

    while (ts.next() != "end") {
      parse_statement(ts);
    }

    parse_token("end", ts);

    if (ts.next() == "else") {
      parse_token("else", ts);

      parse_token("begin", ts);

      while (ts.next() != "end") {
	parse_statement(ts);
      }

      parse_token("end", ts);
      
    }
  }

  void parse_id_statement(token_stream& ts) {
    vector<string> strs;

    while (ts.next() != ";") {
      strs.push_back(ts.next());
      ts++;
    }

    parse_token(";", ts);
  }

  void parse_case(token_stream& ts) {
    cout << "Parsing case" << endl;

    parse_token("case", ts);

    parse_enclosed_tokens("(", ")", ts);

    cout << "Tokens in case = " << endl;
    while (ts.next() != "endcase") {
      while (ts.next() != ";") {
	cout << ts.next() << endl;
	ts++;
      }

      parse_token(";", ts);

    }

    parse_token("endcase", ts);

    cout << "Done parsing case" << endl;
  }

  void parse_statement(token_stream& ts) {
    string ns = ts.next();

    if ((ns == "input") || (ns == "output") || (ns == "reg")) {
      parse_declaration(ts);
    } else if (ns == "always") {
      parse_always(ts);
    } else if (ns == "if") {
      parse_if(ts);
    } else if (ns == "else") {
      assert(false);
    } else if (ns == "endcase") {
      assert(false);
    } else if (ns == "case") {
      parse_case(ts);
    } else if (isalpha(ns[0]) || (ns[0] == '_')) {
      parse_id_statement(ts);
    } else {
      cout << "Unsupported statement start token = " << ns << endl;
      while (ts.chars_left()) {
	cout << ts.next() << endl;
	ts++;
      }

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

    vector<vector<string> > statements;
    // Statement parsing
    while (ts.next() != "endmodule") {
      parse_statement(ts);
    }
    
    parse_token("endmodule", ts);

    assert(!ts.chars_left());

    return verilog_module(port_names, statements);
  }

}
