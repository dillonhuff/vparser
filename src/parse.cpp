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

  bool is_integer(const std::string& str) {
    for (int i = 0; i < str.size(); i++) {
      if (!isdigit(str[i])) {
        return false;
      }
    }

    return true;
  }

  bool is_id(const std::string& str) {
    return isalpha(str[0]);
  }

  expression* parse_expression(token_stream& ts);
  statement* parse_statement(token_stream& ts);

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

  statement*
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

    return new decl_stmt();
  }

  statement* parse_always(token_stream& ts) {
    string ns = ts.next();

    parse_token("always", ts);

    parse_token("@", ts);

    parse_enclosed_tokens("(", ")", ts);

    parse_token("begin", ts);

    vector<statement*> stmts;

    while (ts.next() != "end") {
      stmts.push_back(parse_statement(ts));
    }

    parse_token("end", ts);

    return new always_stmt(stmts);
  }

  statement* parse_declaration(token_stream& ts) {
    string ns = ts.next();

    vector<string> declStrs;
    while (ts.next() != ";") {
      declStrs.push_back(ts.next());
      ts++;
    }

    assert(ts.next() == ";");
    ts++;

    return new decl_stmt(); //declStrs;
    
  }

  statement* parse_if(token_stream& ts) {
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

    return new if_stmt();
  }

  statement* parse_module_instantiation(token_stream& ts) {
    string module_type = ts.next();
    ts++;

    assert(is_id(module_type));

    string module_name = ts.next();
    ts++;

    assert(is_id(module_name));

    vector<pair<string, expression*> > port_assignments;

    parse_token("(", ts);

    while (ts.next() != ")") {
      parse_token(".", ts);

      string port_name = ts.next();
      ts++;

      parse_token("(", ts);

      expression* expr = parse_expression(ts);

      parse_token(")", ts);

      port_assignments.push_back({port_name, expr});      

      if (ts.next() == ",") {
        parse_token(",", ts);
      } else {
        break;
      }

    }

    parse_token(")", ts);

    return new module_instantiation_stmt(module_type,
                                         module_name,
                                         port_assignments);
  }

  statement* parse_id_statement(token_stream& ts) {
    vector<string> strs;

    while (ts.next() != ";") {
      strs.push_back(ts.next());
      ts++;
    }

    parse_token(";", ts);

    return new decl_stmt(); //{};
  }

  int parse_integer(token_stream& ts) {
    assert(is_integer(ts.next()));

    int val = stoi(ts.next());
    ts++;
    return val;
  }

  expression* parse_expression(token_stream& ts) {
    string nx = ts.next();

    //cout << "nx = " << nx << endl;

    expression* expr = nullptr;
    if (is_integer(nx)) {

      //int len = stoi(nx);
      ts++;

      parse_token("'", ts);
      //string val = ts.next();
      ts++;

      expr = new num_expr();
    } else if (is_id(nx)) {
      ts++;
      expr = new id_expr(nx);
    }

    nx = ts.next();
    if (nx[0] == '[') {
      parse_token("[", ts);

      cout << "Next token in slice = " << ts.next() << endl;

      expression* start = parse_expression(ts);
      //int start = parse_integer(ts);
      // int start = stoi(ts.next());
      // ts++;

      parse_token(":", ts);

      expression* end = parse_expression(ts);
      // int end = stoi(ts.next());
      // ts++;

      parse_token("]", ts);

      return new slice_expr(expr, start, end);
    }

    return expr;
  }

  statement* parse_case(token_stream& ts) {

    parse_token("case", ts);

    parse_enclosed_tokens("(", ")", ts);

    vector<pair<expression*, statement*> > cases;
    statement* default_case;

    bool found_default = false;    

    while (ts.next() != "endcase") {

      expression* expr = nullptr;
      statement* stmt = nullptr;

      if (ts.next() != "default") {
        expr = parse_expression(ts);
      } else {
        // There can only be one default case
        assert(!found_default);

        found_default = true;
        ts++;
      }

      parse_token(":", ts);

      stmt = parse_statement(ts);

      //cout << "Found statement = " << *stmt << endl;

      if (!found_default) {
        cases.push_back({expr, stmt});
      } else {
        default_case = stmt;
      }

    }

    parse_token("endcase", ts);

    if (found_default) {
      return new case_stmt(cases, default_case);
    }
    return new case_stmt(cases);
  }

  statement* parse_assign(token_stream& ts) {
    parse_token("assign", ts);

    expression* lhs = parse_expression(ts);

    parse_token("=", ts);

    expression* rhs = parse_expression(ts);

    parse_token(";", ts);

    return new assign_stmt(lhs, rhs);
  }

  statement* parse_statement(token_stream& ts) {
    string ns = ts.next();

    if ((ns == "input") || (ns == "output") || (ns == "reg")) {
      return parse_declaration(ts);
    } else if (ns == "always") {
      return parse_always(ts);
    } else if (ns == "if") {
      return parse_if(ts);
    } else if (ns == "assign") {
      return parse_assign(ts);
    } else if (ns == "else") {
      assert(false);
    } else if (ns == "endcase") {
      assert(false);
    } else if (ns == "case") {
      return parse_case(ts);
    } else if (isalpha(ns[0]) || (ns[0] == '_')) {
      string nn = ts.next(1);

      if (is_id(nn)) {
        return parse_module_instantiation(ts);
      }

      return parse_id_statement(ts);
    } else if (ns == ";") {
      ts++;
      return new empty_stmt();
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

    token_stream ts(tokens);
    parse_token("module", ts);

    string modName = ts.next();
    ts++;

    vector<string> port_names =
      parse_token_list("(", ")", ",", ts);

    // Add statement parsing
    parse_token(";", ts);

    vector<statement*> statements;
    // Statement parsing
    while (ts.next() != "endmodule") {
      statements.push_back(parse_statement(ts));
    }
    
    parse_token("endmodule", ts);

    assert(!ts.chars_left());

    return verilog_module(port_names, statements);
  }

  statement* parse_statement(const std::string& stmt_string) {
    token_stream ts(tokenize(stmt_string));
    return parse_statement(ts);
  }

  expression* parse_expression(const std::string& stmt_string) {
    token_stream ts(tokenize(stmt_string));
    return parse_expression(ts);
  }
  
}
