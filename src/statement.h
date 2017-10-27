#pragma once

#include <vector>

namespace vparser {

  enum statement_type {
    STATEMENT_DECL,
    STATEMENT_ALWAYS,
    STATEMENT_IF,
    STATEMENT_ASSIGN,
    STATEMENT_CASE
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


  class case_stmt : public statement {
  public:

    statement_type get_type() const {
      return STATEMENT_CASE;
    }
  };
  
}
