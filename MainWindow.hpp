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
	void auto_minmax_square( int depth );
	Ui::MainWindow *ui;
	std::vector< QLabel * > display( );
	void update_value( );
	core_2048 * data;
	QPixmap & getPicture( const square & );
	Dialog * dlg;
	void try_move( core_2048::direction dir );
	std::string picture_path;
	int sleep_for_miliseconds = 0;
	double evaluate_positsion_minmax( const core_2048 & c, size_t depth ) const;
	double evaluate_positsion( const core_2048 & c ) const;
	double empty_square_value = 10;
	double can_move_up_value = 10;
	double can_move_down_value = 10;
	double can_move_right_value = 10;
	double can_move_left_value = 10;
	double upleft_corner_empty_value = 10;
	double upright_corner_empty_value = 10;
	double downleft_corner_empty_value = 10;
	double downright_corner_empty_value = 10;
	void update_factor_weight( double learn_rate, const core_2048 & next_step );
private slots:
	void on_actionRestart_triggered();
	void on_actionHowTo_triggered();
	void on_actionAuto_triggered();
	void on_actionDelayBetweenMove_triggered();
};

#endif // MAINWINDOW_HPP
