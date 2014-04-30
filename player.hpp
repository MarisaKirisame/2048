#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "core_2048.hpp"
#include <functional>
#include <vector>
#include <utility>
#include <array>
struct player
{
	virtual void update( const core_2048 & data, const core_2048 & next_step ) = 0;
	virtual void update( const core_2048 & data, core_2048::direction dir );
	virtual void add_evaluation( std::function< double ( const core_2048 & ) > && ) = 0;
	virtual void add_evaluation( const std::function< double ( const core_2048 & ) > & ) = 0;
	virtual double evaluate( const core_2048 & c ) = 0;
	virtual ~player( );
	double learn_rate = 0.1;
	double inital_weight = 10;
	size_t search_depth = 0;
	virtual core_2048::direction find_move( const core_2048 & );
	virtual double evaluate_positsion_minmax( const core_2048 & c, size_t depth );
	virtual void update( const core_2048 & data, core_2048::direction data_dir, const core_2048 & next_step ) = 0;
};
struct SARSA_player : player
{
	virtual double evaluate( const core_2048 & c, const core_2048::direction dir );
	virtual double evaluate( const core_2048 & c ) override;
	core_2048::direction instant_find_move( const core_2048 & c );
	std::vector< std::pair< std::function< double ( const core_2048 & ) >, std::array< double, 8 > > > evaluation;
	void add_evaluation( std::function< double ( const core_2048 & ) > && ) override;
	void add_evaluation( const std::function< double ( const core_2048 & ) > & ) override;
	void update( const core_2048 & data, const core_2048 & next_step ) override;
	virtual void update( const core_2048 & data, core_2048::direction data_dir, const core_2048 & next_step );
};
struct SRS_player : player
{
	std::vector< std::pair< std::function< double ( const core_2048 & ) >, std::pair< double, double > > > evaluation;
	void add_evaluation( std::function< double ( const core_2048 & ) > && ) override;
	void add_evaluation( const std::function< double ( const core_2048 & ) > & ) override;
	void update( const core_2048 & data, const core_2048 & next_step ) override;
	SRS_player( );
	virtual double evaluate( const core_2048 & c ) override;
	virtual void update( const core_2048 & data, core_2048::direction data_dir, const core_2048 & next_step );
};
template< typename BC >
struct player_wrapper : BC
{
	player_wrapper( )
	{
		for ( size_t s : { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 } )
		{
			this->add_evaluation( [=]( const core_2048 & c ){ return c.empty_square_count( ) == s; } );
			for ( size_t i : { 0, 1, 2, 3 } )
			{
				for ( size_t j : { 0, 1, 2, 3 } )
				{
					for ( size_t z : { 0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072 } )
					{ this->add_evaluation( [=]( const core_2048 & c ){ return c.data[i][j] == z && c.empty_square_count( ) == s; } ); }
					this->add_evaluation( [=]( const core_2048 & c ){ return c.largest_square( ) == c.data[i][j] && c.empty_square_count( ) == s; } );
					for ( size_t a : { 0, 1, 2, 3 } )
					{
						for ( size_t b : { 0, 1, 2, 3 } )
						{
							if ( i * 4 + j > a * 4 + b )
							{
								for ( size_t z : { 0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072 } )
								{
									this->add_evaluation( [=]( const core_2048 & c )
									{ return c.data[i][j] > c.data[a][b] && c.data[a][b] == z && c.empty_square_count( ) == s; } );
									this->add_evaluation( [=]( const core_2048 & c )
									{ return c.data[i][j] == c.data[a][b] && c.data[a][b] == z && c.empty_square_count( ) == s; } );
									this->add_evaluation( [=]( const core_2048 & c )
									{ return c.data[i][j] > c.data[a][b] && c.data[i][j] == z && c.empty_square_count( ) == s; } );
								}
							}
						}
					}
				}
			}
			for ( core_2048::direction dir : { core_2048::up, core_2048::down, core_2048::left, core_2048::right } )
			{ this->add_evaluation( [=]( const core_2048 & c ){ return c.can_move( dir ) && c.empty_square_count( ) == s; } ); }
			for ( size_t ii = 0; ii < 16; ++ii )
			for ( size_t z : { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072 } )
			{ this->add_evaluation( [=]( const core_2048 & c ){ return c.count( z ) == ii && c.empty_square_count( ) == s; } ); }
			for ( size_t z : { 0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072 } )
			{
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[0][0] < c.data[0][1] && c.data[0][0] < c.data[1][0] && c.data[0][0] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[0][3] < c.data[0][2] && c.data[0][3] < c.data[1][3] && c.data[0][3] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[3][0] < c.data[3][1] && c.data[3][0] < c.data[2][0] && c.data[3][0] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[3][3] < c.data[3][2] && c.data[3][3] < c.data[2][3] && c.data[3][3] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[0][1] < c.data[0][0] && c.data[0][1] < c.data[0][2] && c.data[0][1] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[0][2] < c.data[0][1] && c.data[0][2] < c.data[0][3] && c.data[0][2] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[1][0] < c.data[0][0] && c.data[1][0] < c.data[2][0] && c.data[1][0] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[2][0] < c.data[1][0] && c.data[2][0] < c.data[3][0] && c.data[2][0] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[1][3] < c.data[0][3] && c.data[1][3] < c.data[2][3] && c.data[1][3] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[2][3] < c.data[1][3] && c.data[2][3] < c.data[3][3] && c.data[2][3] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[3][1] < c.data[3][0] && c.data[3][1] < c.data[3][2] && c.data[3][1] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{ return c.data[3][2] < c.data[3][1] && c.data[3][2] < c.data[3][3] && c.data[3][2] == z && c.empty_square_count( ) == s; } );
				this->add_evaluation( [=]( const core_2048 & c )
				{
					return
							c.data[0][1] < c.data[0][0] &&
							c.data[0][1] < c.data[0][2] &&
							c.data[0][1] < c.data[1][1] &&
							c.data[0][1] == z &&
							c.empty_square_count( ) == s;
				} );
				this->add_evaluation( [=]( const core_2048 & c )
				{
					return
							c.data[0][2] < c.data[0][1] &&
							c.data[0][2] < c.data[0][3] &&
							c.data[0][2] < c.data[1][2] &&
							c.data[0][2] == z &&
							c.empty_square_count( ) == s;
				} );
				this->add_evaluation( [=]( const core_2048 & c )
				{
					return
							c.data[1][0] < c.data[0][0] &&
							c.data[1][0] < c.data[2][0] &&
							c.data[1][0] < c.data[1][1] &&
							c.data[1][0] == z &&
							c.empty_square_count( ) == s;
				} );
				this->add_evaluation( [=]( const core_2048 & c )
				{
					return
							c.data[2][0] < c.data[1][0] &&
							c.data[2][0] < c.data[3][0] &&
							c.data[2][0] < c.data[2][1] &&
							c.data[2][0] == z &&
							c.empty_square_count( ) == s;
				} );
				this->add_evaluation( [=]( const core_2048 & c )
				{
					return
							c.data[1][3] < c.data[0][3] &&
							c.data[1][3] < c.data[2][3] &&
							c.data[1][3] < c.data[1][2] &&
							c.data[1][3] == z &&
							c.empty_square_count( ) == s;
				} );
				this->add_evaluation( [=]( const core_2048 & c )
				{
					return
							c.data[2][3] < c.data[1][3] &&
							c.data[2][3] < c.data[3][3] &&
							c.data[2][3] < c.data[2][2] &&
							c.data[2][3] == z &&
							c.empty_square_count( ) == s;
				} );
				this->add_evaluation( [=]( const core_2048 & c )
				{
					return
							c.data[3][1] < c.data[3][0] &&
							c.data[3][1] < c.data[3][2] &&
							c.data[3][1] < c.data[2][1] &&
							c.data[3][1] == z &&
							c.empty_square_count( ) == s;
				} );
				this->add_evaluation( [=]( const core_2048 & c )
				{
					return
							c.data[3][2] < c.data[3][1] &&
							c.data[3][2] < c.data[3][3] &&
							c.data[3][2] < c.data[2][2] &&
							c.data[3][2] == z &&
							c.empty_square_count( ) == s;
				} );
			}
		}
	}
};
#endif // PLAYER_HPP
