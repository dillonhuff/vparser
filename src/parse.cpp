#include "parse.h"

#include "tokenize.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace vparser {

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

    statement* stmt = parse_statement(ts);

    return new always_stmt(stmt);
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

  statement* parse_stmt_block(token_stream& ts) {
    parse_token("begin", ts);

    vector<statement*> stmts;

    while (ts.next() != "end") {
      stmts.push_back(parse_statement(ts));
    }

    parse_token("end", ts);

    return new begin_stmt(stmts);
  }

  statement* parse_if(token_stream& ts) {
    parse_token("if", ts);

    parse_enclosed_tokens("(", ")", ts);

    statement* if_s = parse_statement(ts); //nullptr;
    statement* ex_s = nullptr;

    if (ts.next() == "else") {
      ts++;
      ex_s = parse_statement(ts);
    }

    return new if_stmt(if_s, ex_s);
  }

  statement* parse_module_instantiation(token_stream& ts) {
    string module_type = ts.next();
    ts++;

    cout << "Parsing module: " << module_type << endl;

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

  expression* parse_basic_expression(token_stream& ts) {
    string nx = ts.next();

    expression* expr = nullptr;
    if (is_integer(nx)) {

      if (ts.next(1) == "'") {
        ts++;

        parse_token("'", ts);

        ts++;

        expr = new num_expr();
      } else {
        parse_integer(ts);

        expr = new num_expr();
      }
    } else if (is_id(nx)) {
      ts++;
      expr = new id_expr(nx);
    } else if (nx[0] == '"') {
      ts++;
      return new string_literal_expr(nx);
    }

    return expr;
  }

  bool is_string_literal(const std::string& nx) {
    return nx[0] == '"';
  }

  // TODO: Update to take paren and bracket levels into account
  bool at_expression_end(const token_stream& ts) {
    return !ts.chars_left() || (ts.next() == ":") || (ts.next() == "]") || (ts.next() == ")") || (ts.next() == "=") || (ts.next() == ";") || (ts.next() == "<=");
  }

  bool is_binop(const std::string& nx) {
    return (nx == "&&") ||
      (nx == "&") ||
      (nx == "||") ||
      (nx == "|") ||
      (nx == "==");
  }

  expression* parse_expression(token_stream& ts) {
    cout << "REMAINING: " << ts.remaining_string() << endl;

    assert(ts.chars_left());

    vector<expression*> exprs;
    while (!at_expression_end(ts)) {
      string nx = ts.next();

      cout << "nx = " << nx << endl;

      expression* expr = nullptr;
      if (is_integer(nx) ||
          is_id(nx) ||
          is_string_literal(nx)) {
        expr = parse_basic_expression(ts);
        exprs.push_back(expr);
      } else if (nx[0] == '[') {
        assert(exprs.size() == 1);

        parse_token("[", ts);

        expression* start = parse_expression(ts);

        if (ts.next() == ":") {
          parse_token(":", ts);
          expression* end = parse_expression(ts);
          parse_token("]", ts);
          auto exp = new slice_expr(exprs.back(), start, end);
          exprs.pop_back();
          exprs.push_back(exp);
        } else {
          cout << "Single bracket expr" << endl;
          parse_token("]", ts);
          auto exp = new slice_expr(exprs.back(), start, start);
          exprs.pop_back();
          exprs.push_back(exp);
        }

      } else if (is_binop(nx)) {
        string op = nx;
        ts++;

        auto op2 = parse_expression(ts);

        assert(exprs.size() == 1);

        auto op1 = exprs[0];
        exprs.pop_back();

        exprs.push_back(new binop_expr(op, op1, op2));
      } else if (nx == "(") {
        ts++;
        expression* exp = parse_expression(ts);
        return exp;
      } else {
        assert(false);
      }
    }

    assert(exprs.size() == 1);

    return exprs[0];
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

  statement* parse_call_statement(token_stream& ts) {
    parse_token("$", ts);

    string name = ts.next();
    ts++;

    cout << "name = " << name << endl;

    parse_token("(", ts);

    vector<expression*> args;
    while (true) {
      cout << "next = " << ts.next() << endl;

      expression* expr = parse_expression(ts);
      args.push_back(expr);

      if (ts.next() == ",") {
        ts++;
      } else if (ts.next() == ")") {
        break;
      }
    }

    parse_token(")", ts);

    parse_token(";", ts);

    return new call_stmt(name, args);
  }

  statement* parse_assign(token_stream& ts) {
    parse_token("assign", ts);

    expression* lhs = parse_expression(ts);

    parse_token("=", ts);

    expression* rhs = parse_expression(ts);

    cout << "Remainder after assign = " << ts.remaining_string() << endl;

    parse_token(";", ts);

    return new assign_stmt(lhs, rhs);
  }

  statement* parse_blocking_assign(token_stream& ts) {
    expression* lhs = parse_expression(ts);

    parse_token("=", ts);

    expression* rhs = parse_expression(ts);

    parse_token(";", ts);

    return new blocking_assign_stmt(lhs, rhs);
  }

  statement* parse_non_blocking_assign(token_stream& ts) {
    expression* lhs = parse_expression(ts);

    parse_token("<=", ts);

    expression* rhs = parse_expression(ts);

    parse_token(";", ts);

    return new non_blocking_assign_stmt(lhs, rhs);
  }
  
  statement* parse_statement(token_stream& ts) {
    string ns = ts.next();

    if ((ns == "input") || (ns == "output") || (ns == "reg") || (ns == "wire")) {
      return parse_declaration(ts);
    } else if (ns == "always") {
      return parse_always(ts);
    } else if (ns == "if") {
      return parse_if(ts);
    } else if (ns == "assign") {
      return parse_assign(ts);
    } else if (ns == "begin") {
      return parse_stmt_block(ts);
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
      } else if (nn == "=") {
        return parse_blocking_assign(ts);
      } else if (nn == "<=") {
        return parse_non_blocking_assign(ts);
      }

      return parse_id_statement(ts);

    } else if (ns == ";") {
      ts++;
      return new empty_stmt();
    } else if (ns == "$") {
      return parse_call_statement(ts);
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
    auto toks = tokenize(stmt_string);
    token_stream ts(toks);
    return parse_expression(ts);
  }

}
