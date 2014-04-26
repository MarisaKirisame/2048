#ifndef CORE_2048_HPP
#define CORE_2048_HPP
#include <iterator>
#include <algorithm>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/range/join.hpp>
#include <boost/iterator/filter_iterator.hpp>
template< typename t >
t construct( );
template< typename iterator, size_t skip_size >
struct skipping_iterator : std::iterator< std::random_access_iterator_tag, typename std::iterator_traits< iterator >::value_type >
{
	template< typename i >
	static decltype( ( * static_cast< i * >( nullptr ) ).operator->( ) ) arrow( i ii ) { return ii.operator->( ); }
	template< typename i >
	static i arrow( i * ii ) { return ii; }
	iterator current;
	typedef decltype( * current ) elem_type;
	size_t distance_to_end;
	elem_type operator * ( ) const;
	skipping_iterator & operator ++ ( );
	skipping_iterator & operator -- ( );
	bool is_end( ) const { return distance_to_end == 0; }
	bool operator == ( const skipping_iterator & comp ) const;
	bool operator != ( const skipping_iterator & comp ) const { return ! ( *this == comp ); }
	decltype( arrow( current ) ) operator -> ( ) const { return arrow( current ); }
	elem_type operator ++ ( int );
	skipping_iterator & operator += ( int n );
	skipping_iterator operator + ( int n ) const;
	friend skipping_iterator operator + ( int n, const skipping_iterator & i ) { return i + n; }
	skipping_iterator & operator -= ( int n ) { return *this += -n; }
	skipping_iterator operator - ( int n );
	size_t operator - ( const skipping_iterator & rhs ) const;
	decltype( * current ) operator [ ]( size_t n ) const { return *( *this + n ); }
	bool operator < ( const skipping_iterator & comp ) { return ( comp - *this ) > 0; }
	bool operator > ( const skipping_iterator & comp ) { return comp < *this; }
	bool operator >= ( const skipping_iterator & comp ) { return !( *this < comp ); }
	bool operator <= ( const skipping_iterator & comp ) { return !( *this > comp ); }
	skipping_iterator( iterator current, iterator end ) : current( current ), distance_to_end( std::distance( current, end ) ) { }
	skipping_iterator( iterator current, size_t distance_to_end ) : current( current ), distance_to_end( distance_to_end ) { }
	template< typename it, typename op, size_t n >
	static void split( it begin, it end, op out );
};

