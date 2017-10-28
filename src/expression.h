#pragma once

#include <string>

namespace vparser {

  enum expression_type {
    EXPRESSION_ID,
    EXPRESSION_NUM,
    EXPRESSION_SLICE
  };

  class expression {
  public:

    virtual expression_type get_type() const = 0;
  };

  class num_expr : public expression {
  public:
    virtual expression_type get_type() const {
      return EXPRESSION_NUM;
    }
  };

  class id_expr : public expression {
    std::string name;

  public:

    id_expr(const std::string& name_) : name(name_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_ID;
    }


    std::string get_name() const { return name; }
  };

  class slice_expr : public expression {
  protected:

    expression* arg;

    int start, end;

  public:

    slice_expr(expression* const arg_,
               const int start_,
               const int end_) : arg(arg_), start(start_), end(end_) {}

    virtual expression_type get_type() const {
      return EXPRESSION_SLICE;
    }

    int get_start() const { return start; }
    int get_end() const { return end; }

    expression* get_arg() const { return arg; }

  };
  
}
