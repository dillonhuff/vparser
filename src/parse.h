#pragma once

#include <string>
#include <vector>

#include "statement.h"

namespace vparser {

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
