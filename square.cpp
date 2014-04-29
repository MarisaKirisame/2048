#include "square.hpp"
#include <iterator>
square &square::operator ++() { ++num; return * this; }

bool square::empty() const { return num == 0; }

bool square::operator ==(const square & comp) { return num == comp.num; }

square::operator size_t() const { return num == 0 ? 0 : std::pow( 2, num ); }

square::square() : square( 0 ) { }

square::square(size_t num) : num( num ) { }
