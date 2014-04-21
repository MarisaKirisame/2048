#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
#include "q2048_global.hpp"
#include <QMainWindow>
#include <cassert>
#include <iterator>
#include <boost/range/join.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <core_2048.hpp>
#include <array>
class QLCDNumber;
namespace Ui { class MainWindow; }
struct Q2048SHARED_EXPORT MainWindow : QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow( QWidget * parent = 0 );
	~MainWindow( );
	Ui::MainWindow *ui;
	std::vector< QLCDNumber * > display( );
	void update_value( );
};

#endif // MAINWINDOW_HPP
