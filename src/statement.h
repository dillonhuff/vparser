#pragma once

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
}
