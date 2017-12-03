#pragma once

#include <vector>
#include <iostream>

#include "expression.h"

namespace vparser {

  enum statement_type {
    STATEMENT_DECL,
    STATEMENT_ALWAYS,
    STATEMENT_BEGIN,
    STATEMENT_IF,
    STATEMENT_ASSIGN,
    STATEMENT_CASE,
    STATEMENT_EMPTY,
    STATEMENT_MODULE_INSTANTIATION,
    STATEMENT_BLOCKING_ASSIGN,
    STATEMENT_NON_BLOCKING_ASSIGN,
    STATEMENT_CALL
  };

  class statement {
  public:

    virtual statement_type get_type() const = 0;

    virtual void print(std::ostream& out) const = 0;

    virtual std::string to_string(const int indent_level) const {
      return indent(indent_level) + "STATEMENT PLACEHOLDER";
    }

    virtual ~statement() {}
  };

  class decl_stmt : public statement {

    std::string category;
    std::string storage_type;
    expression* w_start;
    expression* w_end;
    std::string name;
    expression* init_value;

  public:

    decl_stmt(const std::string& category_,
              const std::string& storage_type_,
              expression* const w_start_,
              expression* const w_end_,
              const std::string& name_,
              expression* const init_value_) :
      category(category_), storage_type(storage_type_), w_start(w_start_), w_end(w_end_), name(name_), init_value(init_value_) {}

    statement_type get_type() const {
      return STATEMENT_DECL;
    }

    std::string to_string(const int lvl) const {
      std::string str = indent(lvl) + category + " ";

      if ((w_end != nullptr) &&
          (w_start != nullptr)) {
        str +=  "[ " + w_end->to_string() + " : " + w_start->to_string() + " ] ";
      }

      str += storage_type + " " + name;
      if (init_value != nullptr) {
        str += " = " + init_value->to_string();
      }

      str += ";";
        
      return str;
    }
    
    virtual void print(std::ostream& out) const {
      out << "declaration" << std::endl;
    }
  };

  std::ostream& operator<<(std::ostream& out, const statement& stmt);
  
  class always_stmt : public statement {

    //std::vector<statement*> stmts;

    statement* stmt;

  public:

    //always_stmt(const std::vector<statement*>& stmts_) : stmts(stmts_) {}
    always_stmt(statement* const stmt_) : stmt(stmt_) {}

    statement_type get_type() const {
      return STATEMENT_ALWAYS;
    }

    statement* get_statement() const {
      return stmt;
    }

    std::string to_string(const int lvl) const {
      std::string str =
        indent(lvl) + "always @(INSERT TOKENS)\n" +
        get_statement()->to_string(lvl + 1);
      return str;
    }
    
    virtual void print(std::ostream& out) const {
      out << "always () begin" << std::endl;
      // for (auto& stmt : get_statement_block()) {
      //   out << *stmt << std::endl;
      // }

      out << std::endl;
    }

  };

  class if_stmt : public statement {

  protected:

    expression* condition;
    statement* if_exe;
    statement* else_exe;

  public:

    if_stmt(expression* const condition_, statement* const if_exe_) :
      condition(condition_), if_exe(if_exe_), else_exe(nullptr) {}

    if_stmt(expression* const condition_,
            statement* const if_exe_,
            statement* const else_exe_) :
      condition(condition_), if_exe(if_exe_), else_exe(else_exe_) {}

    statement* get_if_exe() const { return if_exe; }
    statement* get_else_exe() const { return else_exe; }
    
    statement_type get_type() const {
      return STATEMENT_IF;
    }

    std::string to_string(const int lvl) const {
      std::string str = indent(lvl) + "if (" + condition->to_string() + ")\n";
      str += get_if_exe()->to_string(lvl + 1);
      if (get_else_exe() != nullptr) {
        str += indent(lvl) + "else\n" + get_else_exe()->to_string(lvl + 1);
      }
      
      return str;
    }

    virtual void print(std::ostream& out) const {
      out << "if" << std::endl;
    }

  };
  
  class empty_stmt : public statement {
  public:

    statement_type get_type() const {
      return STATEMENT_EMPTY;
    }

    virtual void print(std::ostream& out) const {
      out << " " << std::endl;
    }

    virtual std::string to_string(const int) const {
      return "";
    }

  };

  class begin_stmt : public statement {
  protected:

    std::vector<statement*> stmts;

  public:

    begin_stmt(const std::vector<statement*>& stmts_) :
      stmts(stmts_) {}

    std::vector<statement*> get_statements() const {
      return stmts;
    }

    std::string to_string(const int lvl) const {
      std::string str = indent(lvl) + "begin\n";
      for (auto& stmt : get_statements()) {
        str += stmt->to_string(lvl + 1) + "\n";
      }
      str += "\n" + indent(lvl) + "end\n";
      return str;
    }
    
    statement_type get_type() const {
      return STATEMENT_BEGIN;
    }

    virtual void print(std::ostream& out) const {
      out << to_string(0) << std::endl;
    }

  };

  class case_stmt : public statement {
  protected:

    std::vector<std::pair<expression*, statement*>> inner_cases;

    statement* default_stmt;

  public:

