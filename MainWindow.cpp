#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "boost/range/as_literal.hpp"
#include "core_2048.hpp"
#include <QKeyEvent>
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
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui( new Ui::MainWindow ),
	data( new core_2048 )
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
	delete data;
}
#include <QLCDNumber>
std::vector< QLCDNumber *> MainWindow::display( )
{
	return std::vector< QLCDNumber * >(
	{
		ui->lcdNumber,
		ui->lcdNumber_2,
		ui->lcdNumber_3,
		ui->lcdNumber_4,
		ui->lcdNumber_5,
		ui->lcdNumber_6,
		ui->lcdNumber_7,
		ui->lcdNumber_8,
		ui->lcdNumber_9,
		ui->lcdNumber_10,
		ui->lcdNumber_11,
		ui->lcdNumber_12,
		ui->lcdNumber_13,
		ui->lcdNumber_14,
		ui->lcdNumber_15,
		ui->lcdNumber_16
	} );
}

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
		(*ri)->display( static_cast< int >( s ) );
		++ri;
	} );
}
