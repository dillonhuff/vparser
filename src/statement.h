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

    virtual ~statement() {}
  };

  class decl_stmt : public statement {
  public:

    statement_type get_type() const {
      return STATEMENT_DECL;
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

    statement* if_exe;
    statement* else_exe;

  public:

    if_stmt(statement* const if_exe_) :
      if_exe(if_exe_), else_exe(nullptr) {}

    if_stmt(statement* const if_exe_,
            statement* const else_exe_) :
      if_exe(if_exe_), else_exe(else_exe_) {}

    statement* get_if_exe() { return if_exe; }
    statement* get_else_exe() { return else_exe; }
    
    statement_type get_type() const {
      return STATEMENT_IF;
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

    statement_type get_type() const {
      return STATEMENT_BEGIN;
    }

    virtual void print(std::ostream& out) const {
      out << " " << std::endl;
    }

  };

  class case_stmt : public statement {
  protected:

    std::vector<std::pair<expression*, statement*>> inner_cases;

    statement* default_stmt;

  public:

    case_stmt(const std::vector<std::pair<expression*, statement*>> inner_cases_) :
      inner_cases(inner_cases_) {}

    case_stmt(const std::vector<std::pair<expression*, statement*>> inner_cases_,
              statement* default_stmt_) :
      inner_cases(inner_cases_), default_stmt(default_stmt_) {}
    
    statement_type get_type() const {
      return STATEMENT_CASE;
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

    virtual void print(std::ostream& out) const {
      out << " " << std::endl;
    }

  };
  
}
