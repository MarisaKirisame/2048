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
	update( );
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
void MainWindow::update()
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
		for ( auto & i : res ) { update_factor_weight( i ); }
		data->move( dir );
	}
	update( );
	ui->statusbar->showMessage(
				QString( "Score:" ) +
				QString( std::to_string( data->score ).c_str( ) ) );
	repaint();
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
			c.data[3][3].empty( ) * downright_corner_empty_value +
			( c.largest_square( ) == c.data[0][0] ) * largest_on_00_value +
			( c.largest_square( ) == c.data[0][1] ) * largest_on_01_value +
			( c.largest_square( ) == c.data[0][2] ) * largest_on_02_value +
			( c.largest_square( ) == c.data[0][3] ) * largest_on_03_value +
			( c.largest_square( ) == c.data[1][0] ) * largest_on_10_value +
			( c.largest_square( ) == c.data[1][1] ) * largest_on_11_value +
			( c.largest_square( ) == c.data[1][2] ) * largest_on_12_value +
			( c.largest_square( ) == c.data[1][3] ) * largest_on_13_value +
			( c.largest_square( ) == c.data[2][0] ) * largest_on_20_value +
			( c.largest_square( ) == c.data[2][1] ) * largest_on_21_value +
			( c.largest_square( ) == c.data[2][2] ) * largest_on_22_value +
			( c.largest_square( ) == c.data[2][3] ) * largest_on_23_value +
			( c.largest_square( ) == c.data[3][0] ) * largest_on_30_value +
			( c.largest_square( ) == c.data[3][1] ) * largest_on_31_value +
			( c.largest_square( ) == c.data[3][2] ) * largest_on_32_value +
			( c.largest_square( ) == c.data[3][3] ) * largest_on_33_value;
}

