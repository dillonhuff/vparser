#pragma once

#include <string>
#include <vector>

#include "token.h"

namespace vparser {

  std::vector<token> tokenize(const std::string& verilog_code);

}
