module;
#include <limits>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:limits;
export namespace std {
    using std::numeric_limits;
}