void MainWindow::update_factor_weight( const core_2048 & next_step )
{
	double empty_square_difference =
			static_cast< double >( data->empty_square_count( ) ) -
			static_cast< double >( next_step.empty_square_count( ) );
	double can_move_down_difference =
			static_cast< double >( data->can_move( core_2048::down ) ) -
			static_cast< double >( next_step.can_move( core_2048::down ) );
	double can_move_left_difference =
			static_cast< double >( data->can_move( core_2048::left ) ) -
			static_cast< double >( next_step.can_move( core_2048::left ) );
	double can_move_right_difference =
			static_cast< double >( data->can_move( core_2048::right ) ) -
			static_cast< double >( next_step.can_move( core_2048::right ) );
	double can_move_up_difference =
			static_cast< double >( data->can_move( core_2048::up ) ) -
			static_cast< double >( next_step.can_move( core_2048::up ) );
	double upleft_corner_empty_difference =
			static_cast< double >( data->data[0][0].empty( ) ) -
			static_cast< double >( next_step.data[0][0].empty( ) );
	double upright_corner_empty_difference =
			static_cast< double >( data->data[0][3].empty( ) ) -
			static_cast< double >( next_step.data[0][3].empty( ) );
	double downleft_corner_empty_difference =
			static_cast< double >( data->data[3][0].empty( ) ) -
			static_cast< double >( next_step.data[3][0].empty( ) );
	double downright_corner_empty_difference =
			static_cast< double >( data->data[3][3].empty( ) ) -
			static_cast< double >( next_step.data[3][3].empty( ) );
	double largest_on_00_difference =
			static_cast< double >( data->largest_square( ) == data->data[0][0] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[0][0] );
	double largest_on_01_difference =
			static_cast< double >( data->largest_square( ) == data->data[0][1] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[0][1] );
	double largest_on_02_difference =
			static_cast< double >( data->largest_square( ) == data->data[0][2] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[0][2] );
	double largest_on_03_difference =
			static_cast< double >( data->largest_square( ) == data->data[0][3] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[0][3] );
	double largest_on_10_difference =
			static_cast< double >( data->largest_square( ) == data->data[1][0] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[1][0] );
	double largest_on_11_difference =
			static_cast< double >( data->largest_square( ) == data->data[1][1] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[1][1] );
	double largest_on_12_difference =
			static_cast< double >( data->largest_square( ) == data->data[1][2] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[1][2] );
	double largest_on_13_difference =
			static_cast< double >( data->largest_square( ) == data->data[1][3] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[1][3] );
	double largest_on_20_difference =
			static_cast< double >( data->largest_square( ) == data->data[2][0] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[2][0] );
	double largest_on_21_difference =
			static_cast< double >( data->largest_square( ) == data->data[2][1] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[2][1] );
	double largest_on_22_difference =
			static_cast< double >( data->largest_square( ) == data->data[2][2] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[2][2] );
	double largest_on_23_difference =
			static_cast< double >( data->largest_square( ) == data->data[2][3] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[2][3] );
	double largest_on_30_difference =
			static_cast< double >( data->largest_square( ) == data->data[3][0] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[3][0] );
	double largest_on_31_difference =
			static_cast< double >( data->largest_square( ) == data->data[3][1] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[3][1] );
	double largest_on_32_difference =
			static_cast< double >( data->largest_square( ) == data->data[3][2] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[3][2] );
	double largest_on_33_difference =
			static_cast< double >( data->largest_square( ) == data->data[3][3] ) -
			static_cast< double >( next_step.largest_square( ) == next_step.data[3][3] );
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
	largest_on_00_value += learn_rate * largest_on_00_difference * largest_on_00_value / difference;
	largest_on_01_value += learn_rate * largest_on_01_difference * largest_on_01_value / difference;
	largest_on_02_value += learn_rate * largest_on_02_difference * largest_on_02_value / difference;
	largest_on_03_value += learn_rate * largest_on_03_difference * largest_on_03_value / difference;
	largest_on_10_value += learn_rate * largest_on_10_difference * largest_on_10_value / difference;
	largest_on_11_value += learn_rate * largest_on_11_difference * largest_on_11_value / difference;
	largest_on_12_value += learn_rate * largest_on_12_difference * largest_on_12_value / difference;
	largest_on_13_value += learn_rate * largest_on_13_difference * largest_on_13_value / difference;
	largest_on_20_value += learn_rate * largest_on_20_difference * largest_on_20_value / difference;
	largest_on_21_value += learn_rate * largest_on_21_difference * largest_on_21_value / difference;
	largest_on_22_value += learn_rate * largest_on_22_difference * largest_on_22_value / difference;
	largest_on_23_value += learn_rate * largest_on_23_difference * largest_on_23_value / difference;
	largest_on_30_value += learn_rate * largest_on_30_difference * largest_on_30_value / difference;
	largest_on_31_value += learn_rate * largest_on_31_difference * largest_on_31_value / difference;
	largest_on_32_value += learn_rate * largest_on_32_difference * largest_on_32_value / difference;
	largest_on_33_value += learn_rate * largest_on_33_difference * largest_on_33_value / difference;
}

void MainWindow::on_actionRestart_triggered( )
{
	delete data;
	data = new core_2048;
	data->random_add( );
	update( );
}

void MainWindow::on_actionHowTo_triggered()
{
	dlg->show( );
}
#include <chrono>
#include <QInputDialog>

void MainWindow::on_actionAuto_triggered()
{
	auto_minmax_square( search_depth );
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

void MainWindow::on_actionTrain_triggered()
{
	size_t train_time = QInputDialog::getInt( this, "Enter Train Episod", "", 1, 0 );
	while ( train_time-->0 )
	{
		on_actionAuto_triggered( );
		on_actionRestart_triggered( );
	}
}

void MainWindow::on_actionSearchDepth_triggered()
{
	search_depth = QInputDialog::getInt( this, "Enter Train Episod", "", search_depth, 0 );
}

void MainWindow::on_actionLearnRate_triggered()
{
	learn_rate = QInputDialog::getDouble( this, "Enter Learn Rate", "", learn_rate, 0, 1, 10 );
}
