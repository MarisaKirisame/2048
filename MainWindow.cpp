#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "boost/range/as_literal.hpp"
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	
}

MainWindow::~MainWindow()
{
	delete ui;
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
