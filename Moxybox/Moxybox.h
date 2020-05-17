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
