#include <Parser/Expr.hh>

namespace ktpp::parser {
bool GenericParameters::is_present() { return !type.empty(); }
bool Lambda::is_generic() { return generics->is_present(); }
}  // namespace ktpp::parser