#include "parse.h"

#include "tokenize.h"

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

  verilog_module parse_module(const string& mod_string) {
    vector<string> tokens = tokenize(mod_string);

    token_stream ts(tokens);
    
    return {};
  }

}
