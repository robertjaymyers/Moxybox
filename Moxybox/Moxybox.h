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

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Moxybox.h"
#include "GameplayScreen.h"
#include <QGridLayout>
#include <QFontDatabase>

class Moxybox : public QMainWindow
{
	Q_OBJECT

public:
	Moxybox(QWidget *parent = Q_NULLPTR);
	void closeEvent(QCloseEvent *event);

private:
	Ui::MoxyboxClass ui;

	QFontDatabase fontDatabase;

	std::unique_ptr<GameplayScreen> gameplayScreen = std::make_unique<GameplayScreen>(this);
	std::unique_ptr<QGridLayout> layout = std::make_unique<QGridLayout>();
};
