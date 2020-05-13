#include "Moxybox.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Moxybox w;
	a.setWindowIcon(QIcon(":/Moxybox/Icon/moxybox_program_icon.ico"));
	w.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	w.show();
	return a.exec();
}
