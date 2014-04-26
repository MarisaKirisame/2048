#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "boost/range/as_literal.hpp"
#include "core_2048.hpp"
#include <QKeyEvent>
#include <Dialog.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <thread>
void MainWindow::keyPressEvent( QKeyEvent * event )
{
	if( event->key( ) == Qt::Key_Up ) { try_move( core_2048::up ); }
	else if( event->key( ) == Qt::Key_Down ) { try_move( core_2048::down ); }
	else if( event->key( ) == Qt::Key_Left ) { try_move( core_2048::left ); }
	else if( event->key( ) == Qt::Key_Right ) { try_move( core_2048::right ); }
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui( new Ui::MainWindow ),
	data( new core_2048 ),
	dlg( new Dialog )
{
	ui->setupUi(this);
	data->random_add( );
	boost::property_tree::ptree pt;
	boost::property_tree::read_xml( "config.xml", pt );
	picture_path = pt.get< std::string >( "pictures.path", "/home/jerry/Work/Q2048/TouHou" );
	update_value( );
}

MainWindow::~MainWindow()
{
	delete ui;
	delete data;
	delete dlg;
	boost::property_tree::ptree pt;
	pt.put< std::string >( "pictures.path", picture_path );
	boost::property_tree::write_xml( "config.xml", pt );
}

void MainWindow::auto_minmax_square( int depth )
{
	while( data->can_move( ) )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( sleep_for_miliseconds ) );
		auto ret = data->all_next_move( );
		assert( ! ret.empty( ) );
		std::vector< std::vector< core_2048 > > next_board;
		std::transform(
					ret.begin( ),
					ret.end( ),
					std::back_inserter( next_board ),
					[this](core_2048::direction dir) { return data->make_move( dir ); } );
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
				result += this->evaluate_positsion_minmax( c, depth );
			} );
			return result;
		} );
		try_move( ret[ std::distance( evaluate.begin( ), std::max_element( evaluate.begin( ), evaluate.end( ) ) ) ] );
	}
}
#include <QLabel>
std::vector< QLabel *> MainWindow::display( )
{
	return std::vector< QLabel * >(
	{
		ui->label,
		ui->label_2,
		ui->label_3,
		ui->label_4,
		ui->label_5,
		ui->label_6,
		ui->label_7,
		ui->label_8,
		ui->label_9,
		ui->label_10,
		ui->label_11,
		ui->label_12,
		ui->label_13,
		ui->label_14,
		ui->label_15,
		ui->label_16
	} );
}
#include <QPicture>
void MainWindow::update_value()
{
	auto r = display( );
	auto ri = r.begin( );
	auto i = data->begin( );
	auto end = data->end( );
	std::for_each(
				i,
				end,
				[&](square & s)
	{
		(*ri)->setPixmap( getPicture( s ) );
		++ri;
	} );
}
#include <QPixmap>
QPixmap & MainWindow::getPicture( const square & s )
{
	auto it = map.find( s );
	if ( it == map.end( ) )
	{
		auto ret = map.insert(
								 std::make_pair(
									 s,
									 QPixmap( QString( picture_path.c_str( ) ) +
														QString( "/" ) +
														QString( std::to_string( s ).c_str( ) ) +
														QString( ".png" ) ).scaled( 128, 128 ) ) );
		return ret.first->second;
	}
	else { return it->second; }
}

void MainWindow::try_move(core_2048::direction dir)
{
	if ( data->can_move( dir ) )
	{
		auto res = data->make_move( dir );
		for ( auto & i : res ) { update_factor_weight( 0.0001, i ); }
		data->move( dir );
	}
	update_value( );
	ui->statusbar->showMessage(
				QString( "Score:" ) +
				QString( std::to_string( data->score ).c_str( ) ) );
}

