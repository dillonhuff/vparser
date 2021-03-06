#pragma once

#include <string>
#include <vector>

#include "statement.h"
#include "token.h"

namespace vparser {

  class token_stream {
  protected:
    const std::vector<token>& toks;
    int i;

  public:
    token_stream(const std::vector<token>& toks_) : toks(toks_), i(0) {}

    bool chars_left() const {
      return toks.size() - i > 0;
    }

    int index() const { return i; }

    token_stream operator++(int) {

      i++;
      return *this;
    }

    std::string remaining_string() const {
      std::string rem = "";
      for (int ind = i; ind < toks.size(); ind++) {
        rem += toks[ind].get_text() + " ";
      }
      return rem;
    }

    token_stream operator--(int) {
      i--;
      return *this;
    }

    std::string next() const { return toks[i].get_text(); }

    std::string next(const int off) const { return toks[i + off].get_text(); }
    
  };

  void parse_token(const std::string& str, token_stream& ts);

  class verilog_module {

    std::string name;
    std::vector<decl_stmt*> ports;
    std::vector<statement*> statements;

  public:

    verilog_module() {}

    ~verilog_module() {
      for (auto st : statements) {
        delete st;
      }
    }

    verilog_module(const std::string& name,
                   //const std::vector<std::string>& port_names_,
                   const std::vector<decl_stmt*>& ports_,
                   const std::vector<statement*>& statements_) :
      name(name), ports(ports_), statements(statements_) {}

    std::string get_name() const {
      return name;
    }

    std::vector<std::string> get_port_names() const {
      std::vector<std::string> port_names;
      for (auto& port : ports) {
        port_names.push_back(port->get_name());
      }
      return port_names;
    }

    std::string to_string() {
      std::string str = "module " + name + "(\n";

      auto ports = get_port_names();
      for (int i = 0; i < ports.size(); i++) {
        str += indent(1) + ports[i];

        if (i < ports.size() - 1) {
          str += ",\n";
        }
      }

      str += ");\n\n";

      for (auto& stmt : get_statements()) {
        str += stmt->to_string(1) + "\n";
      }

      str += "\nendmodule";
      return str;
    }

    std::vector<statement*>
    get_statements() const {
      return statements;
    }
  };

  verilog_module parse_module(const std::string& mod_string);

  statement* parse_statement(const std::string& stmt_string);
  expression* parse_expression(const std::string& stmt_string);

  expression* parse_expression(token_stream& ts);

}
