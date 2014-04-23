#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "boost/range/as_literal.hpp"
#include "core_2048.hpp"
#include <QKeyEvent>
#include <Dialog.hpp>
void MainWindow::keyPressEvent( QKeyEvent * event )
{
	if( event->key( ) == Qt::Key_Up )
	{ if ( data->can_move( core_2048::up ) ) { data->move( core_2048::up ); } }
	else if( event->key( ) == Qt::Key_Down )
	{ if ( data->can_move( core_2048::down ) ) { data->move( core_2048::down ); } }
	else if( event->key( ) == Qt::Key_Left )
	{ if ( data->can_move( core_2048::left ) ) { data->move( core_2048::left ); } }
	else if( event->key( ) == Qt::Key_Right )
	{ if ( data->can_move( core_2048::right ) ) { data->move( core_2048::right ); } }
	update_value( );
	ui->statusbar->showMessage(
				QString( "Score:" ) +
				QString( std::to_string( data->score ).c_str( ) ) );
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui( new Ui::MainWindow ),
	data( new core_2048 ),
	dlg( new Dialog )
{
	ui->setupUi(this);
	data->random_add( );
	update_value( );
}

MainWindow::~MainWindow()
{
	delete ui;
	delete data;
	delete dlg;
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
	static std::map< size_t, QPixmap > map;
	auto it = map.find( s );
	if ( it == map.end( ) )
	{
		auto ret = map.insert(
								 std::make_pair(
									 s,
									 QPixmap(
										 QString( "../Q2048/" ) +
										 QString( std::to_string( s ).c_str( ) ) +
										 QString( ".png" ) ).scaled( 128, 128 ) ) );
		return ret.first->second;
	}
	else { return it->second; }
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