double MainWindow::evaluate_positsion_minmax(const core_2048 & c, size_t depth) const
{
	if ( depth == 0 ) { return evaluate_positsion( c ); }
	else
	{
		auto ret = data->all_next_move( );
		assert( ! ret.empty( ) );
		std::vector< std::vector< core_2048 > > next_board;
		std::transform(
					ret.begin( ),
					ret.end( ),
					std::back_inserter( next_board ),
					[this](core_2048::direction dir) { return data->make_move( dir ); } );
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

double MainWindow::evaluate_positsion(const core_2048 & c) const
{
	return
			c.score +
			c.empty_square_count( ) * empty_square_value +
			c.can_move( core_2048::up ) * can_move_up_value +
			c.can_move( core_2048::down ) * can_move_down_value +
			c.can_move( core_2048::left ) * can_move_left_value +
			c.can_move( core_2048::right ) * can_move_right_value +
			c.data[0][0].empty( ) * upleft_corner_empty_value +
			c.data[0][3].empty( ) * upright_corner_empty_value +
			c.data[3][0].empty( ) * downleft_corner_empty_value +
			c.data[3][3].empty( ) * downright_corner_empty_value;
}

void MainWindow::update_factor_weight( double learn_rate, const core_2048 & next_step )
{
	double empty_square_difference =
			static_cast< int >( data->empty_square_count( ) ) -
			static_cast< int >( next_step.empty_square_count( ) );
	double can_move_down_difference =
			static_cast< int >( data->can_move( core_2048::down ) ) -
			static_cast< int >( next_step.can_move( core_2048::down ) );
	double can_move_left_difference =
			static_cast< int >( data->can_move( core_2048::left ) ) -
			static_cast< int >( next_step.can_move( core_2048::left ) );
	double can_move_right_difference =
			static_cast< int >( data->can_move( core_2048::right ) ) -
			static_cast< int >( next_step.can_move( core_2048::right ) );
	double can_move_up_difference =
			static_cast< int >( data->can_move( core_2048::up ) ) -
			static_cast< int >( next_step.can_move( core_2048::up ) );
	double upleft_corner_empty_difference =
			static_cast< int >( data->data[0][0].empty( ) ) -
			static_cast< int >( next_step.data[0][0].empty( ) );
	double upright_corner_empty_difference =
			static_cast< int >( data->data[0][3].empty( ) ) -
			static_cast< int >( next_step.data[0][3].empty( ) );
	double downleft_corner_empty_difference =
			static_cast< int >( data->data[3][0].empty( ) ) -
			static_cast< int >( next_step.data[3][0].empty( ) );
	double downright_corner_empty_difference =
			static_cast< int >( data->data[3][3].empty( ) ) -
			static_cast< int >( next_step.data[3][3].empty( ) );
	double difference = evaluate_positsion( * data ) - evaluate_positsion( next_step );
	if ( difference == 0 ) { return; }
	empty_square_value += learn_rate * empty_square_difference * empty_square_value / difference;
	can_move_down_value += learn_rate * can_move_down_difference * can_move_down_value / difference;
	can_move_up_value += learn_rate * can_move_up_difference * can_move_up_value / difference;
	can_move_left_value += learn_rate * can_move_left_difference * can_move_left_value / difference;
	can_move_right_value += learn_rate * can_move_right_difference * can_move_right_value / difference;
	upleft_corner_empty_value += learn_rate * upleft_corner_empty_difference * upleft_corner_empty_value / difference;
	upright_corner_empty_value += learn_rate * upright_corner_empty_difference * upright_corner_empty_value / difference;
	downleft_corner_empty_value += learn_rate * downleft_corner_empty_difference * downleft_corner_empty_value / difference;
	downright_corner_empty_value += learn_rate * downright_corner_empty_difference * downright_corner_empty_value / difference;
}

void MainWindow::on_actionRestart_triggered( )
{
	delete data;
	data = new core_2048;
	data->random_add( );
	update_value( );
}

void MainWindow::on_actionHowTo_triggered()
{
	dlg->show( );
}
#include <chrono>
#include <QInputDialog>

void MainWindow::on_actionAuto_triggered()
{
	auto_minmax_square( 0 );
}

void MainWindow::on_actionDelayBetweenMove_triggered()
{
	sleep_for_miliseconds =
			QInputDialog::getDouble(
				this,
				"Enter Delay",
				"Delay(s):",
				static_cast< double >( sleep_for_miliseconds ) / 1000 ) *
			1000;
}
