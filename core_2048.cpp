#include "core_2048.hpp"
template< typename iterator, size_t skip_size >
skipping_iterator< iterator, skip_size > skipping_iterator< iterator, skip_size >::operator +(int n) const
{
	skipping_iterator tem = *this;
	return tem += n;
}

template< typename iterator, size_t skip_size >
typename skipping_iterator< iterator, skip_size >::elem_type skipping_iterator< iterator, skip_size >::operator *() const { return * current; }

template< typename iterator, size_t skip_size >
skipping_iterator< iterator, skip_size > & skipping_iterator< iterator, skip_size >::operator ++( )
{
	if ( distance_to_end <= skip_size ) { distance_to_end = 0; }
	else
	{
		std::advance( current, skip_size );
		distance_to_end -= skip_size;
	}
	return * this;
}

template< typename iterator, size_t skip_size >
skipping_iterator< iterator, skip_size > &skipping_iterator< iterator, skip_size >::operator --()
{
	current -= skip_size;
	distance_to_end += skip_size;
	return * this;
}

template< typename iterator, size_t skip_size >
bool skipping_iterator< iterator, skip_size >::operator ==(const skipping_iterator< iterator, skip_size > & comp) const
{ return ( is_end( ) && comp.is_end( ) ) || ( current == comp.current && distance_to_end == comp.distance_to_end ); }
template< typename iterator, size_t skip_size >
typename skipping_iterator< iterator, skip_size >::elem_type skipping_iterator< iterator, skip_size >::operator ++(int)
{
	auto tem = **this;
	*this++;
	return tem;
}
template< typename iterator, size_t skip_size >
skipping_iterator< iterator, skip_size > & skipping_iterator< iterator, skip_size >::operator +=(int n)
{
	for( int inc = n > 0 ? 1 : -1; n != 0; n-=inc ) { if( n > 0 ) { ++*this; } else { --*this; } }
	return * this;
}

template< typename iterator, size_t skip_size >
size_t skipping_iterator< iterator, skip_size >::operator -(const skipping_iterator< iterator, skip_size > & rhs) const
{
	if ( ( rhs.distance_to_end - distance_to_end ) % skip_size == 0 )
	{ return ( rhs.distance_to_end - distance_to_end ) / skip_size; }
	else
	{ return ( ( rhs.distance_to_end - distance_to_end ) - ( rhs.distance_to_end - distance_to_end ) % skip_size ) / skip_size + 1; }
}

template< typename iterator, size_t skip_size >
skipping_iterator< iterator, skip_size > skipping_iterator< iterator, skip_size >::operator -(int n)
{
	skipping_iterator< iterator, skip_size > tem = *this;
	return tem -= n;
}

template< typename iterator, size_t skip_size >
template< typename it, typename op, size_t n >
void skipping_iterator< iterator, skip_size >::split( it begin, it end, op out )
{
	std::transform( boost::counting_iterator< size_t >( 0 ),
									boost::counting_iterator< size_t >( n ),
									out,
									[&]( size_t )
	{
		auto ret = skipping_iterator< it, n >( begin, end );
		++begin;
		return std::make_pair( ret, skipping_iterator< it, n >( begin, 0 ) );
	} );
}
template< typename iterator >
bool square::can_merge(iterator begin, iterator end)
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
template< typename iiterator, typename oiterator >
size_t square::merge(iiterator ibegin, iiterator iend, oiterator obegin)
{
	size_t retu = 0;
	if ( ibegin == iend ) { }
	if ( std::distance( ibegin, iend ) == 1 ) { *obegin = *ibegin; }
	else
	{
		auto second = ibegin;
		++second;
		std::vector< square > tem;
		std::copy_if(
					ibegin,
					iend,
					std::back_inserter( tem ),
					[&]( const square & s ) { return ! s.empty( ); } );
		std::vector< square > ret;
		while ( ! tem.empty( ) )
		{
			square s =  tem.back( );
			tem.pop_back( );
			if ( ( ! tem.empty( ) ) && ( tem.back( ) == s ) )
			{
				tem.pop_back( );
				++s;
				retu += s;
			}
			ret.push_back( std::move( s ) );
		}
		obegin = std::transform(
							 boost::counting_iterator< size_t >( 0 ),
							 boost::counting_iterator< size_t >( std::distance( ibegin, iend ) - ret.size( ) ),
							 obegin,
							 []( size_t ){ return square( ); } );
		std::copy( ret.rbegin( ), ret.rend( ), obegin );
	}
	return retu;
}

square::square() : square( 0 ) { }


square::square(size_t num) : num( num ) { }

core_2048::range_type core_2048::range( )
{
	return
		boost::range::join
		(
			boost::range::join
			(
				boost::make_iterator_range( data[0].begin( ), data[0].end( ) ),
				boost::make_iterator_range( data[1].begin( ), data[1].end( ) )
			),
			boost::range::join
			(
				boost::make_iterator_range( data[2].begin( ), data[2].end( ) ),
				boost::make_iterator_range( data[3].begin( ), data[3].end( ) )
			)
		);
}


