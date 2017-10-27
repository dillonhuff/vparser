#pragma once

#include <vector>

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

    virtual ~statement() {}
  };

  class decl_stmt : public statement {
  public:

    statement_type get_type() const {
      return STATEMENT_DECL;
    }
  };

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
  };

  class if_stmt : public statement {
  public:

    statement_type get_type() const {
      return STATEMENT_IF;
    }
  };
  
  class empty_stmt : public statement {
  public:

    statement_type get_type() const {
      return STATEMENT_EMPTY;
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
  };
  
}
