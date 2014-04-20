#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
#include "q2048_global.hpp"
#include <QMainWindow>
namespace Ui { class MainWindow; }
struct Q2048SHARED_EXPORT MainWindow : QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow( QWidget * parent = 0 );
	~MainWindow( );
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_HPP