core_2048::crange_type core_2048::crange( ) const
{
	return
		boost::range::join
		(
			boost::range::join
			(
				boost::make_iterator_range( data[0].begin( ), data[0].end( ) ),
				boost::make_iterator_range( data[1].begin( ), data[1].end( ) )
			),
			boost::range::join
			(
				boost::make_iterator_range( data[2].begin( ), data[2].end( ) ),
				boost::make_iterator_range( data[3].begin( ), data[3].end( ) )
			)
			);
}

core_2048::crange_type core_2048::range() const { return crange( ); }

core_2048::iterator core_2048::begin() { return range( ).begin( ); }

core_2048::iterator core_2048::end() { return range( ).end( ); }

core_2048::const_iterator core_2048::cbegin() const { return begin(); }

core_2048::const_iterator core_2048::cend() const { return end(); }

core_2048::const_iterator core_2048::begin() const { return range( ).begin( ); }

core_2048::const_iterator core_2048::end() const { return range( ).end( ); }

core_2048::reverse_iterator core_2048::rbegin() { return boost::rbegin( range( ) ); }

core_2048::reverse_iterator core_2048::rend() { return boost::rend( range( ) ); }

void core_2048::random_add()
{
	largest_square_cache.reset( );
	std::random_device rd;
	auto i_begin = empty_begin( );
	auto i_end = empty_end( );
	for ( auto distance = std::distance( i_begin, i_end );i_begin != i_end; ++i_begin )
	{
		if ( std::uniform_real_distribution<>( )( rd ) <= 1 / static_cast< double >( distance ) )
		{
			*i_begin = square( std::uniform_int_distribution<>(1,2)( rd ) );
			break;
		}
		--distance;
	}
}

core_2048::const_empty_square_iterator_type core_2048::empty_begin() const { return boost::make_filter_iterator( empty_pred, begin( ), end( ) ); }

core_2048::const_empty_square_iterator_type core_2048::empty_end() const { return boost::make_filter_iterator( empty_pred, end( ), end( ) ); }

core_2048::empty_square_iterator_type core_2048::empty_begin() { return boost::make_filter_iterator( empty_pred, begin( ), end( ) ); }

core_2048::empty_square_iterator_type core_2048::empty_end() { return boost::make_filter_iterator( empty_pred, end( ), end( ) ); }

core_2048::cleft_to_right_type core_2048::left_to_right() const
{
	cleft_to_right_type ret;
	std::transform
			(
				data.cbegin( ),
				data.cend( ),
		std::back_inserter( ret ),
		[]( decltype( data[0] ) data ){ return std::make_pair( data.cbegin( ), data.cend( ) ); }
	);
	return ret;
}

core_2048::cright_to_left_type core_2048::right_to_left() const
{
	cright_to_left_type ret;
	std::transform
	(
		data.begin( ),
		data.end( ),
		std::back_inserter( ret ),
		[]( decltype( data[0] ) data ){ return std::make_pair( data.crbegin( ), data.crend( ) ); }
	);
	return ret;
}

core_2048::cup_to_down_type core_2048::up_to_down() const
{
	cup_to_down_type ret;
	skipping_iterator< iterator, 4 >::split
	<
		decltype( crange( ).begin( ) ),
		decltype( std::back_inserter( ret ) ),
		4
	>
	(
		crange( ).begin( ),
		crange( ).end( ),
		std::back_inserter( ret )
	);
	return ret;
}

core_2048::cdown_to_up_type core_2048::down_to_up() const
{
	cdown_to_up_type ret;
	skipping_iterator< iterator, 4 >::split
	<
		decltype( boost::rbegin( crange( ) ) ),
		decltype( std::back_inserter( ret ) ),
		4
	>
	(
		boost::rbegin( crange( ) ),
		boost::rend( crange( ) ),
		std::back_inserter( ret )
	);
	return ret;
}

void core_2048::move(core_2048::direction dir, bool add_new)
{
	largest_square_cache.reset( );
	size_t ret = 0;
	if ( dir == left )
	{
		auto d = right_to_left( );
		std::for_each(
					d.begin( ),
					d.end( ),
					[&]( decltype( d[0] ) data ){ ret += square::merge( data.first, data.second, data.first ); } );
	}
	else if ( dir == right )
	{
		auto d = left_to_right( );
		std::for_each(
					d.begin( ),
					d.end( ),
					[&]( decltype( d[0] ) data ){ ret += square::merge( data.first, data.second, data.first ); } );
	}
	else if ( dir == up )
	{
		auto d = down_to_up( );
		std::for_each(
					d.begin( ),
					d.end( ),
					[&]( decltype( d[0] ) data ){ ret += square::merge( data.first, data.second, data.first ); } );
	}
	else
	{
		assert( dir ==  down );
		auto d = up_to_down( );
		std::for_each(
					d.begin( ),
					d.end( ),
					[&]( decltype( d[0] ) data ){ ret += square::merge( data.first, data.second, data.first ); } );
	}
	if ( add_new ) { random_add( ); }
	score += ret;
}

