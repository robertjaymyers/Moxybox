/*
This file is part of Moxybox.
	Moxybox is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	Moxybox is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with Moxybox.  If not, see <https://www.gnu.org/licenses/>.
*/

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
