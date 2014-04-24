#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "boost/range/as_literal.hpp"
#include "core_2048.hpp"
#include <QKeyEvent>
#include <Dialog.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
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
	if ( data->can_move( dir ) ) { data->move( dir ); }
	update_value( );
	ui->statusbar->showMessage(
				QString( "Score:" ) +
				QString( std::to_string( data->score ).c_str( ) ) );
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
#include <thread>
#include <QInputDialog>
void MainWindow::on_actionAuto_triggered()
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
					[]( const std::vector< core_2048 > & vc )
		{
			double result = 0;
			std::for_each( vc.begin( ), vc.end( ), [&](const core_2048 & c)
			{
				result += std::distance( c.empty_begin( ), c.empty_end( ) );
			} );
			return result / vc.size( );
		} );
		try_move( ret[ std::distance( evaluate.begin( ), std::max_element( evaluate.begin( ), evaluate.end( ) ) ) ] );
	}
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