bool core_2048::can_move( ) const { return can_move( up ) || can_move( down ) || can_move( left ) || can_move( right ); }

bool core_2048::can_move(core_2048::direction dir) const
{
	if ( dir == left )
	{
		auto d = right_to_left( );
		return std::find_if(
					d.begin( ),
					d.end( ),
					[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
	}
	else if ( dir == right )
	{
		auto d = left_to_right( );
		return std::find_if(
					d.begin( ),
					d.end( ),
					[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
	}
	else if ( dir == up )
	{
		auto d = down_to_up( );
		return std::find_if(
					d.begin( ),
					d.end( ),
					[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
	}
	else
	{
		assert( dir ==  down );
		auto d = up_to_down( );
		return std::find_if(
					d.begin( ),
					d.end( ),
					[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
	}
}

std::vector<core_2048::direction> core_2048::all_next_move() const
{
	std::vector< direction > ret;
	for( direction i : { up, left, down, right } )
	{
		if ( can_move( i ) )
		{ ret.push_back( i ); }
	}
	return ret;
}

std::vector<core_2048> core_2048::make_move( core_2048::direction dir ) const
{
	core_2048 next = *this;
	next.move( dir, false );
	return next.generate_random_add( );
}

std::vector<core_2048> core_2048::generate_random_add() const
{
	std::vector<core_2048> ret;
	auto beg =  empty_begin( );
	auto end = empty_end( );
	for ( ; beg != end; ++beg )
	{
		ret.push_back( add( 2, beg ) );
		ret.push_back( add( 4, beg ) );
	}
	return ret;
}

core_2048 core_2048::add(size_t, core_2048::const_empty_square_iterator_type ii ) const
{
	core_2048 ret( * this );
	auto i = ret.empty_begin( );
	std::advance( i, std::distance( empty_begin( ), ii ) );
	return ret;
}
size_t core_2048::empty_square_count() const { return std::count_if( begin(), end(), empty_pred ); }

square core_2048::largest_square() const
{
	if ( largest_square_cache.get( ) == nullptr )
	{ largest_square_cache = std::make_shared< square >( * std::max_element( begin( ), end( ) ) ); }
	return * largest_square_cache;
}

bool core_2048::largest_on_edge() const
{
	auto res = largest_square( );
	return
			res == data[0][1] || res == data[0][2] ||
			res == data[1][0] || res == data[2][0] ||
			res == data[2][0] || res == data[2][2] ||
			res == data[3][1] || res == data[3][2] ||
			largest_on_corner( );
}

bool core_2048::largest_on_corner() const
{
	auto res = largest_square( );
	return res == data[0][0] || res == data[0][3] || res == data[3][0] || res == data[3][3];
}

template< typename O >
O & operator <<(O & o, const core_2048 & s)
{
	o
			<< s.data[0][0] <<  "|" << s.data[0][1] << "|" << s.data[0][2] << "|" << s.data[0][3] << std::endl
			<< s.data[1][0] <<  "|" << s.data[1][1] << "|" << s.data[1][2] << "|" << s.data[1][3] << std::endl
			<< s.data[2][0] <<  "|" << s.data[2][1] << "|" << s.data[2][2] << "|" << s.data[2][3] << std::endl																																																																																																																																	<< s.data[3][0] <<  "|" << s.data[3][1] << "|" << s.data[3][2] << "|" << s.data[3][3] << std::endl;
	return o;
}

core_2048::down_to_up_type core_2048::down_to_up()
{
	down_to_up_type ret;
	skipping_iterator< iterator, 4 >::split
	<
		decltype( boost::rbegin( range( ) ) ),
		decltype( std::back_inserter( ret ) ),
		4
	>
	(
		boost::rbegin( range( ) ),
		boost::rend( range( ) ),
		std::back_inserter( ret )
	);
	return ret;
}

core_2048::up_to_down_type core_2048::up_to_down()
{
	up_to_down_type ret;
	skipping_iterator< iterator, 4 >::split
	<
		decltype( range( ).begin( ) ),
		decltype( std::back_inserter( ret ) ),
		4
	>
	(
		range( ).begin( ),
		range( ).end( ),
		std::back_inserter( ret )
	);
	return ret;
}

core_2048::right_to_left_type core_2048::right_to_left()
{
	right_to_left_type ret;
	std::transform
			(
				data.begin( ),
				data.end( ),
				std::back_inserter( ret ),
				[]( decltype( data[0] ) data ){ return std::make_pair( data.rbegin( ), data.rend( ) ); }
	);
	return ret;
}

core_2048::left_to_right_type core_2048::left_to_right()
{
	left_to_right_type ret;
	std::transform
			(
				data.begin( ),
				data.end( ),
				std::back_inserter( ret ),
				[]( decltype( data[0] ) data ){ return std::make_pair( data.begin( ), data.end( ) ); }
	);
	return ret;
}
