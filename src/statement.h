#pragma once

#include <vector>
#include <iostream>

#include "expression.h"

namespace vparser {

  enum statement_type {
    STATEMENT_DECL,
    STATEMENT_ALWAYS,
    STATEMENT_IF,
    STATEMENT_ASSIGN,
    STATEMENT_CASE,
    STATEMENT_EMPTY
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

    std::vector<statement*> stmts;

  public:

    always_stmt(const std::vector<statement*>& stmts_) : stmts(stmts_) {}

    statement_type get_type() const {
      return STATEMENT_ALWAYS;
    }

    std::vector<statement*> get_statement_block() const {
      return stmts;
    }

    virtual void print(std::ostream& out) const {
      out << "always () begin" << std::endl;
      for (auto& stmt : get_statement_block()) {
        out << *stmt << std::endl;
      }

      out << std::endl;
    }

  };

  class if_stmt : public statement {
  public:

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

  
}
