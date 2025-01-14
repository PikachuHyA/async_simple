module;
#include <unordered_map>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:unordered_map;
export namespace std {
    using std::unordered_map;
}

#if defined(__GLIBCXX__)
export namespace std {
    namespace __detail {
       using std::__detail::_Node_const_iterator;
#if __GLIBCXX__ < 20220506
       using std::__detail::operator==;
#endif
    }
}
#endif
