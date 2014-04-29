#ifndef SQUARE_HPP
#define SQUARE_HPP
#include <cstddef>
#include <cmath>
#include <iterator>
struct square
{
	square & operator ++ ( );
	size_t num;
	bool empty( ) const;
	operator size_t( ) const;
	template< typename iterator >
	static bool can_merge( iterator begin, iterator end )
	{
		if ( std::distance( begin, end ) <= 1 ) { return false; }
		else
		{
			auto second = begin;
			++second;
			if ( ( begin->num != 0 && second->num == 0 ) || ( begin->num != 0 && begin->num == second->num ) ) { return true; }
			else { return can_merge( second, end ); }
		}
	}
	bool operator == ( const square & comp );
	template< typename iiterator, typename oiterator >
	static size_t merge( iiterator ibegin, iiterator iend, oiterator obegin );
	square( );
	explicit square( size_t num );
};
#endif // SQUARE_HPP
