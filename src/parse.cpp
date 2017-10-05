#include "parse.h"

#include "tokenize.h"

#include <cassert>

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
    assert(ts.next() == str);
    ts++;
  }

  vector<string> parse_token_list(const string& start_delim,
				  const string& end_delim,
				  const string& sep,
				  token_stream& ts) {
    assert(ts.next() == start_delim);

    vector<string> strs;

    if (ts.next(1) == end_delim) {
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

  verilog_module parse_module(const string& mod_string) {
    vector<string> tokens = tokenize(mod_string);

    token_stream ts(tokens);
    parse_token("module", ts);

    vector<string> port_names =
      parse_token_list("(", ")", ",", ts);
    
    return verilog_module(port_names);
  }

}
