#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <QDialog>

namespace Ui {
	class Dialog;
}

class Dialog : public QDialog
{
	Q_OBJECT

public:
	explicit Dialog(QWidget *parent = 0);
	~Dialog();

private:
	Ui::Dialog *ui;
};

#endif // DIALOG_HPP
