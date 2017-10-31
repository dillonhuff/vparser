#pragma once

#include <string>
#include <vector>

#include "statement.h"

namespace vparser {

  class token_stream {
  protected:
    const std::vector<std::string>& toks;
    int i;

  public:
    token_stream(const std::vector<std::string>& toks_) : toks(toks_), i(0) {}

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

    std::string next() const { return toks[i]; }

    std::string next(const int off) const { return toks[i + off]; }
    
  };

  void parse_token(const std::string& str, token_stream& ts);

  class verilog_module {

    std::vector<std::string> port_names;

    std::vector<statement*> statements;

  public:

    verilog_module() {}

    ~verilog_module() {
      for (auto st : statements) {
        delete st;
      }
    }

    verilog_module(const std::vector<std::string>& port_names_,
                   const std::vector<statement*>& statements_) :
      port_names(port_names_), statements(statements_) {}

    std::vector<std::string> get_port_names() const {
      return port_names;
    }

    std::vector<statement*>
    get_statements() const {
      return statements;
    }
  };

  verilog_module parse_module(const std::string& mod_string);

  statement* parse_statement(const std::string& stmt_string);
  expression* parse_expression(const std::string& stmt_string);

}
