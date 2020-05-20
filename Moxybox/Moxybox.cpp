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

Moxybox::Moxybox(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	fontDatabase.addApplicationFont(":/Moxybox/Fonts/Pixellari.ttf");

	ui.centralWidget->setLayout(layout.get());

	layout.get()->setMargin(0);
	layout.get()->addWidget(gameplayScreen.get(), 0, 0);

	gameplayScreen.get()->show();
}

void Moxybox::closeEvent(QCloseEvent *event)
{
	// The main window needs to access the pref save function from our QGraphicsView,
	// when the main window triggers a close event, so we make that function public.
	// Otherwise, we'd need the pref save function to be able to operate outside of QGraphicsView, which is...
	// well, messy.
	// So we do it this hacky way.
	// Classes are in a special class of their own.
	gameplayScreen.get()->prefSave();
	event->accept();
}