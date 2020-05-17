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