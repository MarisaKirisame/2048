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
	picture_path = pt.get< std::string >( "pictures.path" );
	update( );
}

MainWindow::~MainWindow()
{
	delete ui;
	delete data;
	delete dlg;
	delete p;
	boost::property_tree::ptree pt;
	pt.put< std::string >( "pictures.path", picture_path );
	boost::property_tree::write_xml( "config.xml", pt );
}

void MainWindow::auto_minmax_square( )
{
	while( data->can_move( ) )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( sleep_for_miliseconds ) );
		auto res = p->find_move( * data );
		try_move( res );
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

void MainWindow::on_actionAuto_triggered( ) { auto_minmax_square( ); }

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
	p->search_depth = QInputDialog::getInt( this, "Enter Train Episod", "", p->search_depth, 0 );
}

void MainWindow::on_actionLearnRate_triggered()
{
	p->learn_rate = QInputDialog::getDouble( this, "Enter Learn Rate", "", p->learn_rate, 0, 1, 10 );
}
