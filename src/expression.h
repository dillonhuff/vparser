#pragma once

#include <string>

namespace vparser {

  enum expression_type {
    EXPRESSION_ID,
    EXPRESSION_NUM
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
  
}
