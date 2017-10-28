#include "statement.h"

namespace vparser {

  std::ostream& operator<<(std::ostream& out, const statement& stmt) {
    stmt.print(out);
    return out;
  }

}