struct square
{
	square & operator ++ ( ) { ++num; return * this; }
	size_t num;
	bool empty( ) const { return num == 0; }
	operator size_t( ) const { return num == 0 ? 0 : std::pow( 2, num ); }
	template< typename iterator >
	static bool can_merge( iterator begin, iterator end );
	bool operator == ( const square & comp ) { return num == comp.num; }
	template< typename iiterator, typename oiterator >
	static size_t merge( iiterator ibegin, iiterator iend, oiterator obegin );
	square( );
	explicit square( size_t num );
};
struct core_2048
{
	core_2048( ) { }
	std::array< std::array< square, 4 >, 4 > data;
	typedef decltype
	(
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
		)
	) range_type;
	range_type range( );
	typedef decltype
	(
		boost::range::join
		(
			boost::range::join
			(
				boost::make_iterator_range( data[0].cbegin( ), data[0].cend( ) ),
				boost::make_iterator_range( data[1].cbegin( ), data[1].cend( ) )
			),
			boost::range::join
			(
				boost::make_iterator_range( data[2].cbegin( ), data[2].cend( ) ),
				boost::make_iterator_range( data[3].cbegin( ), data[3].cend( ) )
			)
		)
	)
	crange_type;
	crange_type crange( ) const;
	crange_type range( ) const;
	typedef decltype( ( static_cast< range_type * >( nullptr ) )->begin( ) ) iterator;
	typedef decltype( ( static_cast< crange_type * >( nullptr )->begin( ) ) ) const_iterator;
	typedef decltype( boost::rbegin( * static_cast< crange_type * >( nullptr ) ) ) const_reverse_iterator;
	iterator begin( );
	iterator end( );
	const_iterator cbegin( ) const;
	const_iterator cend( ) const;
	const_iterator begin( ) const;
	const_iterator end( ) const;
	typedef decltype( boost::rbegin( * static_cast< range_type * >( nullptr ) ) ) reverse_iterator;
	reverse_iterator rbegin( );
	reverse_iterator rend( );
	enum direction { left, right, up, down };
	void random_add( );
	static bool empty_pred( const square & s ) { return s.empty( ); }
	typedef decltype(
			boost::make_filter_iterator(
				empty_pred,
				* static_cast< const_iterator * >( nullptr ),
				* static_cast< const_iterator * >( nullptr ) ) ) const_empty_square_iterator_type;
	typedef decltype(
			boost::make_filter_iterator(
				empty_pred,
				* static_cast< iterator * >( nullptr ),
				* static_cast< iterator * >( nullptr ) ) ) empty_square_iterator_type;
	const_empty_square_iterator_type empty_begin( ) const;
	const_empty_square_iterator_type empty_end( ) const;
	empty_square_iterator_type empty_begin( );
	empty_square_iterator_type empty_end( ) ;
	typedef std::vector
	<
		std::pair
		<
			decltype( data[0].begin( ) ),
			decltype( data[0].begin( ) )
		>
	> left_to_right_type;
	left_to_right_type left_to_right( );
	typedef std::vector
	<
		std::pair
		<
			decltype( data[0].cbegin( ) ),
			decltype( data[0].cbegin( ) )
		>
	> cleft_to_right_type;
	cleft_to_right_type left_to_right( ) const;
	typedef std::vector
	<
		std::pair
		<
			decltype( data[0].rbegin( ) ),
			decltype( data[0].rbegin( ) )
		>
	> right_to_left_type;
	right_to_left_type right_to_left( );
	typedef std::vector
	<
		std::pair
		<
			decltype( data[0].crbegin( ) ),
			decltype( data[0].crbegin( ) )
		>
	> cright_to_left_type;
	cright_to_left_type right_to_left( ) const;
	typedef
	std::vector
	<
		std::pair
		<
			skipping_iterator
			<
				iterator,
				4
			>,
			skipping_iterator
			<
				iterator,
				4
			>
		>
	>
	up_to_down_type;
	up_to_down_type up_to_down( );
	typedef
	std::vector
	<
		std::pair
		<
			skipping_iterator
			<
				const_iterator,
				4
			>,
			skipping_iterator
			<
				const_iterator,
				4
			>
		>
	>
	cup_to_down_type;
	cup_to_down_type up_to_down( ) const;
	typedef
	std::vector
	<
		std::pair
		<
			skipping_iterator
			<
				reverse_iterator,
				4
			>,
			skipping_iterator
			<
				reverse_iterator,
				4
			>
		>
	>
	down_to_up_type;
	down_to_up_type down_to_up( );
	typedef
	std::vector
	<
		std::pair
		<
			skipping_iterator
			<
				const_reverse_iterator,
				4
			>,
			skipping_iterator
			<
				const_reverse_iterator,
				4
			>
		>
	>
	cdown_to_up_type;
	cdown_to_up_type down_to_up( ) const;
	void move( direction dir, bool add_new = true );
	bool can_move( ) const;
	bool can_move( direction dir ) const;
	template< typename O >
	friend O & operator << ( O & o, const core_2048 & s );
	size_t score = 0;
	std::vector< direction > all_next_move( ) const;
	std::vector< core_2048 > make_move( direction ) const;
	std::vector< core_2048 > generate_random_add( ) const;
	core_2048 add( size_t, const_empty_square_iterator_type ) const;
	size_t empty_square_count( ) const;
};
#endif // CORE_2048_HPP