    case_stmt(const std::vector<std::pair<expression*, statement*>> inner_cases_) :
      inner_cases(inner_cases_), default_stmt(nullptr) {}

    case_stmt(const std::vector<std::pair<expression*, statement*>> inner_cases_,
              statement* default_stmt_) :
      inner_cases(inner_cases_), default_stmt(default_stmt_) {}
    
    statement_type get_type() const {
      return STATEMENT_CASE;
    }

    std::string to_string(const int lvl) const {
      std::string str = indent(lvl) + "case (CASE EXPR)\n";

      for (auto& cs : inner_cases) {
        str += indent(lvl + 1) + cs.first->to_string() + ": " + cs.second->to_string(0) + "\n";
      }

      if (default_stmt != nullptr) {
        str += indent(lvl + 1) + "default: " + default_stmt->to_string(0) + "\n";
      }

      str += "\n" + indent(lvl) + "endcase\n";

      return str;
    }
    
    std::vector<std::pair<expression*, statement*> >
    get_cases() const {
      return inner_cases;
    }

    virtual void print(std::ostream& out) const {
      out << "case ()" << std::endl;
    }

  };

  class assign_stmt : public statement {
  protected:

    expression* lhs;
    expression* rhs;

  public:

    assign_stmt(expression* const lhs_,
                expression* const rhs_) : lhs(lhs_), rhs(rhs_) {}

    statement_type get_type() const {
      return STATEMENT_ASSIGN;
    }

    virtual void print(std::ostream& out) const {
      out << " " << std::endl;
    }

    std::string to_string(const int lvl) const {
      std::string str =
        indent(lvl) + "assign " + lhs->to_string() + " = " + rhs->to_string() + ";";
      return str;
    }
    
    expression* get_lhs() const {
      return lhs;
    }

    expression* get_rhs() const {
      return rhs;
    }

  };

  class blocking_assign_stmt : public statement {
  protected:

    expression* lhs;
    expression* rhs;

  public:

    blocking_assign_stmt(expression* const lhs_,
                expression* const rhs_) : lhs(lhs_), rhs(rhs_) {}

    statement_type get_type() const {
      return STATEMENT_BLOCKING_ASSIGN;
    }

    std::string to_string(const int lvl) const {
      std::string str =
        indent(lvl) + lhs->to_string() + " = " + rhs->to_string() + ";";
      return str;
    }
    
    virtual void print(std::ostream& out) const {
      out << " " << std::endl;
    }

    expression* get_lhs() const {
      return lhs;
    }

    expression* get_rhs() const {
      return rhs;
    }

  };

  class non_blocking_assign_stmt : public statement {
  protected:

    expression* lhs;
    expression* rhs;

  public:

    non_blocking_assign_stmt(expression* const lhs_,
                expression* const rhs_) : lhs(lhs_), rhs(rhs_) {}

    statement_type get_type() const {
      return STATEMENT_NON_BLOCKING_ASSIGN;
    }

    std::string to_string(const int lvl) const {
      std::string str =
        indent(lvl) + lhs->to_string() + " <= " + rhs->to_string() + ";";
      return str;
    }
    
    virtual void print(std::ostream& out) const {
      out << " " << std::endl;
    }

    expression* get_lhs() const {
      return lhs;
    }

    expression* get_rhs() const {
      return rhs;
    }

  };

  class call_stmt : public statement {
    std::string name;
    std::vector<expression*> args;

  public:

    call_stmt(const std::string& name_,
              const std::vector<expression*>& args_) : name(name_), args(args_) {}

    std::string to_string(const int lvl) const {
      std::string str =
        indent(lvl) + "$" + name + "( ";

      for (int i = 0; i < args.size(); i++) {
        str += args[i]->to_string();

        if (i < args.size() - 1) {
          str += ", ";
        }
      }
      str += ");";
      return str;
    }
    
    statement_type get_type() const {
      return STATEMENT_CALL;
    }

    virtual void print(std::ostream& out) const {
      out << " " << std::endl;
    }

  };
  
  class module_instantiation_stmt : public statement {
    std::string module_type, name;

    std::vector<std::pair<std::string, expression*> > port_assignments;

  public:

    module_instantiation_stmt(const std::string& module_type_,
                              const std::string& name_,
                              const std::vector<std::pair<std::string, expression*> > port_assignments_) :
      module_type(module_type_),
      name(name_),
      port_assignments(port_assignments_) {}

    statement_type get_type() const {
      return STATEMENT_MODULE_INSTANTIATION;
    }

    std::string get_module_type() const { return module_type; }
    std::string get_name() const { return name; }

    std::vector<std::pair<std::string, expression*> > get_port_assignments() const {
      return port_assignments;
    }

    std::string to_string(const int lvl) const {
      std::string str = indent(lvl) + module_type + " " + name + "(";

      for (int i = 0; i < port_assignments.size(); i++) {
        str += "." + port_assignments[i].first + "(" + port_assignments[i].second->to_string() + ")";
        if (i < (port_assignments.size() - 1)) {
          str += ", ";
        }
      }
      str += ");";

      return str;
    }

    virtual void print(std::ostream& out) const {
      out << " " << std::endl;
    }

  };
  
}
