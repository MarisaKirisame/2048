#include "player.hpp"
void player::update( const core_2048 & data, const core_2048::direction dir )
{
	auto res = data.make_move( dir );
	for ( auto & i : res ) { update( data, dir, i ); }
}

player::~player( ) { }

core_2048::direction player::find_move( const core_2048 & data )
{
	auto ret = data.all_next_move( );
	assert( ! ret.empty( ) );
	std::vector< std::vector< core_2048 > > next_board;
	std::transform(
				ret.begin( ),
				ret.end( ),
				std::back_inserter( next_board ),
				[&](core_2048::direction dir) { return data.make_move( dir ); } );
	std::vector< double > evaluate;
	std::transform(
				next_board.begin( ),
				next_board.end( ),
				std::back_inserter( evaluate ),
				[&]( const std::vector< core_2048 > & vc )
	{
		double result = 0;
		std::for_each( vc.begin( ), vc.end( ), [&](const core_2048 & c)
		{
			result += evaluate_positsion_minmax( c, search_depth );
		} );
		return result;
	} );
	auto res = ret[ std::distance( evaluate.begin( ), std::max_element( evaluate.begin( ), evaluate.end( ) ) ) ];
	update( data, res );
	return res;
}

double player::evaluate_positsion_minmax(const core_2048 & c, size_t depth)
{
	if ( depth == 0 ) { return evaluate( c ); }
	else
	{
		auto ret = c.all_next_move( );
		if ( ret.empty( ) ) { return c.score; }
		std::vector< std::vector< core_2048 > > next_board;
		std::transform(
					ret.begin( ),
					ret.end( ),
					std::back_inserter( next_board ),
					[&](core_2048::direction dir) { return c.make_move( dir ); } );
		std::vector< double > evaluate;
		std::transform(
					next_board.begin( ),
					next_board.end( ),
					std::back_inserter( evaluate ),
					[&]( const std::vector< core_2048 > & vc )
		{
			double result = 0;
			std::for_each( vc.begin( ), vc.end( ), [&](const core_2048 & c)
			{
				result += evaluate_positsion_minmax( c, depth - 1 );
			} );
			return result / vc.size( );
		} );
		return * std::max_element( evaluate.begin( ), evaluate.end( ) );
	}
}

void SRS_player::add_evaluation( std::function< double ( const core_2048 & ) > && f )
{ evaluation.push_back( std::make_pair( std::move( f ), std::make_pair( inital_weight, -1 * inital_weight ) ) ); }

void SRS_player::add_evaluation( const std::function< double ( const core_2048 & ) > & f )
{ evaluation.push_back( std::make_pair( f, std::make_pair( inital_weight, -1 * inital_weight ) ) ); }

void SRS_player::update( const core_2048 & data, const core_2048 & next_step )
{
	double difference = evaluate( data ) - evaluate( next_step ) ;
	if ( difference != 0 )
	{
		std::for_each(
					evaluation.begin( ),
					evaluation.end( ),
					[&]( decltype( evaluation[0] ) f )
		{
			f.second.first += learn_rate * ( f.first( data ) - f.first( next_step ) ) * f.second.first / difference;
			f.second.second -= learn_rate * ( f.first( data ) - f.first( next_step ) ) * f.second.second / difference;
		} );
		if( abs( difference ) < abs( evaluate( data ) - evaluate( next_step ) ) )
		{ learn_rate /= 2; }
	}
}

SRS_player::SRS_player( ) { }

double SRS_player::evaluate( const core_2048 & c )
{
	return
			c.can_move( ) ?
			std::accumulate(
				evaluation.begin( ),
				evaluation.end( ),
				c.score,
				[&]( double d, decltype( evaluation[0] ) f ){ return d + f.first( c ) * ( f.second.first + f.second.second ); } ) : c.score;
}

void SRS_player::update( const core_2048 & data, core_2048::direction, const core_2048 & next_step ) { update( data, next_step ); }

double SARSA_player::evaluate(const core_2048 & c, const core_2048::direction dir)
{
	if ( ! c.can_move( dir ) ) { return c.score; }
	switch ( dir )
	{
		case core_2048::up:
			return
					std::accumulate(
						evaluation.begin( ),
						evaluation.end( ),
						c.score,
						[&]( double d, decltype( evaluation[0] ) f ){ return d + f.first( c ) * ( f.second[0] + f.second[1] ); } );
		case core_2048::down:
			return
					std::accumulate(
						evaluation.begin( ),
						evaluation.end( ),
						c.score,
						[&]( double d, decltype( evaluation[0] ) f ){ return d + f.first( c ) * ( f.second[2] + f.second[3] ); } );
		case core_2048::left:
			return
					std::accumulate(
						evaluation.begin( ),
						evaluation.end( ),
						c.score,
						[&]( double d, decltype( evaluation[0] ) f ){ return d + f.first( c ) * ( f.second[4] + f.second[5] ); } );
		default:
			return
					std::accumulate(
						evaluation.begin( ),
						evaluation.end( ),
						c.score,
						[&]( double d, decltype( evaluation[0] ) f ){ return d + f.first( c ) * ( f.second[6] + f.second[7] ); } );
	}
}

