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
	add_evaluation( []( const core_2048 & c ){ return c.empty_square_count( ); } );
	for ( size_t i : { 0, 1, 2, 3 } )
	{
		for ( size_t j : { 0, 1, 2, 3 } )
		{
			for ( size_t z : { 0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072 } )
			{ add_evaluation( [=]( const core_2048 & c ){ return c.data[i][j] == z; } ); }
			add_evaluation( [=]( const core_2048 & c ){ return c.largest_square( ) == c.data[i][j]; } );
			for ( size_t a : { 0, 1, 2, 3 } )
			{
				for ( size_t b : { 0, 1, 2, 3 } )
				{
					add_evaluation( [=]( const core_2048 & c ){ return c.data[i][j] > c.data[a][b]; } );
					for ( size_t z : { 0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072 } )
					{
						add_evaluation( [=]( const core_2048 & c ){ return c.data[i][j] == c.data[a][b] && c.data[a][b] == z; } );
					}
				}
			}
		}
	}
	for ( core_2048::direction dir : { core_2048::up, core_2048::down, core_2048::left, core_2048::right } )
	{
		add_evaluation( [=]( const core_2048 & c ){ return c.can_move( dir ); } );
	}
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
		update_factor_weight( ret[ std::distance( evaluate.begin( ), std::max_element( evaluate.begin( ), evaluate.end( ) ) ) ] );
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

void MainWindow::try_move( core_2048::direction dir )
{
	if ( data->can_move( dir ) ) { data->move( dir ); }
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
			std::accumulate(
				evaluation.begin( ),
				evaluation.end( ),
				c.score,
				[&]( double d, decltype( evaluation[0] ) f ){ return d + f.first( c ) * ( f.second.first + f.second.second ); } );
}

void MainWindow::update_factor_weight( const core_2048 & next_step )
{
	double difference = evaluate_positsion( * data ) - evaluate_positsion( next_step );
	if ( difference != 0 )
	{
		std::for_each(
					evaluation.begin( ),
					evaluation.end( ),
					[&]( decltype( evaluation[0] ) f )
		{
			f.second.first += learn_rate * ( f.first( * data ) - f.first( next_step ) ) * f.second.first / difference;
			f.second.second -= learn_rate * ( f.first( * data ) - f.first( next_step ) ) * f.second.second / difference;
		} );
	}
}

void MainWindow::update_factor_weight(core_2048::direction dir)
{
	auto res = data->make_move( dir );
	for ( auto & i : res ) { update_factor_weight( i ); }
}

void MainWindow::add_evaluation( const std::function< double ( const core_2048 & ) > & f )
{ evaluation.push_back( std::make_pair( f, std::make_pair( inital_weight, -1 * inital_weight ) ) ); }

void MainWindow::add_evaluation( std::function< double ( const core_2048 & ) > && f )
{ evaluation.push_back( std::make_pair( std::move( f ), std::make_pair( inital_weight, -1 * inital_weight ) ) ); }

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
#include <QMessageBox>
void MainWindow::on_actionTrain_triggered()
{
	double total_score = 0;
	size_t train_time = QInputDialog::getInt( this, "Enter Train Episod", "", 1, 0 );
	for ( size_t i = 0; i < train_time; ++i )
	{
		on_actionAuto_triggered( );
		total_score += data->score;
		on_actionRestart_triggered( );
	}
	QMessageBox::information(
				this,
				"Training Complete",
				QString( "Average score:" ) +
				QString( std::to_string( total_score / train_time ).c_str( ) ) );
}

void MainWindow::on_actionSearchDepth_triggered()
{
	search_depth = QInputDialog::getInt( this, "Enter Train Episod", "", search_depth, 0 );
}

void MainWindow::on_actionLearnRate_triggered()
{
	learn_rate = QInputDialog::getDouble( this, "Enter Learn Rate", "", learn_rate, 0, 1, 10 );
}
