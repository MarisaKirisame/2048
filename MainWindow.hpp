#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
#include "q2048_global.hpp"
#include <QMainWindow>
#include <cassert>
#include <iterator>
#include <boost/range/join.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <array>
#include "core_2048.hpp"
class Dialog;
class QLabel;
namespace Ui { class MainWindow; }
class core_2048;
class QKeyEvent;
class square;
class QPixmap;
struct Q2048SHARED_EXPORT MainWindow : QMainWindow
{
	Q_OBJECT
public:
	std::map< size_t, QPixmap > map;
	void keyPressEvent( QKeyEvent * event );
	explicit MainWindow( QWidget * parent = 0 );
	~MainWindow( );
	Ui::MainWindow *ui;
	std::vector< QLabel * > display( );
	void update_value( );
	core_2048 * data;
	QPixmap & getPicture( const square & );
	Dialog * dlg;
	void try_move( core_2048::direction dir );
	std::string picture_path;
	int sleep_for_miliseconds = 0;
private slots:
	void on_actionRestart_triggered();
	void on_actionHowTo_triggered();
	void on_actionAuto_triggered();
	void on_actionDelayBetweenMove_triggered();
};

#endif // MAINWINDOW_HPP