double SARSA_player::evaluate( const core_2048 & c )
{
	return
			std::max(
				std::max( evaluate( c, core_2048::up ), evaluate( c, core_2048::down ) ),
				std::max( evaluate( c, core_2048::left ), evaluate( c, core_2048::right ) ) );
}

core_2048::direction SARSA_player::instant_find_move( const core_2048 & c )
{
	double up = evaluate( c, core_2048::up );
	double down = evaluate( c, core_2048::down );
	double left = evaluate( c, core_2048::left );
	double right = evaluate( c, core_2048::right );
	double best = std::max( std::max( up, down ), std::max( left, right ) );
	return best == left ? core_2048::left : ( best == right ? core_2048::right : ( best == up ? core_2048::up : core_2048 ::down ) );
}

void SARSA_player::add_evaluation( std::function< double ( const core_2048 & ) > && f )
{
	evaluation.push_back(
				std::make_pair(
					std::move( f ),
					std::array< double, 8 >
					{
						inital_weight, -1 * inital_weight,
						inital_weight, -1 * inital_weight,
						inital_weight, -1 * inital_weight,
						inital_weight, -1 * inital_weight
					} ) );
}

void SARSA_player::add_evaluation( const std::function< double ( const core_2048 & ) > & f )
{
	evaluation.push_back(
				std::make_pair(
					std::move( f ),
					std::array< double, 8 >
					{
						inital_weight, -1 * inital_weight,
						inital_weight, -1 * inital_weight,
						inital_weight, -1 * inital_weight,
						inital_weight, -1 * inital_weight
					} ) );
}

void SARSA_player::update( const core_2048 & data, const core_2048 & next_step )
{ update( data, instant_find_move( data ), next_step ); }

void SARSA_player::update(const core_2048 & data, core_2048::direction data_dir, const core_2048 & next_step)
{
	double difference = evaluate( next_step ) - evaluate( data, data_dir );
	auto next_step_dir = instant_find_move( data );
	if ( difference != 0 )
	{
		std::for_each(
					evaluation.begin( ),
					evaluation.end( ),
					[&]( decltype( evaluation[0] ) f )
		{
			f.second[0] +=
					learn_rate *
					( ( data_dir == core_2048::up ? f.first( data ) : 0 ) - ( next_step_dir == core_2048::up ? f.first( next_step ) : 0 ) ) *
					f.second[0] /
					difference;
			f.second[1] -=
					learn_rate *
					( ( data_dir == core_2048::up ? f.first( data ) : 0 ) - ( next_step_dir == core_2048::up ? f.first( next_step ) : 0 ) ) *
					f.second[1] /
					difference;
			f.second[2] +=
					learn_rate *
					( ( data_dir == core_2048::down ? f.first( data ) : 0 ) - ( next_step_dir == core_2048::down ? f.first( next_step ) : 0 ) ) *
					f.second[2] /
					difference;
			f.second[3] -=
					learn_rate *
					( ( data_dir == core_2048::down ? f.first( data ) : 0 ) - ( next_step_dir == core_2048::down ? f.first( next_step ) : 0 ) ) *
					f.second[3] /
					difference;
			f.second[4] +=
					learn_rate *
					( ( data_dir == core_2048::left ? f.first( data ) : 0 ) - ( next_step_dir == core_2048::left ? f.first( next_step ) : 0 ) ) *
					f.second[4] /
					difference;
			f.second[5] -=
					learn_rate *
					( ( data_dir == core_2048::left ? f.first( data ) : 0 ) - ( next_step_dir == core_2048::left ? f.first( next_step ) : 0 ) ) *
					f.second[5] /
					difference;
			f.second[6] +=
					learn_rate *
					( ( data_dir == core_2048::right ? f.first( data ) : 0 ) - ( next_step_dir == core_2048::right ? f.first( next_step ) : 0 ) ) *
					f.second[6] /
					difference;
			f.second[7] -=
					learn_rate *
					( ( data_dir == core_2048::right ? f.first( data ) : 0 ) - ( next_step_dir == core_2048::right ? f.first( next_step ) : 0 ) ) *
					f.second[7] /
					difference;
		} );
		if( abs( difference ) < abs( evaluate( next_step ) - evaluate( data ) ) ) { learn_rate /= 2; }
	}
}
