#include "GameplayScreen.h"

GameplayScreen::GameplayScreen(QWidget *parent)
	: QGraphicsView(parent)
{
	modLoadThemeIfExists();

	// Modifiable keybinds are initialized before preference load to ensure preference load knows what to look for and edit.
	// (Preference includes storing/loading of keybinds).

	// Grid Visual Aid
	//  _______________________________
	// |Button: (0, 0) | Label: (0, 1) |
	// |_______________________________|
	// |Button: (1, 0) | Label: (1, 1) |
	// |_______________________________|
	// |Button: (2, 0) | Label: (2, 1) |
	// |_______________________________|
	// |Button: (3, 0) | Label: (3, 1) |
	// |_______________________________|

	// Note: First two numbers are row/col in grid of button, second two are row/col in grid of label.
	// (Row is vertical placement, Col is horizontal placement)
	keybindMap.insert(std::pair<KeybindModifiable, keybindComponent>(
		KeybindModifiable::MOVE_LEFT,
		keybindComponent{ "Move Left", Qt::Key::Key_A, 0, 0, 0, 1, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	keybindMap.insert(std::pair<KeybindModifiable, keybindComponent>(
		KeybindModifiable::MOVE_RIGHT,
		keybindComponent{ "Move Right", Qt::Key::Key_D, 1, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	keybindMap.insert(std::pair<KeybindModifiable, keybindComponent>(
		KeybindModifiable::MOVE_UP,
		keybindComponent{ "Move Up", Qt::Key::Key_W, 2, 0, 2, 1, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	keybindMap.insert(std::pair<KeybindModifiable, keybindComponent>(
		KeybindModifiable::MOVE_DOWN,
		keybindComponent{ "Move Down", Qt::Key::Key_S, 3, 0, 3, 1, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	keybindMap.insert(std::pair<KeybindModifiable, keybindComponent>(
		KeybindModifiable::PLACE_PUSHER_UTIL,
		keybindComponent{ "Place Spring", Qt::Key::Key_Q, 4, 0, 4, 1, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	keybindMap.insert(std::pair<KeybindModifiable, keybindComponent>(
		KeybindModifiable::PLACE_SUCKER_UTIL,
		keybindComponent{ "Place Magnet", Qt::Key::Key_E, 5, 0, 5, 1, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	keybindMap.insert(std::pair<KeybindModifiable, keybindComponent>(
		KeybindModifiable::OPEN_MENU,
		keybindComponent{ "Open Menu", Qt::Key::Key_Escape, 6, 0, 6, 1, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	prefLoad();

	if (firstTimeSetup)
	{
		QDir dirSaves(windowsHomePath + "/" + savesFolderName);
		QDir dirLevelDataMods(levelDataPathMods);
		QDir dirThemeMods(themePathMods + "/Images");

		if (!dirSaves.exists())
			dirSaves.mkpath(".");

		if (!dirLevelDataMods.exists())
			dirLevelDataMods.mkpath(".");

		if (!dirThemeMods.exists())
			dirThemeMods.mkpath(".");
	}

	setStyleSheet(styleMap.at("baseStyle"));
	setAlignment(Qt::AlignTop | Qt::AlignLeft);

	scene.get()->setParent(this->parent());
	setScene(scene.get());

	splashItem.get()->setPixmap(imgMap.at("imgSplashTitle"));
	splashItem.get()->setZValue(splashZ);
	scene.get()->addItem(splashItem.get());

	uiGameplayGroup->setVisible(false);

	uiGameplayFontGroupTitle.setFamily(standardFontFamily);
	uiGameplayFontGroupTitle.setPointSize(uiGameplayFontGroupTitle_fontPointSize);
	uiGameplayFontGroupTitle.setStyleStrategy(standardFontStyleStrategy);
	uiGameplayFontGroupTitle.setWeight(standardFontWeight);

	uiGameplayFontTextBox.setFamily(standardFontFamily);
	uiGameplayFontTextBox.setPointSize(uiGameplayFontTextBox_fontPointSize);
	uiGameplayFontTextBox.setStyleStrategy(standardFontStyleStrategy);
	uiGameplayFontTextBox.setWeight(standardFontWeight);

	uiGameplayGroup.get()->setGeometry(QRect(0, gridHeight + 1, screenWidth, screenHeight - gridHeight));
	uiGameplayGroup.get()->setLayout(uiGameplayLayout.get());
	uiGameplayGroup.get()->setFlat(true);
	uiGameplayLayout.get()->setMargin(3);
	uiGameplayLayout.get()->addWidget(uiGameplayStatsGroup.get(), 0, 0);
	uiGameplayLayout.get()->addWidget(uiGameplayObjectivesGroup.get(), 0, 1);
	uiGameplayLayout.get()->addWidget(uiGameplayKeymapGroup.get(), 0, 3);
	uiGameplayLayout.get()->addWidget(uiGameplayMessagesGroup.get(), 0, 2);

	uiGameplayStatsGroup.get()->setLayout(uiGameplayStatsLayout.get());
	uiGameplayStatsGroup.get()->setMinimumWidth(minBoxWidth);
	uiGameplayStatsLayout.get()->setContentsMargins(5, 25, 0, 5);
	uiGameplayStatsLayout.get()->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	statCounterMap.insert(std::pair<StatCounterType, statComponent>(
		StatCounterType::TURNS_REMAINING,
		statComponent{ "Turns Remaining: ", 0, 0, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	statCounterMap.insert(std::pair<StatCounterType, statComponent>(
		StatCounterType::KEYS_HELD,
		statComponent{ "Keys: ", 1, 0, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	statCounterMap.insert(std::pair<StatCounterType, statComponent>(
		StatCounterType::TRAPS_PUSHERS,
		statComponent{ "Spring Traps: ", 2, 0, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	statCounterMap.insert(std::pair<StatCounterType, statComponent>(
		StatCounterType::TRAPS_SUCKERS,
		statComponent{ "Magnet Traps: ", 3, 0, Qt::AlignLeft | Qt::AlignTop }
		)
	);

	for (auto& entry : statCounterMap)
	{
		uiGameplayStatsLayout.get()->addWidget
		(
			entry.second.widget.get(),
			entry.second.gridLayoutRow,
			entry.second.gridLayoutCol,
			entry.second.gridLayoutAlign
		);

		entry.second.widget.get()->setStyleSheet(styleMap.at("uiGameplayLabelStyle"));
		entry.second.widget.get()->setFont(uiGameplayFontTextBox);
	}

	uiGameplayObjectivesGroup.get()->setLayout(uiGameplayObjectivesLayout.get());
	uiGameplayObjectivesGroup.get()->setMinimumWidth(minBoxWidth);
	uiGameplayObjectivesLayout.get()->setContentsMargins(5, 20, 0, 5);
	uiGameplayObjectivesLayout.get()->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	uiGameplayObjectivesLayout.get()->addWidget(uiGameplayObjectivesTextBox.get(), 0, 0, Qt::AlignLeft | Qt::AlignTop);
	uiGameplayObjectivesTextBox.get()->setStyleSheet(styleMap.at("uiGameplayTextBoxStyle"));
	uiGameplayObjectivesTextBox.get()->setFont(uiGameplayFontTextBox);
	uiGameplayObjectivesTextBox.get()->setText("Collect Keys\nOpen Pods with Keys");

	uiGameplayMessagesGroup.get()->setLayout(uiGameplayMessagesLayout.get());
	uiGameplayMessagesGroup.get()->setMinimumWidth(minBoxWidth);
	uiGameplayMessagesLayout.get()->setContentsMargins(5, 20, 0, 5);
	uiGameplayMessagesLayout.get()->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	uiGameplayMessagesLayout.get()->addWidget(uiGameplayMessagesTextBox.get(), 0, 0, Qt::AlignLeft | Qt::AlignTop);
	uiGameplayMessagesTextBox.get()->setStyleSheet(styleMap.at("uiGameplayTextBoxStyle"));
	uiGameplayMessagesTextBox.get()->setFont(uiGameplayFontTextBox);

	uiGameplayKeymapGroup.get()->setLayout(uiGameplayKeymapLayout.get());
	uiGameplayKeymapGroup.get()->setMinimumWidth(minBoxWidth);
	uiGameplayKeymapLayout.get()->setContentsMargins(5, 20, 0, 5);
	uiGameplayKeymapLayout.get()->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	for (auto& k : keybindMap)
	{
		uiGameplayKeymapLayout.get()->addWidget
		(
			k.second.uiButton.get(), 
			k.second.uiButtonGridLayoutRow, 
			k.second.uiButtonGridLayoutCol, 
			k.second.gridLayoutAlign
		);

		uiGameplayKeymapLayout.get()->addWidget
		(
			k.second.uiLabel.get(),
			k.second.uiLabelGridLayoutRow,
			k.second.uiLabelGridLayoutCol,
			k.second.gridLayoutAlign
		);

		k.second.uiButton.get()->setMinimumSize(QSize(uiGameplayKeymapBtnSize, uiGameplayKeymapBtnSize));
		k.second.uiButton.get()->setStyleSheet(styleMap.at("uiGameplayKeymapBtnStyle"));
		k.second.uiButton.get()->setFont(uiGameplayFontTextBox);
		k.second.uiButton.get()->setText(QKeySequence(k.second.keybind).toString());

		k.second.uiLabel.get()->setMinimumHeight(uiGameplayKeymapBtnSize);
		k.second.uiLabel.get()->setStyleSheet(styleMap.at("uiGameplayLabelStyle"));
		k.second.uiLabel.get()->setFont(uiGameplayFontTextBox);
		k.second.uiLabel.get()->setText(k.second.labelText);

		connect(k.second.uiButton.get(), &QPushButton::clicked, this, [&]() {
			if (gameState == GameState::PLAYING)
			{
				uiGameplayKeymapGroup.get()->setTitle(uiGameplayKeymapGroupTitleKeybinding);
				k.second.uiButton.get()->setStyleSheet(styleMap.at("uiGameplayKeymapModifBtnStyle"));
				keybindToModify = k.first;
				gameState = GameState::KEYBINDING;
			}
			else if (gameState == GameState::KEYBINDING)
			{
				uiGameplayKeymapGroup.get()->setTitle(uiGameplayKeymapGroupTitleDefault);
				keybindMap.at(keybindToModify).uiButton.get()->setStyleSheet(styleMap.at("uiGameplayKeymapBtnStyle"));
				keybindToModify = KeybindModifiable::NONE;
				gameState = GameState::PLAYING;
			}
		});
	}

	uiGameplayStatsGroup.get()->setFont(uiGameplayFontGroupTitle);
	uiGameplayObjectivesGroup.get()->setFont(uiGameplayFontGroupTitle);
	uiGameplayMessagesGroup.get()->setFont(uiGameplayFontGroupTitle);
	uiGameplayKeymapGroup.get()->setFont(uiGameplayFontGroupTitle);

	uiGameplayStatsGroup.get()->setStyleSheet(styleMap.at("uiGameplayGroupBoxStyle"));
	uiGameplayObjectivesGroup.get()->setStyleSheet(styleMap.at("uiGameplayGroupBoxStyle"));
	uiGameplayMessagesGroup.get()->setStyleSheet(styleMap.at("uiGameplayGroupBoxStyle"));
	uiGameplayKeymapGroup.get()->setStyleSheet(styleMap.at("uiGameplayGroupBoxStyle"));

	uiGameplayStatsGroup.get()->setTitle("Stats");
	uiGameplayObjectivesGroup.get()->setTitle("Objectives");
	uiGameplayMessagesGroup.get()->setTitle("Messages");
	uiGameplayKeymapGroup.get()->setTitle(uiGameplayKeymapGroupTitleDefault);

	uiMenuFontBtn.setFamily(standardFontFamily);
	uiMenuFontBtn.setPointSize(uiMenuFontBtn_fontPointSize);
	uiMenuFontBtn.setStyleStrategy(standardFontStyleStrategy);
	uiMenuFontBtn.setWeight(standardFontWeight);

	uiMenuGroup.get()->setGeometry(QRect(
		(screenWidth / 2) - (uiMenuWidth / 2), 
		(screenHeight / 2) - (uiMenuHeight / 2),
		uiMenuWidth,
		uiMenuHeight)
	);
	uiMenuGroup.get()->setLayout(uiMenuLayout.get());
	uiMenuGroup.get()->setFlat(true);
	uiMenuGroup.get()->setStyleSheet(styleMap.at("uiMenuGroupBoxStyle"));
	uiMenuLayout.get()->setMargin(0);
	uiMenuLayout.get()->setVerticalSpacing(uiMenuBtnSpacing);
	uiMenuLayout.get()->setAlignment(Qt::AlignTop);

	uiMenuBtnMap.insert(std::pair<UiMenuBtnType, uiMenuBtnComponent>(
		UiMenuBtnType::RESUME,
		uiMenuBtnComponent{ "RESUME", 0, 0, Qt::AlignTop }
		)
	);

	uiMenuBtnMap.insert(std::pair<UiMenuBtnType, uiMenuBtnComponent>(
		UiMenuBtnType::SAVE,
		uiMenuBtnComponent{ "SAVE", 1, 0, Qt::AlignTop }
		)
	);

	uiMenuBtnMap.insert(std::pair<UiMenuBtnType, uiMenuBtnComponent>(
		UiMenuBtnType::LOAD,
		uiMenuBtnComponent{ "LOAD", 2, 0, Qt::AlignTop }
		)
	);

	uiMenuBtnMap.insert(std::pair<UiMenuBtnType, uiMenuBtnComponent>(
		UiMenuBtnType::RESET,
		uiMenuBtnComponent{ "RESET LEVEL", 3, 0, Qt::AlignTop }
		)
	);

	uiMenuBtnMap.insert(std::pair<UiMenuBtnType, uiMenuBtnComponent>(
		UiMenuBtnType::EXIT,
		uiMenuBtnComponent{ "EXIT GAME", 4, 0, Qt::AlignTop }
		)
	);

	for (auto& component : uiMenuBtnMap)
	{
		uiMenuLayout.get()->addWidget
		(
			component.second.widget.get(),
			component.second.gridLayoutRow,
			component.second.gridLayoutCol,
			component.second.gridLayoutAlign
		);

		component.second.widget.get()->setFixedSize(QSize(uiMenuBtnWidth, uiMenuBtnHeight));
		component.second.widget.get()->setStyleSheet(styleMap.at("uiMenuBtnStyle"));
		component.second.widget.get()->setFont(uiMenuFontBtn);
		component.second.widget.get()->setText(component.second.text);

		switch (component.first)
		{
		case UiMenuBtnType::RESUME:
			connect(uiMenuBtnMap.at(component.first).widget.get(), &QPushButton::clicked, this, &GameplayScreen::uiMenuBtnClickResume);
			break;
		case UiMenuBtnType::SAVE:
			connect(uiMenuBtnMap.at(component.first).widget.get(), &QPushButton::clicked, this, &GameplayScreen::uiMenuBtnClickSave);
			break;
		case UiMenuBtnType::LOAD:
			connect(uiMenuBtnMap.at(component.first).widget.get(), &QPushButton::clicked, this, &GameplayScreen::uiMenuBtnClickLoad);
			break;
		case UiMenuBtnType::RESET:
			connect(uiMenuBtnMap.at(component.first).widget.get(), &QPushButton::clicked, this, &GameplayScreen::uiMenuBtnClickReset);
			break;
		case UiMenuBtnType::EXIT:
			connect(uiMenuBtnMap.at(component.first).widget.get(), &QPushButton::clicked, this, &GameplayScreen::uiMenuBtnClickExit);
			break;
		}
	}

	uiMenuGroup.get()->setVisible(false);


	int gridPosX = gridAnchorX;
	int gridPosY = gridAnchorY;
	for (int col = 0; col < gridColSize; col++)
	{
		for (int row = 0; row < gridRowSize; row++)
		{
			gridPiecesAll.emplace_back
			(
				gridPiece
				{
					QPoint(row, col),
					QPointF(gridPosX, gridPosY)
				}
			);
			gridPosX += gridPieceSize;
		}
		gridPosY += gridPieceSize;
		gridPosX = gridAnchorX;
	}

	for (auto& piece : gridPiecesAll)
	{
		QString imgKey;
		if (piece.gridPoint == QPoint(0, 0))
			imgKey = "imgGridCornerUpL";
		else if (piece.gridPoint == QPoint(gridRowSize - 1, 0))
			imgKey = "imgGridCornerUpR";
		else if (piece.gridPoint == QPoint(0, gridColSize - 1))
			imgKey = "imgGridCornerDownL";
		else if (piece.gridPoint == QPoint(gridRowSize - 1, gridColSize - 1))
			imgKey = "imgGridCornerDownR";
		else if (piece.gridPoint.x() > 0 && piece.gridPoint.x() < gridRowSize - 1 && piece.gridPoint.y() == 0)
			imgKey = "imgGridEdgeUpX";
		else if ((piece.gridPoint.x() > 0 && piece.gridPoint.x() < gridRowSize - 1) && piece.gridPoint.y() == gridColSize - 1)
			imgKey = "imgGridEdgeDownX";
		else if (piece.gridPoint.y() > 0 && piece.gridPoint.y() < gridColSize - 1 && piece.gridPoint.x() == 0)
			imgKey = "imgGridEdgeLeftY";
		else if ((piece.gridPoint.y() > 0 && piece.gridPoint.y() < gridColSize - 1) && piece.gridPoint.x() == gridRowSize - 1)
			imgKey = "imgGridEdgeRightY";
		else
			imgKey = "imgGridInner";

		piece.item.get()->setPixmap(QPixmap(imgMap.at(imgKey)));
		piece.item.get()->setPos(piece.pos);
		piece.item.get()->setZValue(gridPieceZ);
		scene.get()->addItem(piece.item.get());
	}

	dirIteratorLoadLevelData(levelDataPath);

	// After loading level data through the main expected area, we also look for any level data coming
	// from the user's documents/home area. We set this up as an extra area to look for levels, so that
	// when someone wants to add extra levels to the game beyond the default, they don't have to deal with
	// the permissions of, for example, the "program files" folder. They can place it in their documents/home
	// area instead and the game will pick it up.
	{
		QDir dirLevels(levelDataPathMods);
		if (dirLevels.exists())
			dirIteratorLoadLevelData(levelDataPathMods);
	}

	std::sort(levelsAll.begin(), levelsAll.end(), [&](const levelData &lhs, const levelData &rhs) {
		return lhs.difficulty < rhs.difficulty;
	});

	// Initialize base parameters for each level.
	// These are the default states that components get loaded in when a level is first loaded.
	for (auto& level : levelsAll)
	{
		levelSetToDefaults(level);
	}

	addCurrentLevelToScene();

	levelsAll[levelCurrent].players[pIndex].heldKeys = 0;
	levelsFound = levelsAll.size();
	levelsRemaining = levelsFound;

	uiGameplaySetToDefaults();

	turnOwner = TurnOwner::PLAYER;
}

// protected:

void GameplayScreen::keyReleaseEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
	{
		event->ignore();
	}
	else
	{
		if (gameState == GameState::TITLE)
		{
			scene.get()->removeItem(splashItem.get());
			uiGameplayGroup->setVisible(true);
			updateWindowTitle();
			gameState = GameState::PLAYING;
		}
		else if (gameState == GameState::PLAYING)
		{
			if (event->key() == keybindMap.at(KeybindModifiable::MOVE_LEFT).keybind
				|| event->key() == keybindMap.at(KeybindModifiable::MOVE_RIGHT).keybind
				|| event->key() == keybindMap.at(KeybindModifiable::MOVE_UP).keybind
				|| event->key() == keybindMap.at(KeybindModifiable::MOVE_DOWN).keybind)
			{
				if (turnOwner == TurnOwner::PLAYER)
				{
					uiGameplayMessagesTextBox.get()->setText("");

					if (event->key() == keybindMap.at(KeybindModifiable::MOVE_LEFT).keybind)
						levelsAll[levelCurrent].players[pIndex].facing = tokenPlayer::Facing::LEFT;
					else if (event->key() == keybindMap.at(KeybindModifiable::MOVE_RIGHT).keybind)
						levelsAll[levelCurrent].players[pIndex].facing = tokenPlayer::Facing::RIGHT;
					else if (event->key() == keybindMap.at(KeybindModifiable::MOVE_UP).keybind)
						levelsAll[levelCurrent].players[pIndex].facing = tokenPlayer::Facing::UP;
					else if (event->key() == keybindMap.at(KeybindModifiable::MOVE_DOWN).keybind)
						levelsAll[levelCurrent].players[pIndex].facing = tokenPlayer::Facing::DOWN;

					if (!hitSolidObjectPlayerMoving())
					{
						const auto& pItem = levelsAll[levelCurrent].players[pIndex].item.get();
						switch (levelsAll[levelCurrent].players[pIndex].facing)
						{
						case (tokenPlayer::Facing::LEFT):
							levelsAll[levelCurrent].players[pIndex].item.get()->setPos
							(
								pItem->x() - (gridPieceSize * levelsAll[levelCurrent].players[pIndex].movementSpeed),
								pItem->y()
							);
							break;
						case (tokenPlayer::Facing::RIGHT):
							levelsAll[levelCurrent].players[pIndex].item.get()->setPos
							(
								pItem->x() + (gridPieceSize * levelsAll[levelCurrent].players[pIndex].movementSpeed),
								pItem->y()
							);
							break;
						case (tokenPlayer::Facing::UP):
							levelsAll[levelCurrent].players[pIndex].item.get()->setPos
							(
								pItem->x(),
								pItem->y() - (gridPieceSize * levelsAll[levelCurrent].players[pIndex].movementSpeed)
							);
							break;
						case (tokenPlayer::Facing::DOWN):
							levelsAll[levelCurrent].players[pIndex].item.get()->setPos
							(
								pItem->x(),
								pItem->y() + (gridPieceSize * levelsAll[levelCurrent].players[pIndex].movementSpeed)
							);
							break;
						}
						levelsAll[levelCurrent].players[pIndex].item.get()->setPixmap
						(
							facingToImg(levelsAll[levelCurrent].players[pIndex].facing)
						);
						levelsAll[levelCurrent].turnsRemaining--;
						uiGameplayUpdateStatCounter(StatCounterType::TURNS_REMAINING);
						teleportHitCheck();
						turnOwner = TurnOwner::PATROLLER;
					}

					if (turnOwner == TurnOwner::PATROLLER)
					{
						updatePositionPatrollers();
						suckInRange();

						if (allGatesOpened())
						{
							turnOwner = TurnOwner::NONE;
							levelSetComplete();
						}
						else if (levelsAll[levelCurrent].turnsRemaining <= 0)
						{
							turnOwner = TurnOwner::NONE;
							levelSetFailed();
						}
						else
							turnOwner = TurnOwner::PLAYER;
					}
				}
			}
			else if (event->key() == keybindMap.at(KeybindModifiable::PLACE_PUSHER_UTIL).keybind ||
				event->key() == keybindMap.at(KeybindModifiable::PLACE_SUCKER_UTIL).keybind)
			{
				if (turnOwner == TurnOwner::PLAYER)
				{
					uiGameplayMessagesTextBox.get()->setText("");

					auto& pushI = levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex;
					auto& suckI = levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex;

					if (event->key() == keybindMap.at(KeybindModifiable::PLACE_PUSHER_UTIL).keybind 
						&& pushI.size() > 0)
					{
						levelsAll[levelCurrent].utils[pushI[0]].item.get()->setPos
						(
							levelsAll[levelCurrent].players[pIndex].item.get()->x(),
							levelsAll[levelCurrent].players[pIndex].item.get()->y()
						);
						levelsAll[levelCurrent].utils[pushI[0]].stateModified = tokenUtil::State::ACTIVE;
						levelsAll[levelCurrent].utils[pushI[0]].item.get()->setPixmap
						(
							stateToImg(levelsAll[levelCurrent].utils[pushI[0]].stateModified, levelsAll[levelCurrent].utils[pushI[0]].type)
						);
						pushI.erase(pushI.begin() + 0);
						uiGameplayUpdateStatCounter(StatCounterType::TRAPS_PUSHERS);
						uiGameplayMessagesTextBox->setText(uiGameplayMessagesTrapPusherDeployed);

						levelsAll[levelCurrent].players[pIndex].facing = tokenPlayer::Facing::NEUTRAL;
						levelsAll[levelCurrent].players[pIndex].item.get()->setPixmap
						(
							facingToImg(levelsAll[levelCurrent].players[pIndex].facing)
						);

						turnOwner = TurnOwner::PATROLLER;
					}
					else if (event->key() == keybindMap.at(KeybindModifiable::PLACE_SUCKER_UTIL).keybind 
						&& suckI.size() > 0)
					{
						levelsAll[levelCurrent].utils[suckI[0]].item.get()->setPos
						(
							levelsAll[levelCurrent].players[pIndex].item.get()->x(),
							levelsAll[levelCurrent].players[pIndex].item.get()->y()
						);
						levelsAll[levelCurrent].utils[suckI[0]].stateModified = tokenUtil::State::ACTIVE;
						levelsAll[levelCurrent].utils[suckI[0]].item.get()->setPixmap
						(
							stateToImg(levelsAll[levelCurrent].utils[suckI[0]].stateModified, levelsAll[levelCurrent].utils[suckI[0]].type)
						);
						suckI.erase(suckI.begin() + 0);
						uiGameplayUpdateStatCounter(StatCounterType::TRAPS_SUCKERS);
						uiGameplayMessagesTextBox->setText(uiGameplayMessagesTrapSuckerDeployed);

						levelsAll[levelCurrent].players[pIndex].facing = tokenPlayer::Facing::NEUTRAL;
						levelsAll[levelCurrent].players[pIndex].item.get()->setPixmap
						(
							facingToImg(levelsAll[levelCurrent].players[pIndex].facing)
						);

						turnOwner = TurnOwner::PATROLLER;
					}

					if (turnOwner == TurnOwner::PATROLLER)
					{
						updatePositionPatrollers();
						suckInRange();

						if (allGatesOpened())
						{
							turnOwner = TurnOwner::NONE;
							levelSetComplete();
						}
						else if (levelsAll[levelCurrent].turnsRemaining <= 0)
						{
							turnOwner = TurnOwner::NONE;
							levelSetFailed();
						}
						else
							turnOwner = TurnOwner::PLAYER;
					}
				}
			}
			else if (event->key() == keybindMap.at(KeybindModifiable::OPEN_MENU).keybind)
			{
				if (turnOwner == TurnOwner::PLAYER)
				{
					gameState = GameState::PAUSED;
					uiMenuGroup.get()->setVisible(true);
				}
			}
			else if (event->key() == keybindSkipLevel_DEBUG)
			{
				levelSetComplete();
			}
			else if (event->key() == keybindLoadLevelByName_DEBUG)
			{
				QStringList levelNames;
				for (const auto& level : levelsAll)
				{
					levelNames.append(level.name + " by " + level.creator);
				}

				bool ok;
				QString level = QInputDialog::getItem
				(
					this->parentWidget(),
					tr("Jump To Level"), 
					tr("Level:"), 
					levelNames,
					levelCurrent, 
					false, 
					&ok, 
					Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint
				);

				if (ok && !level.isEmpty())
				{
					// Logic could be a little cleaner. Using levelSetComplete causes
					// the current level to be set as complete even if it isn't,
					// meaning if you naturally come back to a level after jumping backwards,
					// you'll skip over. Since this is a DEBUG command for testing, it's not super important.
					// What matters most is the jumping to functionally works to enable quick testing.
					levelSetComplete();

					levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex.clear();
					levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex.clear();
					removeCurrentLevelFromScene();
					levelCurrent = levelNames.indexOf(level);
					addCurrentLevelToScene();
					levelSetToDefaults(levelsAll[levelCurrent]);
					scene.get()->removeItem(splashItem.get());
					uiGameplaySetToDefaults();
					uiGameplayGroup->setVisible(true);
					updateWindowTitle();
					gameState = GameState::PLAYING;
					turnOwner = TurnOwner::PLAYER;
				}
			}
		}
		else if (gameState == GameState::PAUSED)
		{
			if (event->key() == keybindMap.at(KeybindModifiable::OPEN_MENU).keybind)
			{
				uiMenuResumePlay();
			}
		}
		else if (gameState == GameState::KEYBINDING)
		{
			Qt::Key newKeybind = Qt::Key(event->key());

			if (newKeybind == keybindMap.at(KeybindModifiable::MOVE_LEFT).keybind ||
				newKeybind == keybindMap.at(KeybindModifiable::MOVE_RIGHT).keybind ||
				newKeybind == keybindMap.at(KeybindModifiable::MOVE_UP).keybind ||
				newKeybind == keybindMap.at(KeybindModifiable::MOVE_DOWN).keybind ||
				newKeybind == keybindMap.at(KeybindModifiable::PLACE_PUSHER_UTIL).keybind ||
				newKeybind == keybindMap.at(KeybindModifiable::PLACE_SUCKER_UTIL).keybind ||
				newKeybind == keybindMap.at(KeybindModifiable::OPEN_MENU).keybind ||
				newKeybind == keybindNextLevel ||
				newKeybind == keybindResetLevel ||
				newKeybind == keybindSkipLevel_DEBUG ||
				newKeybind == keybindLoadLevelByName_DEBUG)
			{
				// Logic here is a bit redundant. Could maybe be improved.

				QMessageBox qMsg(this->parentWidget());
				qMsg.setStyleSheet(styleMap.at("uiMessageBoxStyle"));
				qMsg.setWindowTitle("Keybind In Use");
				qMsg.setStandardButtons(QMessageBox::Ok);
				qMsg.setDefaultButton(QMessageBox::Ok);
				qMsg.setFont(uiGameplayFontTextBox);
				qMsg.button(QMessageBox::Ok)->setFont(uiGameplayFontTextBox);

				for (const auto& k : keybindMap)
				{
					if (newKeybind == k.second.keybind)
					{
						qMsg.setText("\"" + QKeySequence(newKeybind).toString() + "\" is already bound to the \"" + k.second.labelText + "\" command.");
						qMsg.exec();
						return;
					}
				}

				if (newKeybind == keybindNextLevel)
				{
					qMsg.setText("\"" + QKeySequence(newKeybind).toString() + "\" is already bound to the \"" + "Next Level" + "\" command.");
					qMsg.exec();
					return;
				}
				else if (newKeybind == keybindResetLevel)
				{
					qMsg.setText("\"" + QKeySequence(newKeybind).toString() + "\" is already bound to the \"" + "Reset Level" + "\" command.");
					qMsg.exec();
					return;
				}
				else if (newKeybind == keybindSkipLevel_DEBUG)
				{
					qMsg.setText("\"" + QKeySequence(newKeybind).toString() + "\" is already bound to the \"" + "Skip Level DEBUG" + "\" command.");
					qMsg.exec();
					return;
				}
				else if (newKeybind == keybindLoadLevelByName_DEBUG)
				{
					qMsg.setText("\"" + QKeySequence(newKeybind).toString() + "\" is already bound to the \"" + "Jump To Level DEBUG" + "\" command.");
					qMsg.exec();
					return;
				}
			}
			else if (newKeybind == Qt::Key_Control || newKeybind == Qt::Key_Shift)
			{
				QMessageBox qMsg(this->parentWidget());
				qMsg.setStyleSheet(styleMap.at("uiMessageBoxStyle"));
				qMsg.setWindowTitle("Keybind Invalid");
				qMsg.setText("Ctrl and Shift are not valid key input.");
				qMsg.setStandardButtons(QMessageBox::Ok);
				qMsg.setDefaultButton(QMessageBox::Ok);
				qMsg.setFont(uiGameplayFontTextBox);
				qMsg.button(QMessageBox::Ok)->setFont(uiGameplayFontTextBox);
				qMsg.exec();
			}
			else
			{
				keybindMap.at(keybindToModify).keybind = newKeybind;
				keybindMap.at(keybindToModify).uiButton.get()->setStyleSheet(styleMap.at("uiGameplayKeymapBtnStyle"));
				keybindMap.at(keybindToModify).uiButton.get()->setText(QKeySequence(keybindMap.at(keybindToModify).keybind).toString());
				uiGameplayKeymapGroup.get()->setTitle(uiGameplayKeymapGroupTitleDefault);
				keybindToModify = KeybindModifiable::NONE;
				gameState = GameState::PLAYING;
			}
		}
		else if (gameState == GameState::LEVEL_COMPLETE)
		{
			if (event->key() == keybindNextLevel)
			{
				levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex.clear();
				levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex.clear();
				removeCurrentLevelFromScene();
				levelCurrent++;

				// We do a looping check on the next level to see if it's already set as complete.
				// This is to account for levels that are set to COMPLETE through a loaded save.
				// Normally, this would never be relevant, since levels are played in order and 
				// so you wouldn't hit one that is complete.
				// In fact, if file directory load order is consistent, it could be irrelevant even in save loading.
				// But we want to account for an edge case where a completed level comes after the current loaded one in a save.
				// Otherwise, player may have to complete the same levels twice, which makes Save functionality kinda pointless.
				while (levelsAll[levelCurrent].state == levelData::State::COMPLETE)
				{
					levelCurrent++;
				}

				qDebug() << "**DEBUG** Current Level Id: " + levelsAll[levelCurrent].id;

				addCurrentLevelToScene();
				scene.get()->removeItem(splashItem.get());
				uiGameplaySetToDefaults();
				uiGameplayGroup->setVisible(true);
				updateWindowTitle();
				gameState = GameState::PLAYING;
				turnOwner = TurnOwner::PLAYER;
			}
		}
		else if (gameState == GameState::LEVEL_FAILED)
		{
			if (event->key() == keybindResetLevel)
			{
				levelSetToDefaults(levelsAll[levelCurrent]);
				scene.get()->removeItem(splashItem.get());
				uiGameplaySetToDefaults();
				uiGameplayGroup->setVisible(true);
				gameState = GameState::PLAYING;
				turnOwner = TurnOwner::PLAYER;
			}
		}
	}
}

void GameplayScreen::prefSave()
{
	QFile fileWrite(windowsHomePath + "/config.txt");
	if (fileWrite.open(QIODevice::WriteOnly))
	{
		QTextStream qStream(&fileWrite);

		qStream << "KEYBINDS: \r\n";
		for (const auto& k : keybindMap)
		{
			// We remove any spaces from the identifier for stricter specificity of searching,
			// e.g. "Open Menu" becomes "OpenMenu", then we search for "OpenMenu=" when loading.
			QString identifier = k.second.labelText;
			qStream << 
				identifier.remove(QChar(' '), Qt::CaseSensitive) +
				"=" +
				QKeySequence(k.second.keybind).toString() +
				"\r\n"
				;
		}
		fileWrite.close();
	}
}

// private:

void GameplayScreen::prefLoad()
{
	// If the config file exists in the home path we assume first time setup has happened.
	// Otherwise, we assume it hasn't and run first time setup to generate home path folders/files.
	const QString configPath = windowsHomePath + "/config.txt";
	if (!QFile(configPath).exists())
		return;
	else
		firstTimeSetup = false;

	QFile fileRead(configPath);
	if (fileRead.open(QIODevice::ReadOnly))
	{
		QTextStream qStream(&fileRead);
		while (!qStream.atEnd())
		{
			QString line = qStream.readLine();

			for (auto& k : keybindMap)
			{
				QString identifier = k.second.labelText;
				if (line.contains(identifier.remove(QChar(' '), Qt::CaseSensitive) + "="))
				{
					QString newKey = extractSubstringInbetweenQt("=", "", line);
					if (!newKey.isEmpty())
						k.second.keybind = Qt::Key(QKeySequence::fromString(newKey)[0]);
					continue;
				}
			}
		}
		fileRead.close();
	}
}

void GameplayScreen::dirIteratorLoadLevelData(const QString &dirPath)
{
	// Get all paths of data files and store them in vectors as strings
	qDebug() << dirPath;
	QDirIterator dirIt(dirPath, QDir::AllEntries | QDir::NoDotAndDotDot);
	while (dirIt.hasNext())
	{
		QString filePath = dirIt.next();
		qDebug() << filePath;

		if (QFileInfo(filePath).suffix() != levelDataFileExtension)
			continue;

		QString fileContents;
		QFile fileRead(filePath);
		if (fileRead.open(QIODevice::ReadOnly))
		{
			bool validLevelFound = true;
			levelData newLevelData;
			QTextStream qStream(&fileRead);
			while (!qStream.atEnd())
			{
				QString line = qStream.readLine();
				if (line.contains("::Id="))
				{
					newLevelData.id = extractSubstringInbetweenQt("::Id=", "::", line);
					newLevelData.creator = extractSubstringInbetweenQt("::CreatorName=", "::", line);
					newLevelData.name = extractSubstringInbetweenQt("::LevelName=", "::", line);
					newLevelData.difficulty = extractSubstringInbetweenQt("::LevelDifficulty=", "::", line).toInt();
					newLevelData.turnsInitial = extractSubstringInbetweenQt("::TurnsRemaining=", "::", line).toInt();
					newLevelData.turnsRemaining = newLevelData.turnsInitial;
				}
				else if (line.contains("::Gate=") && line.contains("::Key="))
				{
					// Need checks here to make sure number of Gate and number of Key is the same
					// Failing checks means NOT a valid level, leave out of level list

					QString gateData = extractSubstringInbetweenQt("::Gate=", "::", line);
					QString keyData = extractSubstringInbetweenQt("::Key=", "::", line);

					QStringList gateList = extractSubstringInbetweenQtLoopList("(", ")", gateData);
					QStringList keyList = extractSubstringInbetweenQtLoopList("(", ")", keyData);

					if (gateList.length() != keyList.length())
						validLevelFound = false;

					for (const auto& gate : gateList)
					{
						QStringList coords = gate.split(",", QString::SkipEmptyParts);
						newLevelData.gates.emplace_back
						(
							tokenImmobile
							{
								coords[0].toInt(),
								coords[1].toInt(),
								tokenImmobile::Type::GATE
							}
						);
					}
					for (const auto& key : keyList)
					{
						QStringList coords = key.split(",", QString::SkipEmptyParts);
						newLevelData.keys.emplace_back
						(
							tokenImmobile
							{
								coords[0].toInt(),
								coords[1].toInt(),
								tokenImmobile::Type::KEY
							}
						);
					}
				}
				else if (line.contains("::Player="))
				{
					QStringList coords = extractSubstringInbetweenQt("::Player=", "::", line).split(",", QString::SkipEmptyParts);
					newLevelData.players.emplace_back
					(
						tokenPlayer
						{
							coords[0].toInt(),
							coords[1].toInt()
						}
					);
				}
				else if (line.contains("::Pusher="))
				{
					QString pusherData = extractSubstringInbetweenQt("::Pusher=", "::", line);
					QStringList pusherList = extractSubstringInbetweenQtLoopList("(", ")", pusherData);
					for (const auto& pusher : pusherList)
					{
						QStringList components = pusher.split(",", QString::SkipEmptyParts);
						newLevelData.pushers.emplace_back
						(
							tokenPatroller
							{
								components[0].toInt(),
								components[1].toInt(),
								tokenPatroller::typeToEnum(components[2]),
								tokenPatroller::facingToEnum(components[3]),
								tokenPatroller::facingToEnum(components[3]),
								tokenPatroller::patrolDirToEnum(components[4]),
								components[5].toInt(),
								components[6].toInt(),
								components[7].toInt(),
								components[8].toInt()
							}
						);
					}
				}
				else if (line.contains("::Sucker="))
				{
					QString suckerData = extractSubstringInbetweenQt("::Sucker=", "::", line);
					QStringList suckerList = extractSubstringInbetweenQtLoopList("(", ")", suckerData);
					for (const auto& pusher : suckerList)
					{
						QStringList components = pusher.split(",", QString::SkipEmptyParts);
						newLevelData.suckers.emplace_back
						(
							tokenPatroller
							{
								components[0].toInt(),
								components[1].toInt(),
								tokenPatroller::typeToEnum(components[2]),
								tokenPatroller::facingToEnum(components[3]),
								tokenPatroller::facingToEnum(components[3]),
								tokenPatroller::patrolDirToEnum(components[4]),
								components[5].toInt(),
								components[6].toInt(),
								components[7].toInt(),
								components[8].toInt()
							}
						);
					}
				}
				else if (line.contains("::Util="))
				{
					QString utilData = extractSubstringInbetweenQt("::Util=", "::", line);
					QStringList utilList = extractSubstringInbetweenQtLoopList("(", ")", utilData);
					for (const auto& util : utilList)
					{
						// When loading a level for the first time, utils should always be INACTIVE.
						// For reusability of code and loading procedures, we look for state regardless.
						// This way for loading an in-progress level, varying state can be loaded as needed.
						QStringList components = util.split(",", QString::SkipEmptyParts);
						newLevelData.utils.emplace_back
						(
							tokenUtil
							{
								components[0].toInt(),
								components[1].toInt(),
								tokenUtil::typeToEnum(components[2]),
								tokenUtil::stateToEnum(components[3])
							}
						);
					}
				}
				else if (line.contains("::Block="))
				{
					QString blockData = extractSubstringInbetweenQt("::Block=", "::", line);
					QStringList blockList = extractSubstringInbetweenQtLoopList("(", ")", blockData);
					for (const auto& block : blockList)
					{
						QStringList coords = block.split(",", QString::SkipEmptyParts);
						newLevelData.blocks.emplace_back
						(
							tokenImmobile
							{
								coords[0].toInt(),
								coords[1].toInt(),
								tokenImmobile::Type::BLOCK
							}
						);
					}
				}
				else if (line.contains("::Hazard="))
				{
					QString hazardData = extractSubstringInbetweenQt("::Hazard=", "::", line);
					QStringList hazardList = extractSubstringInbetweenQtLoopList("(", ")", hazardData);
					for (const auto& hazard : hazardList)
					{
						QStringList coords = hazard.split(",", QString::SkipEmptyParts);
						newLevelData.hazards.emplace_back
						(
							tokenImmobile
							{
								coords[0].toInt(),
								coords[1].toInt(),
								tokenImmobile::Type::HAZARD
							}
						);
					}
				}
				else if (line.contains("::Teleport="))
				{
					QString teleportData = extractSubstringInbetweenQt("::Teleport=", "::", line);
					QStringList teleportList = extractSubstringInbetweenQtLoopList("(", ")", teleportData);
					for (const auto& teleport : teleportList)
					{
						QStringList coords = teleport.split(",", QString::SkipEmptyParts);
						newLevelData.teleports.emplace_back
						(
							tokenImmobile
							{
								coords[0].toInt(),
								coords[1].toInt(),
								tokenImmobile::Type::TELEPORT
							}
						);
					}
				}
			}

			if (validLevelFound)
				levelsAll.emplace_back(std::move(newLevelData));
		}
		fileRead.close();
	}
}

bool GameplayScreen::hitSolidObjectPlayerMoving()
{
	// Areas beyond grid edges are solid objects
	// Patroller and player are solid objects
	// Gate is considered solid, unless a player with a key is hitting it
	// Keys are not solid for player, and are solid for enemies

	const auto& pItem = levelsAll[levelCurrent].players[pIndex].item.get();
	const auto& pAtIndex = levelsAll[levelCurrent].players[pIndex];

	int nextY;
	int nextX;

	switch (levelsAll[levelCurrent].players[pIndex].facing)
	{
	case (tokenPlayer::Facing::UP):
		nextY = pItem->y() - (gridPieceSize * pAtIndex.movementSpeed);
		nextX = pItem->x();
		if (nextY < gridBoundUp)
		{
			return true;
		}
		break;
	case (tokenPlayer::Facing::DOWN):
		nextY = pItem->y() + (gridPieceSize * pAtIndex.movementSpeed);
		nextX = pItem->x();
		if (nextY > gridBoundDown)
		{
			return true;
		}
		break;
	case (tokenPlayer::Facing::LEFT):
		nextY = pItem->y();
		nextX = pItem->x() - (gridPieceSize * pAtIndex.movementSpeed);
		if (nextX < gridBoundLeft)
		{
			return true;
		}
		break;
	case (tokenPlayer::Facing::RIGHT):
		nextY = pItem->y();
		nextX = pItem->x() + (gridPieceSize * pAtIndex.movementSpeed);
		if (nextX > gridBoundRight)
		{
			return true;
		}
		break;
	}


	if (hitImmobileObject(levelsAll[levelCurrent].blocks, nextY, nextX))
	{
		uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesObstacle);
		return true;
	}
	else if (hitImmobileObject(levelsAll[levelCurrent].hazards, nextY, nextX))
	{
		uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesHazard);
		return true;
	}
	else if (hitImmobileObject(levelsAll[levelCurrent].teleports, nextY, nextX))
	{
		uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesTeleport);
		return false;
	}
	else if (hitEnemy(levelsAll[levelCurrent].suckers, nextY, nextX))
	{
		return true;
	}
	else if (hitEnemy(levelsAll[levelCurrent].pushers, nextY, nextX))
	{
		for (int i = 0; i < levelsAll[levelCurrent].players[pIndex].knockbackAmount; i++)
		{
			knockbackPlayerMoving();
		}
		hazardHitCheck();
		teleportHitCheck();
		turnOwner = TurnOwner::PATROLLER;
		return true;
	}
	else if (hitEnemy(levelsAll[levelCurrent].suckers, nextY, nextX))
	{
		turnOwner = TurnOwner::PATROLLER;
		return true;
	}
	else if (hitUtil(nextY, nextX))
	{
		return false;
	}
	else if (hitImmobileObject(levelsAll[levelCurrent].gates, nextY, nextX))
	{
		if (levelsAll[levelCurrent].players[pIndex].heldKeys > 0)
		{
			hitImmobileObjectAndDelete(levelsAll[levelCurrent].gates, nextY, nextX);
			levelsAll[levelCurrent].players[pIndex].heldKeys--;
			uiGameplayUpdateStatCounter(StatCounterType::KEYS_HELD);
			return false;
		}
		else
		{
			uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesKeyNeeded);
			return true;
		}
	}
	else if (hitImmobileObjectAndDelete(levelsAll[levelCurrent].keys, nextY, nextX))
	{
		levelsAll[levelCurrent].players[pIndex].heldKeys++;
		uiGameplayUpdateStatCounter(StatCounterType::KEYS_HELD);
		uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesKeyObtained);
		return false;
	}
	else
	{
		return false;
	}
}

bool GameplayScreen::hitImmobileObject(const std::vector<tokenImmobile>& immobiles, const int playerNextY, const int playerNextX)
{
	const int numImmobiles = immobiles.size();
	for (int i = 0; i < numImmobiles; i++)
	{
		if (playerNextY == immobiles[i].item.get()->y()
			&& playerNextX == immobiles[i].item.get()->x()
			&& immobiles[i].state != tokenImmobile::State::INVISIBLE)
		{
			return true;
		}
	}
	return false;
}

bool GameplayScreen::hitImmobileObjectAndDelete(std::vector<tokenImmobile>& immobiles, const int playerNextY, const int playerNextX)
{
	const int numImmobiles = immobiles.size();
	for (int i = 0; i < numImmobiles; i++)
	{
		if (playerNextY == immobiles[i].item.get()->y()
			&& playerNextX == immobiles[i].item.get()->x()
			&& immobiles[i].state == tokenImmobile::State::ACTIVE)
		{
			if (immobiles[i].type == tokenImmobile::Type::KEY)
				immobiles[i].state = tokenImmobile::State::HELD;
			else
				immobiles[i].state = tokenImmobile::State::INVISIBLE;
			immobiles[i].item.get()->setPixmap
			(
				stateToImg(immobiles[i].state, immobiles[i].type)
			);
			return true;
		}
	}
	return false;
}

bool GameplayScreen::hitEnemy(const std::vector<tokenPatroller>& patrollers, const int playerNextY, const int playerNextX)
{
	const int tokens = patrollers.size();
	for (int i = 0; i < tokens; i++)
	{
		if (playerNextY == patrollers[i].item.get()->y() && playerNextX == patrollers[i].item.get()->x())
		{
			return true;
		}
	}
	return false;
}

bool GameplayScreen::hitUtil(const int playerNextY, const int playerNextX)
{
	const int numUtils = levelsAll[levelCurrent].utils.size();
	for (int i = 0; i < numUtils; i++)
	{
		if (playerNextY == levelsAll[levelCurrent].utils[i].item.get()->y()
			&& playerNextX == levelsAll[levelCurrent].utils[i].item.get()->x())
		{
			if (levelsAll[levelCurrent].utils[i].stateModified != tokenUtil::State::HELD)
			{
				if (levelsAll[levelCurrent].utils[i].type == tokenUtil::Type::PUSHER)
				{
					levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex.push_back(i);
					uiGameplayUpdateStatCounter(StatCounterType::TRAPS_PUSHERS);
					uiGameplayMessagesTextBox->setText(uiGameplayMessagesTrapPusherObtained);
				}
				else if (levelsAll[levelCurrent].utils[i].type == tokenUtil::Type::SUCKER)
				{
					levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex.push_back(i);
					uiGameplayUpdateStatCounter(StatCounterType::TRAPS_SUCKERS);
					uiGameplayMessagesTextBox->setText(uiGameplayMessagesTrapSuckerObtained);
				}
				levelsAll[levelCurrent].utils[i].stateModified = tokenUtil::State::HELD;
				levelsAll[levelCurrent].utils[i].item.get()->setPixmap
				(
					stateToImg(levelsAll[levelCurrent].utils[i].stateModified, levelsAll[levelCurrent].utils[i].type)
				);

				return true;
			}
		}
	}
	return false;
}

void GameplayScreen::knockbackPlayerMoving()
{
	// If player is moving into enemy, we knockback player in the opposite direction

	const auto& pItem = levelsAll[levelCurrent].players[pIndex].item.get();
	const auto& pAtIndex = levelsAll[levelCurrent].players[pIndex];

	int nextY;
	int nextX;

	switch (levelsAll[levelCurrent].players[pIndex].facing)
	{
	case (tokenPlayer::Facing::UP):
		nextY = pItem->y() + (gridPieceSize * pAtIndex.movementSpeed);
		nextX = pItem->x();
		if (nextY < gridBoundDown)
		{
			pItem->setPos(nextX, nextY);
		}
		else
		{
			pItem->setPos(nextX, gridBoundDown - gridPieceSize + (gridPieceSize / 4));
		}
		break;
	case (tokenPlayer::Facing::DOWN):
		nextY = pItem->y() - (gridPieceSize * pAtIndex.movementSpeed);
		nextX = pItem->x();
		if (nextY > gridBoundUp)
		{
			pItem->setPos(nextX, nextY);
		}
		else
		{
			pItem->setPos(nextX, gridBoundUp + (gridPieceSize / 4));
		}
		break;
	case (tokenPlayer::Facing::LEFT):
		nextY = pItem->y();
		nextX = pItem->x() + (gridPieceSize * pAtIndex.movementSpeed);
		if (nextX < gridBoundRight)
		{
			pItem->setPos(nextX, nextY);
		}
		else
		{
			pItem->setPos(gridBoundRight - gridPieceSize + (gridPieceSize / 4), nextY);
		}
		break;
	case (tokenPlayer::Facing::RIGHT):
		nextY = pItem->y();
		nextX = pItem->x() - (gridPieceSize * pAtIndex.movementSpeed);
		if (nextX > gridBoundLeft)
		{
			pItem->setPos(nextX, nextY);
		}
		else
		{
			pItem->setPos(gridBoundLeft + (gridPieceSize / 4), nextY);
		}
		break;
	}

	nextY = pItem->y();
	nextX = pItem->x();

	if (hitImmobileObjectAndDelete(levelsAll[levelCurrent].blocks, nextY, nextX)
		|| hitEnemy(levelsAll[levelCurrent].pushers, nextY, nextX)
		|| hitEnemy(levelsAll[levelCurrent].suckers, nextY, nextX))
	{
		return;
	}
	else if (hitImmobileObject(levelsAll[levelCurrent].gates, nextY, nextX))
	{
		if (levelsAll[levelCurrent].players[pIndex].heldKeys > 0)
		{
			hitImmobileObjectAndDelete(levelsAll[levelCurrent].gates, nextY, nextX);
			levelsAll[levelCurrent].players[pIndex].heldKeys--;
			uiGameplayUpdateStatCounter(StatCounterType::KEYS_HELD);
			return;
		}
		else
		{
			uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesKeyNeeded);
			return;
		}
	}
	else if (hitImmobileObjectAndDelete(levelsAll[levelCurrent].keys, nextY, nextX))
	{
		levelsAll[levelCurrent].players[pIndex].heldKeys++;
		uiGameplayUpdateStatCounter(StatCounterType::KEYS_HELD);
		uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesKeyObtained);
		return;
	}
}

void GameplayScreen::updatePositionPatrollers()
{
	const int numPushers = levelsAll[levelCurrent].pushers.size();
	for (int i = 0; i < numPushers; i++)
	{
		updatePositionPatrollerMoves(levelsAll[levelCurrent].pushers, i);
	}

	const int numSuckers = levelsAll[levelCurrent].suckers.size();
	for (int i = 0; i < numSuckers; i++)
	{
		updatePositionPatrollerMoves(levelsAll[levelCurrent].suckers, i);
	}
}

void GameplayScreen::updatePositionPatrollerMoves(std::vector<tokenPatroller>& patrollers, const int enemyNum)
{
	const auto& eItem = patrollers[enemyNum].item.get();

	switch (patrollers[enemyNum].patrolDir)
	{
	case tokenPatroller::PatrolDir::VERTICAL:
	{
		switch (patrollers[enemyNum].facing)
		{
		case tokenPatroller::Facing::UP:
			if (patrollers[enemyNum].item.get()->y() - gridPieceSize
				< patrollers[enemyNum].initialY - (patrollers[enemyNum].patrolBoundUp * gridPieceSize))
			{
				patrollers[enemyNum].facing = tokenPatroller::Facing::DOWN;
				patrollers[enemyNum].item.get()->setPixmap
				(
					facingToImg(patrollers[enemyNum].facing, patrollers[enemyNum].type)
				);
			}
			else
			{
				int nextY = eItem->y() - (gridPieceSize * patrollers[enemyNum].movementSpeed);
				int nextX = eItem->x();
				if (patrollerHitTrapPush(patrollers, enemyNum, nextY, nextX))
				{
					for (int i = 0; i < 2; i++)
					{
						knockbackHitTrap(patrollers, enemyNum);
					}
				}
				else
				{
					eItem->setPos(eItem->x(), nextY);
					suckInHitTrap(patrollers, enemyNum);
					if (hitPlayer(patrollers, enemyNum))
					{
						if (patrollers[enemyNum].type == tokenPatroller::Type::PUSHER)
						{
							for (int i = 0; i < levelsAll[levelCurrent].players[pIndex].knockbackAmount; i++)
							{
								knockbackEnemyMoving(enemyNum);
							}
							hazardHitCheck();
							teleportHitCheck();
						}
					}
				}
			}
			break;
		case tokenPatroller::Facing::DOWN:
			if (eItem->y() + gridPieceSize
							> patrollers[enemyNum].initialY + (patrollers[enemyNum].patrolBoundDown * gridPieceSize))
			{
				patrollers[enemyNum].facing = tokenPatroller::Facing::UP;
				patrollers[enemyNum].item.get()->setPixmap
				(
					facingToImg(patrollers[enemyNum].facing, patrollers[enemyNum].type)
				);
			}
			else
			{
				int nextY = eItem->y() + (gridPieceSize * patrollers[enemyNum].movementSpeed);
				int nextX = eItem->x();
				if (patrollerHitTrapPush(patrollers, enemyNum, nextY, nextX))
				{
					for (int i = 0; i < 2; i++)
					{
						knockbackHitTrap(patrollers, enemyNum);
					}
				}
				else
				{
					eItem->setPos(eItem->x(), nextY);
					suckInHitTrap(patrollers, enemyNum);
					if (hitPlayer(patrollers, enemyNum))
					{
						if (patrollers[enemyNum].type == tokenPatroller::Type::PUSHER)
						{
							for (int i = 0; i < levelsAll[levelCurrent].players[pIndex].knockbackAmount; i++)
							{
								knockbackEnemyMoving(enemyNum);
							}
							hazardHitCheck();
							teleportHitCheck();
						}
					}
				}
			}
			break;
		}
	}
	break;
	case tokenPatroller::PatrolDir::HORIZONTAL:
	{
		switch (patrollers[enemyNum].facing)
		{
		case tokenPatroller::Facing::LEFT:
			if (eItem->x() - gridPieceSize
				< patrollers[enemyNum].initialX - (patrollers[enemyNum].patrolBoundLeft * gridPieceSize))
			{
				patrollers[enemyNum].facing = tokenPatroller::Facing::RIGHT;
				patrollers[enemyNum].item.get()->setPixmap
				(
					facingToImg(patrollers[enemyNum].facing, patrollers[enemyNum].type)
				);
			}
			else
			{
				int nextY = eItem->y();
				int nextX = eItem->x() - (gridPieceSize * patrollers[enemyNum].movementSpeed);
				if (patrollerHitTrapPush(patrollers, enemyNum, nextY, nextX))
				{
					for (int i = 0; i < 2; i++)
					{
						knockbackHitTrap(patrollers, enemyNum);
					}
				}
				else
				{
					eItem->setPos(nextX, eItem->y());
					suckInHitTrap(patrollers, enemyNum);
					if (hitPlayer(patrollers, enemyNum))
					{
						if (patrollers[enemyNum].type == tokenPatroller::Type::PUSHER)
						{
							for (int i = 0; i < levelsAll[levelCurrent].players[pIndex].knockbackAmount; i++)
							{
								knockbackEnemyMoving(enemyNum);
							}
							hazardHitCheck();
							teleportHitCheck();
						}
					}
				}
			}
			break;
		case tokenPatroller::Facing::RIGHT:
			if (eItem->x() + gridPieceSize
							> patrollers[enemyNum].initialX + (patrollers[enemyNum].patrolBoundRight * gridPieceSize))
			{
				patrollers[enemyNum].facing = tokenPatroller::Facing::LEFT;
				patrollers[enemyNum].item.get()->setPixmap
				(
					facingToImg(patrollers[enemyNum].facing, patrollers[enemyNum].type)
				);
			}
			else
			{
				int nextY = eItem->y();
				int nextX = eItem->x() + (gridPieceSize * patrollers[enemyNum].movementSpeed);
				if (patrollerHitTrapPush(patrollers, enemyNum, nextY, nextX))
				{
					for (int i = 0; i < 2; i++)
					{
						knockbackHitTrap(patrollers, enemyNum);
					}
				}
				else
				{
					eItem->setPos(nextX, eItem->y());
					suckInHitTrap(patrollers, enemyNum);
					if (hitPlayer(patrollers, enemyNum))
					{
						if (patrollers[enemyNum].type == tokenPatroller::Type::PUSHER)
						{
							for (int i = 0; i < levelsAll[levelCurrent].players[pIndex].knockbackAmount; i++)
							{
								knockbackEnemyMoving(enemyNum);
							}
							hazardHitCheck();
							teleportHitCheck();
						}
					}
				}
			}
			break;
		}
	}
	break;
	}
}

void GameplayScreen::knockbackEnemyMoving(int enemyNum)
{
	// If enemy is moving into player, we knockback player in the direction enemy is moving

	const auto& pItem = levelsAll[levelCurrent].players[pIndex].item.get();
	const auto& pAtIndex = levelsAll[levelCurrent].players[pIndex];

	int nextY;
	int nextX;

	switch (levelsAll[levelCurrent].pushers[enemyNum].facing)
	{
	case (tokenPatroller::Facing::UP):
		nextY = pItem->y() - (gridPieceSize * pAtIndex.movementSpeed);
		nextX = pItem->x();
		if (nextY > gridBoundUp)
		{
			pItem->setPos(pItem->x(), nextY);
		}
		else
		{
			pItem->setPos(pItem->x(), gridBoundUp + (gridPieceSize / 4));
		}
		break;
	case (tokenPatroller::Facing::DOWN):
		nextY = pItem->y() + (gridPieceSize * pAtIndex.movementSpeed);
		nextX = pItem->x();
		if (nextY < gridBoundDown)
		{
			pItem->setPos(pItem->x(), nextY);
		}
		else
		{
			pItem->setPos(pItem->x(), gridBoundDown - gridPieceSize + (gridPieceSize / 4));
		}
		break;
	case (tokenPatroller::Facing::LEFT):
		nextY = pItem->y();
		nextX = pItem->x() - (gridPieceSize * pAtIndex.movementSpeed);
		if (nextX > gridBoundLeft)
		{
			pItem->setPos(nextX, pItem->y());
		}
		else
		{
			pItem->setPos(gridBoundLeft + (gridPieceSize / 4), pItem->y());
		}
		break;
	case (tokenPatroller::Facing::RIGHT):
		nextY = pItem->y();
		nextX = pItem->x() + (gridPieceSize * pAtIndex.movementSpeed);
		if (nextX < gridBoundRight)
		{
			pItem->setPos(nextX, pItem->y());
		}
		else
		{
			pItem->setPos(gridBoundRight - gridPieceSize + (gridPieceSize / 4), pItem->y());
		}
		break;
	}

	nextY = pItem->y();
	nextX = pItem->x();

	if (hitImmobileObjectAndDelete(levelsAll[levelCurrent].blocks, nextY, nextX))
	{
		return;
	}
	else if (hitImmobileObject(levelsAll[levelCurrent].gates, nextY, nextX))
	{
		if (pAtIndex.heldKeys > 0)
		{
			hitImmobileObjectAndDelete(levelsAll[levelCurrent].gates, nextY, nextX);
			levelsAll[levelCurrent].players[pIndex].heldKeys--;
			uiGameplayUpdateStatCounter(StatCounterType::KEYS_HELD);
			return;
		}
		else
		{
			uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesKeyNeeded);
			return;
		}
	}
	else if (hitImmobileObjectAndDelete(levelsAll[levelCurrent].keys, nextY, nextX))
	{
		levelsAll[levelCurrent].players[pIndex].heldKeys++;
		uiGameplayUpdateStatCounter(StatCounterType::KEYS_HELD);
		uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesKeyObtained);
		return;
	}
	else
	{
		return;
	}
}

bool GameplayScreen::patrollerHitTrapPush(const std::vector<tokenPatroller>& patrollers, const int enemyNum, const int patrollerNextY, const int patrollerNextX)
{
	const int tokens = levelsAll[levelCurrent].utils.size();
	for (int i = 0; i < tokens; i++)
	{
		if (patrollerNextY == levelsAll[levelCurrent].utils[i].item.get()->y()
			&& patrollerNextX == levelsAll[levelCurrent].utils[i].item.get()->x())
		{
			if (levelsAll[levelCurrent].utils[i].type == tokenUtil::Type::PUSHER)
			{
				if (levelsAll[levelCurrent].utils[i].stateModified == tokenUtil::State::ACTIVE)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void GameplayScreen::knockbackHitTrap(std::vector<tokenPatroller>& patrollers, const int enemyNum)
{
	// Logistics of this feature:
	// Follow the same logic as player knockback (e.g. check for knockback hit BEFORE enemy is moved)
	// If there's a collision with a knockback trap, knockback patroller one square (do this twice)

	// If patroller is moving into knockback trap, we knockback patroller in the opposite direction

	const auto& eItem = patrollers[enemyNum].item.get();

	int nextY;
	int nextX;

	switch (patrollers[enemyNum].facing)
	{
	case (tokenPatroller::Facing::UP):
		nextY = eItem->y() + (gridPieceSize * patrollers[enemyNum].movementSpeed);
		nextX = eItem->x();
		if (nextY < gridBoundDown)
		{
			eItem->setPos(eItem->x(), nextY);
		}
		else
		{
			eItem->setPos(eItem->x(), gridBoundDown - gridPieceSize + (gridPieceSize / 4));
		}
		break;
	case (tokenPatroller::Facing::DOWN):
		nextY = eItem->y() - (gridPieceSize * patrollers[enemyNum].movementSpeed);
		nextX = eItem->x();
		if (nextY > gridBoundUp)
		{
			eItem->setPos(eItem->x(), nextY);
		}
		else
		{
			eItem->setPos(eItem->x(), gridBoundUp + (gridPieceSize / 4));
		}
		break;
	case (tokenPatroller::Facing::LEFT):
		nextY = eItem->y();
		nextX = eItem->x() + (gridPieceSize * patrollers[enemyNum].movementSpeed);
		if (nextX < gridBoundRight)
		{
			eItem->setPos(nextX, eItem->y());
		}
		else
		{
			eItem->setPos(gridBoundRight - gridPieceSize + (gridPieceSize / 4), eItem->y());
		}
		break;
	case (tokenPatroller::Facing::RIGHT):
		nextY = eItem->y();
		nextX = eItem->x() - (gridPieceSize * patrollers[enemyNum].movementSpeed);
		if (nextX > gridBoundLeft)
		{
			eItem->setPos(nextX, eItem->y());
		}
		else
		{
			eItem->setPos(gridBoundLeft + (gridPieceSize / 4), eItem->y());
		}
		break;
	}

	nextY = eItem->y();
	nextX = eItem->x();

	if (hitPlayer(patrollers, enemyNum))
	{
		if (patrollers[enemyNum].type == tokenPatroller::Type::PUSHER)
		{
			for (int i = 0; i < levelsAll[levelCurrent].players[pIndex].knockbackAmount; i++)
			{
				knockbackPlayerMoving();
			}
		}
	}
}

bool GameplayScreen::hitPlayer(const std::vector<tokenPatroller>& patrollers, const int enemyNum)
{
	if (patrollers[enemyNum].item.get()->y() == levelsAll[levelCurrent].players[pIndex].item.get()->y()
		&& patrollers[enemyNum].item.get()->x() == levelsAll[levelCurrent].players[pIndex].item.get()->x())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GameplayScreen::suckInHitTrap(std::vector<tokenPatroller>& patrollers, const int enemyNum)
{
	const auto& eItem = patrollers[enemyNum].item.get();

	const int suckRange = 2;
	const int numSuckers = levelsAll[levelCurrent].utils.size();
	for (int i = 0; i < numSuckers; i++)
	{
		const auto& uAtIndex = levelsAll[levelCurrent].utils[i];

		if (uAtIndex.type == tokenUtil::Type::SUCKER
			&& uAtIndex.stateModified == tokenUtil::State::ACTIVE)
		{
			if (eItem->y() == uAtIndex.item.get()->y())
			{
				if (eItem->x() < uAtIndex.item.get()->x())
				{
					// player is to the LEFT of sucker
					if (abs(uAtIndex.item.get()->x() - eItem->x()) / gridPieceSize == suckRange)
					{
						// move player one to the RIGHT
						eItem->setPos(eItem->x() + patrollers[enemyNum].movementSpeed * gridPieceSize, eItem->y());
						suckInHitCheck();
						return;
					}
				}
				else if (eItem->x() > uAtIndex.item.get()->x())
				{
					// player is to the RIGHT of sucker
					if (abs(uAtIndex.item.get()->x() - eItem->x()) / gridPieceSize == suckRange)
					{
						// move player one to the LEFT
						eItem->setPos(eItem->x() - patrollers[enemyNum].movementSpeed * gridPieceSize, eItem->y());
						suckInHitCheck();
						return;
					}
				}
			}
			else if (eItem->x() == uAtIndex.item.get()->x())
			{
				if (eItem->y() < uAtIndex.item.get()->y())
				{
					// player is UP of sucker
					if (abs(uAtIndex.item.get()->y() - eItem->y()) / gridPieceSize == suckRange)
					{
						// move player one DOWN
						eItem->setPos(eItem->x(), eItem->y() + patrollers[enemyNum].movementSpeed * gridPieceSize);
						suckInHitCheck();
						return;
					}
				}
				else if (eItem->y() > uAtIndex.item.get()->y())
				{
					// player is DOWN of sucker
					if (abs(uAtIndex.item.get()->y() - eItem->y()) / gridPieceSize == suckRange)
					{
						// move player one UP
						eItem->setPos(eItem->x(), eItem->y() - patrollers[enemyNum].movementSpeed * gridPieceSize);
						suckInHitCheck();
						return;
					}
				}
			}
		}
	}
}

void GameplayScreen::suckInHitCheck()
{
	const int nextY = levelsAll[levelCurrent].players[pIndex].item.get()->y();
	const int nextX = levelsAll[levelCurrent].players[pIndex].item.get()->x();

	if (hitEnemy(levelsAll[levelCurrent].pushers, nextY, nextX)
		|| hitEnemy(levelsAll[levelCurrent].suckers, nextY, nextX))
	{
		return;
	}
	else if (hitImmobileObjectAndDelete(levelsAll[levelCurrent].blocks, nextY, nextX))
	{
		return;
	}
	else if (hitImmobileObject(levelsAll[levelCurrent].gates, nextY, nextX))
	{
		if (levelsAll[levelCurrent].players[pIndex].heldKeys > 0)
		{
			hitImmobileObjectAndDelete(levelsAll[levelCurrent].gates, nextY, nextX);
			levelsAll[levelCurrent].players[pIndex].heldKeys--;
			uiGameplayUpdateStatCounter(StatCounterType::KEYS_HELD);
			return;
		}
		else
		{
			uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesKeyNeeded);
			return;
		}
	}
	else if (hitImmobileObjectAndDelete(levelsAll[levelCurrent].keys, nextY, nextX))
	{
		levelsAll[levelCurrent].players[pIndex].heldKeys++;
		uiGameplayUpdateStatCounter(StatCounterType::KEYS_HELD);
		uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesKeyObtained);
		return;
	}
}

void GameplayScreen::suckInRange()
{
	const auto& pItem = levelsAll[levelCurrent].players[pIndex].item.get();
	const auto& pAtIndex = levelsAll[levelCurrent].players[pIndex];

	const int suckRange = 2;
	const int numSuckers = levelsAll[levelCurrent].suckers.size();
	for (int i = 0; i < numSuckers; i++)
	{
		const auto& sItem = levelsAll[levelCurrent].suckers[i].item.get();

		if (pItem->y() == sItem->y())
		{
			if (pItem->x() < sItem->x())
			{
				// player is to the LEFT of sucker
				if (abs(sItem->x() - pItem->x()) / gridPieceSize == suckRange)
				{
					// move player one to the RIGHT
					pItem->setPos(pItem->x() + pAtIndex.movementSpeed * gridPieceSize, pItem->y());
					suckInHitCheck();
					hazardHitCheck();
					teleportHitCheck();
					return;
				}
			}
			else if (pItem->x() > sItem->x())
			{
				// player is to the RIGHT of sucker
				if (abs(sItem->x() - pItem->x()) / gridPieceSize == suckRange)
				{
					// move player one to the LEFT
					pItem->setPos(pItem->x() - pAtIndex.movementSpeed * gridPieceSize, pItem->y());
					suckInHitCheck();
					hazardHitCheck();
					teleportHitCheck();
					return;
				}
			}
		}
		else if (pItem->x() == sItem->x())
		{
			if (pItem->y() < sItem->y())
			{
				// player is UP of sucker
				if (abs(sItem->y() - pItem->y()) / gridPieceSize == suckRange)
				{
					// move player one DOWN
					pItem->setPos(pItem->x(), pItem->y() + pAtIndex.movementSpeed * gridPieceSize);
					suckInHitCheck();
					hazardHitCheck();
					teleportHitCheck();
					return;
				}
			}
			else if (pItem->y() > sItem->y())
			{
				// player is DOWN of sucker
				if (abs(sItem->y() - pItem->y()) / gridPieceSize == suckRange)
				{
					// move player one UP
					pItem->setPos(pItem->x(), pItem->y() - pAtIndex.movementSpeed * gridPieceSize);
					suckInHitCheck();
					hazardHitCheck();
					teleportHitCheck();
					return;
				}
			}
		}
	}
}

void GameplayScreen::hazardHitCheck()
{
	// Hazards are instant level-ending penalty if you land on them.
	// This gives a reason in design for player to be more careful about how far they are getting pushed
	// by pushers. Pushed two squares could get them past a hazard. Pushed one square right into it.
	// (expands on what kind of challenges you can present in level design, in other words)
	for (const auto& hazard : levelsAll[levelCurrent].hazards)
	{
		if (levelsAll[levelCurrent].players[pIndex].item.get()->x() == hazard.item.get()->x() &&
			levelsAll[levelCurrent].players[pIndex].item.get()->y() == hazard.item.get()->y())
		{
			levelsAll[levelCurrent].turnsRemaining = 0;
			return;
		}
	}
}

void GameplayScreen::teleportHitCheck()
{
	// There should only ever be two teleports on the grid.
	// With this expected, when player lands on a teleport square,
	// we can move player to whichever teleport square they are NOT on.
	// This enables the possibility in design of getting the player across the board a little faster,
	// rather than always having to walk long distances.
	for (int i = 0; i < levelsAll[levelCurrent].teleports.size(); i++)
	{
		if (levelsAll[levelCurrent].players[pIndex].item.get()->x() == levelsAll[levelCurrent].teleports[i].item.get()->x() &&
			levelsAll[levelCurrent].players[pIndex].item.get()->y() == levelsAll[levelCurrent].teleports[i].item.get()->y())
		{
			if (i == 0)
			{
				levelsAll[levelCurrent].players[pIndex].item.get()->setPos
				(
					levelsAll[levelCurrent].teleports[i + 1].item.get()->x(),
					levelsAll[levelCurrent].teleports[i + 1].item.get()->y()
				);
			}
			else if (i == 1)
			{
				levelsAll[levelCurrent].players[pIndex].item.get()->setPos
				(
					levelsAll[levelCurrent].teleports[i - 1].item.get()->x(),
					levelsAll[levelCurrent].teleports[i - 1].item.get()->y()
				);
			}
			uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesTeleport);
			return;
		}
	}
}

void GameplayScreen::levelSetFailed()
{
	uiGameplayGroup->setVisible(false);
	gameState = GameState::LEVEL_FAILED;
	splashItem.get()->setPixmap(imgMap.at("imgSplashLevelFailed"));
	scene.get()->addItem(splashItem.get());
}

void GameplayScreen::levelSetToDefaults(levelData& level)
{
	levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex.clear();
	levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex.clear();
	levelsAll[levelCurrent].turnsRemaining = levelsAll[levelCurrent].turnsInitial;
	levelsAll[levelCurrent].state = levelData::State::UNTOUCHED;
	for (auto& key : level.keys)
	{
		key.state = tokenImmobile::State::ACTIVE;
		key.item.get()->setPixmap
		(
			stateToImg(key.state, key.type)
		);
		key.item.get()->setPos(key.initialX, key.initialY);
		key.item.get()->setZValue(tokenImmobileZ);
		qDebug() << "key: " << key.initialX;
		qDebug() << "key: " << key.initialY;
	}
	for (auto& gate : level.gates)
	{
		gate.state = tokenImmobile::State::ACTIVE;
		gate.item.get()->setPixmap
		(
			stateToImg(gate.state, gate.type)
		);
		gate.item.get()->setPos(gate.initialX, gate.initialY);
		gate.item.get()->setZValue(tokenImmobileZ);
		qDebug() << "gate: " << gate.initialX;
		qDebug() << "gate: " << gate.initialY;
	}
	for (auto& player : level.players)
	{
		player.heldKeys = 0;
		player.item.get()->setPixmap
		(
			facingToImg(player.facing)
		);
		player.item.get()->setPos(player.initialX, player.initialY);
		player.item.get()->setZValue(tokenMobileZ);
		qDebug() << "player: " << player.initialX;
		qDebug() << "player: " << player.initialY;
	}
	for (auto& pusher : level.pushers)
	{
		pusher.facing = pusher.facingInitial;
		pusher.item.get()->setPixmap
		(
			facingToImg(pusher.facing, pusher.type)
		);
		pusher.item.get()->setPos(pusher.initialX, pusher.initialY);
		pusher.item.get()->setZValue(tokenMobileZ);
		qDebug() << "pusher: " << pusher.initialX;
		qDebug() << "pusher: " << pusher.initialY;
	}
	for (auto& sucker : level.suckers)
	{
		sucker.facing = sucker.facingInitial;
		sucker.item.get()->setPixmap
		(
			facingToImg(sucker.facing, sucker.type)
		);
		sucker.item.get()->setPos(sucker.initialX, sucker.initialY);
		sucker.item.get()->setZValue(tokenMobileZ);
		qDebug() << "sucker: " << sucker.initialX;
		qDebug() << "sucker: " << sucker.initialY;
	}
	for (auto& util : level.utils)
	{
		util.stateModified = util.stateBase;
		util.item.get()->setPixmap
		(
			stateToImg(util.stateModified, util.type)
		);
		util.item.get()->setPos(util.initialX, util.initialY);
		util.item.get()->setZValue(tokenImmobileZ);
		qDebug() << "util: " << util.initialX;
		qDebug() << "util: " << util.initialY;
	}
	for (auto& block : level.blocks)
	{
		block.state = tokenImmobile::State::ACTIVE;
		block.item.get()->setPixmap
		(
			stateToImg(block.state, block.type)
		);
		block.item.get()->setPos(block.initialX, block.initialY);
		block.item.get()->setZValue(tokenImmobileZ);
		qDebug() << block.initialX;
		qDebug() << block.initialY;
	}
	for (auto& hazard : level.hazards)
	{
		hazard.state = tokenImmobile::State::ACTIVE;
		hazard.item.get()->setPixmap
		(
			stateToImg(hazard.state, hazard.type)
		);
		hazard.item.get()->setPos(hazard.initialX, hazard.initialY);
		hazard.item.get()->setZValue(tokenImmobileZ);
		qDebug() << hazard.initialX;
		qDebug() << hazard.initialY;
	}
	for (auto& teleport : level.teleports)
	{
		teleport.state = tokenImmobile::State::ACTIVE;
		teleport.item.get()->setPixmap
		(
			stateToImg(teleport.state, teleport.type)
		);
		teleport.item.get()->setPos(teleport.initialX, teleport.initialY);
		teleport.item.get()->setZValue(tokenImmobileZ);
		qDebug() << teleport.initialX;
		qDebug() << teleport.initialY;
	}
}

void GameplayScreen::levelSetComplete()
{
	uiGameplayGroup->setVisible(false);
	levelsComplete++;
	levelsRemaining--;
	levelsAll[levelCurrent].state = levelData::State::COMPLETE;

	if (levelsRemaining > 0)
	{
		gameState = GameState::LEVEL_COMPLETE;
		splashItem.get()->setPixmap(imgMap.at("imgSplashLevelComplete"));
	}
	else
	{
		gameState = GameState::LEVEL_ALL_DONE;
		splashItem.get()->setPixmap(imgMap.at("imgSplashLevelAllDone"));
	}
	scene.get()->addItem(splashItem.get());
}

int GameplayScreen::levelLoadValidateId(QFile &file)
{
	QTextStream qStream(&file);
	QString line = qStream.readLine();
	int levelPos = -1;
	if (line.contains("::Id="))
	{
		QString idStr = extractSubstringInbetweenQt("::Id=", "::", line);
		return levelFoundInListAtPos(idStr);
	}
	return levelPos;
}

int GameplayScreen::levelFoundInListAtPos(const QString &id)
{
	const int levelsAllSize = levelsAll.size();
	for (int i = 0; i < levelsAllSize; i++)
	{
		if (levelsAll[i].id == id)
			return i;
	}
	return -1;
}

int GameplayScreen::levelFoundInList(const QString &id)
{
	for (const auto& level : levelsAll)
	{
		if (level.id == id)
			return true;
	}
	return false;
}

bool GameplayScreen::allGatesOpened()
{
	for (const auto& gate : levelsAll[levelCurrent].gates)
	{
		if (gate.state != tokenImmobile::State::INVISIBLE)
			return false;
	}
	return true;
}

void GameplayScreen::addCurrentLevelToScene()
{
	for (const auto& key : levelsAll[levelCurrent].keys)
	{
		scene.get()->addItem(key.item.get());
	}
	for (const auto& gate : levelsAll[levelCurrent].gates)
	{
		scene.get()->addItem(gate.item.get());
	}
	for (const auto& player : levelsAll[levelCurrent].players)
	{
		scene.get()->addItem(player.item.get());
	}
	for (const auto& pusher : levelsAll[levelCurrent].pushers)
	{
		scene.get()->addItem(pusher.item.get());
	}
	for (const auto& sucker : levelsAll[levelCurrent].suckers)
	{
		scene.get()->addItem(sucker.item.get());
	}
	for (const auto& util : levelsAll[levelCurrent].utils)
	{
		scene.get()->addItem(util.item.get());
	}
	for (const auto& block : levelsAll[levelCurrent].blocks)
	{
		scene.get()->addItem(block.item.get());
	}
	for (const auto& hazard : levelsAll[levelCurrent].hazards)
	{
		scene.get()->addItem(hazard.item.get());
	}
	for (const auto& teleport : levelsAll[levelCurrent].teleports)
	{
		scene.get()->addItem(teleport.item.get());
	}
}

void GameplayScreen::removeCurrentLevelFromScene()
{
	// We remove rather than using QGraphicsScene clear() function,
	// to avoid deleting the items (they need to be reusable).
	for (const auto& key : levelsAll[levelCurrent].keys)
	{
		scene.get()->removeItem(key.item.get());
	}
	for (const auto& gate : levelsAll[levelCurrent].gates)
	{
		scene.get()->removeItem(gate.item.get());
	}
	for (const auto& player : levelsAll[levelCurrent].players)
	{
		scene.get()->removeItem(player.item.get());
	}
	for (const auto& pusher : levelsAll[levelCurrent].pushers)
	{
		scene.get()->removeItem(pusher.item.get());
	}
	for (const auto& sucker : levelsAll[levelCurrent].suckers)
	{
		scene.get()->removeItem(sucker.item.get());
	}
	for (const auto& util : levelsAll[levelCurrent].utils)
	{
		scene.get()->removeItem(util.item.get());
	}
	for (const auto& block : levelsAll[levelCurrent].blocks)
	{
		scene.get()->removeItem(block.item.get());
	}
	for (const auto& hazard : levelsAll[levelCurrent].hazards)
	{
		scene.get()->removeItem(hazard.item.get());
	}
	for (const auto& teleport : levelsAll[levelCurrent].teleports)
	{
		scene.get()->removeItem(teleport.item.get());
	}
}

void GameplayScreen::uiGameplaySetToDefaults()
{
	for (auto& entry : statCounterMap)
		uiGameplayUpdateStatCounter(entry.first);
	uiGameplayMessagesTextBox.get()->setText("");
}

void GameplayScreen::uiGameplayUpdateStatCounter(const StatCounterType &statCounterType)
{
	switch (statCounterType)
	{
	case StatCounterType::TURNS_REMAINING:
		statCounterMap.at(statCounterType).updateCounter(levelsAll[levelCurrent].turnsRemaining);
		break;
	case StatCounterType::KEYS_HELD:
		statCounterMap.at(statCounterType).updateCounter(levelsAll[levelCurrent].players[pIndex].heldKeys);
		break;
	case StatCounterType::TRAPS_PUSHERS:
		statCounterMap.at(statCounterType).updateCounter(levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex.size());
		break;
	case StatCounterType::TRAPS_SUCKERS:
		statCounterMap.at(statCounterType).updateCounter(levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex.size());
		break;
	}
}

void GameplayScreen::uiMenuBtnClickResume()
{
	if (gameState == GameState::PAUSED)
	{
		uiMenuResumePlay();
	}
}

void GameplayScreen::uiMenuBtnClickSave()
{
	if (gameState == GameState::PAUSED)
	{
		// Id is important here. We use unique Id in save/load process,
		// to ensure that we're loading in the correct level with the right amount of progress.

		QString proposedSaveName;
		proposedSaveName += fileDirLastSaved + "/";
		proposedSaveName += QDateTime::currentDateTime().toString("_yyyy_MM_dd_HH_mm_ss");
		QFileDialog dialog(this, tr("Save As"), proposedSaveName, tr("Moxybox Files (*.MoxySave)"));
		dialog.setWindowModality(Qt::WindowModal);
		dialog.setAcceptMode(QFileDialog::AcceptSave);
		if (dialog.exec() == QFileDialog::Accepted)
		{
			QString fpath = dialog.selectedFiles().first();
			QFile fileWrite(fpath);
			if (fileWrite.open(QIODevice::WriteOnly))
			{
				QTextStream qStream(&fileWrite);
				qStream <<
					"::"
					"Id=" + levelsAll[levelCurrent].id +
					"::" +
					"Difficulty=" + QString::number(levelsAll[levelCurrent].difficulty) +
					"::" +
					"TurnsRemaining=" + QString::number(levelsAll[levelCurrent].turnsRemaining) +
					"::"
					"KeysHeld=" + QString::number(levelsAll[levelCurrent].players[pIndex].heldKeys) +
					"::"
					"TrapsPusherHeldNum=" + QString::number(levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex.size()) +
					"::"
					"TrapsSuckerHeldNum=" + QString::number(levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex.size()) +
					"::";

				qStream << "TrapsPusherHeldIndices=";
				for (const auto& heldIndex : levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex)
				{
					qStream << "(" + QString::number(heldIndex) + ")";
				}
				qStream << "::";

				qStream << "TrapsSuckerHeldIndices=";
				for (const auto& heldIndex : levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex)
				{
					qStream << "(" + QString::number(heldIndex) + ")";
				}
				qStream << "::";

				qStream << "\r\n";

				qStream << "::Gate=";
				for (const auto& gate : levelsAll[levelCurrent].gates)
				{
					qStream <<
						"(" +
						QString::number(gate.item.get()->x()) +
						"," +
						QString::number(gate.item.get()->y()) +
						"," +
						tokenImmobile::stateToString(gate.state) +
						")";
				}
				qStream << "::\r\n";

				qStream << "::Key=";
				for (const auto& key : levelsAll[levelCurrent].keys)
				{
					qStream <<
						"(" +
						QString::number(key.item.get()->x()) +
						"," +
						QString::number(key.item.get()->y()) +
						"," +
						tokenImmobile::stateToString(key.state) +
						")";
				}
				qStream << "::\r\n";

				qStream << "::Player=";
				for (const auto& player : levelsAll[levelCurrent].players)
				{
					qStream <<
						QString::number(player.item.get()->x()) +
						"," +
						QString::number(player.item.get()->y());
				}
				qStream << "::\r\n";

				qStream << "::Pusher=";
				for (auto& pusher : levelsAll[levelCurrent].pushers)
				{
					qStream <<
						"(" +
						QString::number(pusher.item.get()->x()) +
						"," +
						QString::number(pusher.item.get()->y()) +
						"," +
						tokenPatroller::facingToString(pusher.facing) +
						")";
				}
				qStream << "::\r\n";

				qStream << "::Sucker=";
				for (auto& sucker : levelsAll[levelCurrent].suckers)
				{
					qStream <<
						"(" +
						QString::number(sucker.item.get()->x()) +
						"," +
						QString::number(sucker.item.get()->y()) +
						"," +
						tokenPatroller::facingToString(sucker.facing) +
						")";
				}
				qStream << "::\r\n";

				qStream << "::Util=";
				for (auto& util : levelsAll[levelCurrent].utils)
				{
					qStream <<
						"(" +
						QString::number(util.item.get()->x()) +
						"," +
						QString::number(util.item.get()->y()) +
						"," +
						tokenUtil::typeToString(util.type) +
						"," +
						tokenUtil::stateToString(util.stateModified) +
						")";
				}
				qStream << "::\r\n";

				qStream << "::Block=";
				for (auto& block : levelsAll[levelCurrent].blocks)
				{
					qStream <<
						"(" +
						QString::number(block.item.get()->x()) +
						"," +
						QString::number(block.item.get()->y()) +
						"," +
						tokenImmobile::stateToString(block.state) +
						")";
				}
				qStream << "::\r\n";

				qStream << "::Hazard=";
				for (auto& hazard : levelsAll[levelCurrent].hazards)
				{
					qStream <<
						"(" +
						QString::number(hazard.item.get()->x()) +
						"," +
						QString::number(hazard.item.get()->y()) +
						"," +
						tokenImmobile::stateToString(hazard.state) +
						")";
				}
				qStream << "::\r\n";

				qStream << "::Teleport=";
				for (auto& teleport : levelsAll[levelCurrent].teleports)
				{
					qStream <<
						"(" +
						QString::number(teleport.item.get()->x()) +
						"," +
						QString::number(teleport.item.get()->y()) +
						"," +
						tokenImmobile::stateToString(teleport.state) +
						")";
				}
				qStream << "::\r\n";

				qStream << "::LevelsComplete=";
				for (const auto& level : levelsAll)
				{
					if (level.id != levelsAll[levelCurrent].id)
					{
						// If a level is complete, store its ID, so we can set it as complete on load
						if (level.state == levelData::State::COMPLETE)
						{
							qStream <<
								"(" +
								level.id +
								")";
						}
					}
				}
				qStream << "::\r\n";

				fileWrite.close();
				fileDirLastSaved = QFileInfo(fpath).path();
			}

			uiMenuResumePlay();
		}
	}
}

void GameplayScreen::uiMenuBtnClickLoad()
{
	if (gameState == GameState::PAUSED)
	{
		// reverse the saving process with magical powers of deduction
		QString filename = QFileDialog::getOpenFileName(this, tr("Open"), fileDirLastOpened, tr("Moxybox Files (*.MoxySave)"));
		if (!filename.isEmpty())
		{
			fileDirLastOpened = QFileInfo(filename).path();
			QFile fileRead(filename);
			int levelPos = -1;
			if (fileRead.open(QIODevice::ReadOnly))
			{
				levelPos = levelLoadValidateId(fileRead);
				fileRead.close();
			}

			if (levelPos >= 0)
			{
				if (fileRead.open(QIODevice::ReadOnly))
				{
					if (levelCurrent != levelPos)
					{
						removeCurrentLevelFromScene();
						levelCurrent = levelPos;
						qDebug() << "Current level ID: " << levelsAll[levelCurrent].id;
						addCurrentLevelToScene();
					}

					qDebug() << "Loaded level ID: " << levelsAll[levelPos].id;
					qDebug() << "Current level ID: " << levelsAll[levelCurrent].id;
					QTextStream qStream(&fileRead);
					while (!qStream.atEnd())
					{
						QString line = qStream.readLine();
						if (line.contains("::Id="))
						{
							levelsAll[levelCurrent].turnsRemaining = extractSubstringInbetweenQt("::TurnsRemaining=", "::", line).toInt();
							levelsAll[levelCurrent].difficulty = extractSubstringInbetweenQt("::Difficulty=", "::", line).toInt();
							levelsAll[levelCurrent].players[pIndex].heldKeys = extractSubstringInbetweenQt("::KeysHeld=", "::", line).toInt();

							levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex.clear();
							levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex.clear();
							levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex.resize(extractSubstringInbetweenQt("::TrapsPusherHeldNum=", "::", line).toInt());
							levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex.resize(extractSubstringInbetweenQt("::TrapsSuckerHeldNum=", "::", line).toInt());

							QString utilPushLine = extractSubstringInbetweenQt("::TrapsPusherHeldIndices=", "::", line);
							QStringList utilPushList = extractSubstringInbetweenQtLoopList("(", ")", utilPushLine);

							for (int i = 0; i < levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex.size(); i++)
							{
								levelsAll[levelCurrent].players[pIndex].heldUtilPushIndex[i] = utilPushList[i].toInt();
							}

							QString utilSuckLine = extractSubstringInbetweenQt("::TrapsSuckerHeldIndices=", "::", line);
							QStringList utilSuckList = extractSubstringInbetweenQtLoopList("(", ")", utilSuckLine);

							for (int i = 0; i < levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex.size(); i++)
							{
								levelsAll[levelCurrent].players[pIndex].heldUtilSuckIndex[i] = utilSuckList[i].toInt();
							}

							for (auto& entry : statCounterMap)
								uiGameplayUpdateStatCounter(entry.first);
						}
						else if (line.contains("::Gate="))
						{
							QString dataLine = extractSubstringInbetweenQt("::Gate=", "::", line);
							QStringList dataList = extractSubstringInbetweenQtLoopList("(", ")", dataLine);

							for (int i = 0; i < dataList.length(); i++)
							{
								QStringList components = dataList[i].split(",", QString::SkipEmptyParts);
								levelsAll[levelCurrent].gates[i].item.get()->setPos
								(
									components[0].toInt(),
									components[1].toInt()
								);
								levelsAll[levelCurrent].gates[i].state = tokenImmobile::stateToEnum(components[2]);
								levelsAll[levelCurrent].gates[i].item.get()->setPixmap
								(
									stateToImg(levelsAll[levelCurrent].gates[i].state, levelsAll[levelCurrent].gates[i].type)
								);
							}
						}
						else if (line.contains("::Key="))
						{
							QString dataLine = extractSubstringInbetweenQt("::Key=", "::", line);
							QStringList dataList = extractSubstringInbetweenQtLoopList("(", ")", dataLine);

							for (int i = 0; i < dataList.length(); i++)
							{
								QStringList components = dataList[i].split(",", QString::SkipEmptyParts);
								levelsAll[levelCurrent].keys[i].item.get()->setPos
								(
									components[0].toInt(),
									components[1].toInt()
								);
								levelsAll[levelCurrent].keys[i].state = tokenImmobile::stateToEnum(components[2]);
								levelsAll[levelCurrent].keys[i].item.get()->setPixmap
								(
									stateToImg(levelsAll[levelCurrent].keys[i].state, levelsAll[levelCurrent].keys[i].type)
								);
							}
						}
						else if (line.contains("::Player="))
						{
							QStringList components = extractSubstringInbetweenQt("::Player=", "::", line).split(",", QString::SkipEmptyParts);
							levelsAll[levelCurrent].players[pIndex].item.get()->setPos
							(
								components[0].toInt(),
								components[1].toInt()
							);
						}
						else if (line.contains("::Pusher="))
						{
							QString dataLine = extractSubstringInbetweenQt("::Pusher=", "::", line);
							QStringList dataList = extractSubstringInbetweenQtLoopList("(", ")", dataLine);

							for (int i = 0; i < dataList.length(); i++)
							{
								QStringList components = dataList[i].split(",", QString::SkipEmptyParts);
								levelsAll[levelCurrent].pushers[i].item.get()->setPos
								(
									components[0].toInt(),
									components[1].toInt()
								);
								levelsAll[levelCurrent].pushers[i].facing = tokenPatroller::facingToEnum(components[2]);
								levelsAll[levelCurrent].pushers[i].item.get()->setPixmap
								(
									facingToImg(levelsAll[levelCurrent].pushers[i].facing, levelsAll[levelCurrent].pushers[i].type)
								);
							}
						}
						else if (line.contains("::Sucker="))
						{
							QString dataLine = extractSubstringInbetweenQt("::Sucker=", "::", line);
							QStringList dataList = extractSubstringInbetweenQtLoopList("(", ")", dataLine);

							for (int i = 0; i < dataList.length(); i++)
							{
								QStringList components = dataList[i].split(",", QString::SkipEmptyParts);
								levelsAll[levelCurrent].suckers[i].item.get()->setPos
								(
									components[0].toInt(),
									components[1].toInt()
								);
								levelsAll[levelCurrent].suckers[i].facing = tokenPatroller::facingToEnum(components[2]);
								levelsAll[levelCurrent].suckers[i].item.get()->setPixmap
								(
									facingToImg(levelsAll[levelCurrent].suckers[i].facing, levelsAll[levelCurrent].suckers[i].type)
								);
							}
						}
						else if (line.contains("::Util="))
						{
							QString dataLine = extractSubstringInbetweenQt("::Util=", "::", line);
							QStringList dataList = extractSubstringInbetweenQtLoopList("(", ")", dataLine);

							for (int i = 0; i < dataList.length(); i++)
							{
								QStringList components = dataList[i].split(",", QString::SkipEmptyParts);
								levelsAll[levelCurrent].utils[i].item.get()->setPos
								(
									components[0].toInt(),
									components[1].toInt()
								);
								levelsAll[levelCurrent].utils[i].type = levelsAll[levelCurrent].utils[i].typeToEnum(components[2]);
								levelsAll[levelCurrent].utils[i].stateModified = tokenUtil::stateToEnum(components[3]);
								levelsAll[levelCurrent].utils[i].item.get()->setPixmap
								(
									stateToImg(levelsAll[levelCurrent].utils[i].stateModified, levelsAll[levelCurrent].utils[i].type)
								);
							}
						}
						else if (line.contains("::Block="))
						{
							QString dataLine = extractSubstringInbetweenQt("::Block=", "::", line);
							QStringList dataList = extractSubstringInbetweenQtLoopList("(", ")", dataLine);

							for (int i = 0; i < dataList.length(); i++)
							{
								QStringList components = dataList[i].split(",", QString::SkipEmptyParts);
								levelsAll[levelCurrent].blocks[i].item.get()->setPos
								(
									components[0].toInt(),
									components[1].toInt()
								);
								levelsAll[levelCurrent].blocks[i].state = tokenImmobile::stateToEnum(components[2]);
								levelsAll[levelCurrent].blocks[i].item.get()->setPixmap
								(
									stateToImg(levelsAll[levelCurrent].blocks[i].state, levelsAll[levelCurrent].blocks[i].type)
								);
							}
						}
						else if (line.contains("::Hazard="))
						{
							QString dataLine = extractSubstringInbetweenQt("::Hazard=", "::", line);
							QStringList dataList = extractSubstringInbetweenQtLoopList("(", ")", dataLine);

							for (int i = 0; i < dataList.length(); i++)
							{
								QStringList components = dataList[i].split(",", QString::SkipEmptyParts);
								levelsAll[levelCurrent].hazards[i].item.get()->setPos
								(
									components[0].toInt(),
									components[1].toInt()
								);
								levelsAll[levelCurrent].hazards[i].state = tokenImmobile::stateToEnum(components[2]);
								levelsAll[levelCurrent].hazards[i].item.get()->setPixmap
								(
									stateToImg(levelsAll[levelCurrent].hazards[i].state, levelsAll[levelCurrent].hazards[i].type)
								);
							}
						}
						else if (line.contains("::Teleport="))
						{
							QString dataLine = extractSubstringInbetweenQt("::Teleport=", "::", line);
							QStringList dataList = extractSubstringInbetweenQtLoopList("(", ")", dataLine);

							for (int i = 0; i < dataList.length(); i++)
							{
								QStringList components = dataList[i].split(",", QString::SkipEmptyParts);
								levelsAll[levelCurrent].teleports[i].item.get()->setPos
								(
									components[0].toInt(),
									components[1].toInt()
								);
								levelsAll[levelCurrent].teleports[i].state = tokenImmobile::stateToEnum(components[2]);
								levelsAll[levelCurrent].teleports[i].item.get()->setPixmap
								(
									stateToImg(levelsAll[levelCurrent].teleports[i].state, levelsAll[levelCurrent].teleports[i].type)
								);
							}
						}
						else if (line.contains("::LevelsComplete="))
						{
							QString dataLine = extractSubstringInbetweenQt("::LevelsComplete=", "::", line);
							QStringList dataList = extractSubstringInbetweenQtLoopList("(", ")", dataLine);

							for (int i = 0; i < dataList.length(); i++)
							{
								int levelPos = levelFoundInListAtPos(dataList[i]);
								if (levelPos >= 0)
								{
									levelsAll[levelPos].state = levelData::State::COMPLETE;
								}
							}
						}
					}
					fileRead.close();
					uiMenuResumePlay();
				}
			}
			else
			{
				QMessageBox qMsg(this->parentWidget());
				qMsg.setStyleSheet(styleMap.at("uiMessageBoxStyle"));
				qMsg.setWindowTitle("Level Not Found");
				qMsg.setText("Saved level ID was not found in loaded levels.\r\nSave file only saves references to existing levels, so if they are moved or deleted, loading may fail.");
				qMsg.setStandardButtons(QMessageBox::Ok);
				qMsg.setDefaultButton(QMessageBox::Ok);
				qMsg.setFont(uiGameplayFontTextBox);
				qMsg.button(QMessageBox::Ok)->setFont(uiGameplayFontTextBox);
				qMsg.exec();
			}
		}
	}
}

void GameplayScreen::uiMenuBtnClickReset()
{
	if (gameState == GameState::PAUSED)
	{
		uiMenuResumePlay();
		levelSetToDefaults(levelsAll[levelCurrent]);
		uiGameplaySetToDefaults();
		uiGameplayGroup->setVisible(true);
		uiGameplayMessagesTextBox.get()->setText(uiGameplayMessagesLevelReset);
		gameState = GameState::PLAYING;
		turnOwner = TurnOwner::PLAYER;
	}
}

void GameplayScreen::uiMenuBtnClickExit()
{
	if (gameState == GameState::PAUSED)
	{
		this->parentWidget()->parentWidget()->close();
	}
}

void GameplayScreen::uiMenuResumePlay()
{
	gameState = GameState::PLAYING;
	uiMenuGroup.get()->setVisible(false);
}

void GameplayScreen::updateWindowTitle()
{
	this->parentWidget()->parentWidget()->setWindowTitle
	(
		windowTitleProgramName + " - Current Level: " + levelsAll[levelCurrent].name + " by " + levelsAll[levelCurrent].creator
	);
}

QPixmap GameplayScreen::stateToImg(const tokenImmobile::State &state, const tokenImmobile::Type &type)
{
	if (state == tokenImmobile::State::INVISIBLE || state == tokenImmobile::State::HELD)
	{
		return imgInvisible;
	}
	else if (state == tokenImmobile::State::ACTIVE)
	{
		if (type == tokenImmobile::Type::KEY)
			return imgMap.at("imgImmobileKey");
		else if (type == tokenImmobile::Type::GATE)
			return imgMap.at("imgImmobileGate");
		else if (type == tokenImmobile::Type::BLOCK)
			return imgMap.at("imgImmobileBlock");
		else if (type == tokenImmobile::Type::HAZARD)
			return imgMap.at("imgImmobileHazard");
		else if (type == tokenImmobile::Type::TELEPORT)
			return imgMap.at("imgImmobileTeleport");
		else
			return imgError;
	}
	else
		return imgError;
}

QPixmap GameplayScreen::stateToImg(const tokenUtil::State &state, const tokenUtil::Type &type)
{
	if (type == tokenUtil::Type::PUSHER)
	{
		if (state == tokenUtil::State::INACTIVE)
			return imgMap.at("imgUtilPusherInactive");
		else if (state == tokenUtil::State::ACTIVE)
			return imgMap.at("imgUtilPusherActive");
		else if (state == tokenUtil::State::HELD)
			return imgInvisible;
		else
			return imgError;
	}
	else if (type == tokenUtil::Type::SUCKER)
	{
		if (state == tokenUtil::State::INACTIVE)
			return imgMap.at("imgUtilSuckerInactive");
		else if (state == tokenUtil::State::ACTIVE)
			return imgMap.at("imgUtilSuckerActive");
		else if (state == tokenUtil::State::HELD)
			return imgInvisible;
		else
			return imgError;
	}
	else
		return imgError;
}

QPixmap GameplayScreen::facingToImg(const tokenPlayer::Facing &facing)
{
	if (facing == tokenPlayer::Facing::NEUTRAL)
		return imgMap.at("imgPlayerNeutral");
	else if (facing == tokenPlayer::Facing::UP)
		return imgMap.at("imgPlayerUp");
	else if (facing == tokenPlayer::Facing::DOWN)
		return imgMap.at("imgPlayerDown");
	else if (facing == tokenPlayer::Facing::LEFT)
		return imgMap.at("imgPlayerLeft");
	else if (facing == tokenPlayer::Facing::RIGHT)
		return imgMap.at("imgPlayerRight");
	else
		return imgError;
}

QPixmap GameplayScreen::facingToImg(const tokenPatroller::Facing &facing, const tokenPatroller::Type &type)
{
	if (type == tokenPatroller::Type::PUSHER)
	{
		if (facing == tokenPatroller::Facing::UP)
			return imgMap.at("imgPatrollerPusherUp");
		else if (facing == tokenPatroller::Facing::DOWN)
			return imgMap.at("imgPatrollerPusherDown");
		else if (facing == tokenPatroller::Facing::LEFT)
			return imgMap.at("imgPatrollerPusherLeft");
		else if (facing == tokenPatroller::Facing::RIGHT)
			return imgMap.at("imgPatrollerPusherRight");
		else
			return imgError;
	}
	else if (type == tokenPatroller::Type::SUCKER)
	{
		if (facing == tokenPatroller::Facing::UP)
			return imgMap.at("imgPatrollerSuckerUp");
		else if (facing == tokenPatroller::Facing::DOWN)
			return imgMap.at("imgPatrollerSuckerDown");
		else if (facing == tokenPatroller::Facing::LEFT)
			return imgMap.at("imgPatrollerSuckerLeft");
		else if (facing == tokenPatroller::Facing::RIGHT)
			return imgMap.at("imgPatrollerSuckerRight");
		else
			return imgError;
	}
	else
		return imgError;
}

const QFont::StyleStrategy GameplayScreen::fontStrategyToEnum(const QString &str)
{
	// Default to antialias strat if string received is unexpected.
	// Moxybox uses Pixellari with NoAntialias by default, but most fonts someone
	// would want to use are probably going to want to be used with antialiasing.
	// So in the event of a mod messing up the naming to call the right font,
	// we default to antialiasing.
	if (str == "NoAntialias")
		return QFont::StyleStrategy::NoAntialias;
	else if (str == "NoSubpixelAntialias")
		return QFont::StyleStrategy::NoSubpixelAntialias;
	else if (str == "PreferAntialias")
		return QFont::StyleStrategy::PreferAntialias;
	else
		return QFont::StyleStrategy::PreferAntialias;
}

const QString GameplayScreen::fontStrategyToString(const QFont::StyleStrategy &strat)
{
	if (strat == QFont::StyleStrategy::NoAntialias)
		return "NoAntialias";
	else if (strat == QFont::StyleStrategy::NoSubpixelAntialias)
		return "NoSubpixelAntialias";
	else if (strat == QFont::StyleStrategy::PreferAntialias)
		return "PreferAntialias";
	else
		return "PreferAntialias";
}

const QFont::Weight GameplayScreen::fontWeightToEnum(const QString &str)
{
	if (str == "Normal")
		return QFont::Weight::Normal;
	else if (str == "Thin")
		return QFont::Weight::Thin;
	else if (str == "ExtraLight")
		return QFont::Weight::ExtraLight;
	else if (str == "Light")
		return QFont::Weight::Light;
	else if (str == "Medium")
		return QFont::Weight::Medium;
	else if (str == "DemiBold")
		return QFont::Weight::DemiBold;
	else if (str == "Bold")
		return QFont::Weight::Bold;
	else if (str == "ExtraBold")
		return QFont::Weight::ExtraBold;
	else if (str == "Black")
		return QFont::Weight::Black;
	else
		return QFont::Weight::Normal;
}

const QString GameplayScreen::fontWeightToString(const QFont::Weight &strat)
{
	if (strat == QFont::Weight::Normal)
		return "Normal";
	else if (strat == QFont::Weight::Thin)
		return "Thin";
	else if (strat == QFont::Weight::ExtraLight)
		return "ExtraLight";
	else if (strat == QFont::Weight::Light)
		return "Light";
	else if (strat == QFont::Weight::Medium)
		return "Medium";
	else if (strat == QFont::Weight::DemiBold)
		return "DemiBold";
	else if (strat == QFont::Weight::Bold)
		return "Bold";
	else if (strat == QFont::Weight::ExtraBold)
		return "ExtraBold";
	else if (strat == QFont::Weight::Black)
		return "Black";
	else
		return "Normal";
}

QString GameplayScreen::extractSubstringInbetweenQt(const QString strBegin, const QString strEnd, const QString &strExtractFrom)
{
	QString extracted = "";
	int posFound = 0;

	if (!strBegin.isEmpty() && !strEnd.isEmpty())
	{
		while (strExtractFrom.indexOf(strBegin, posFound, Qt::CaseSensitive) != -1)
		{
			int posBegin = strExtractFrom.indexOf(strBegin, posFound, Qt::CaseSensitive) + strBegin.length();
			int posEnd = strExtractFrom.indexOf(strEnd, posBegin, Qt::CaseSensitive);
			extracted += strExtractFrom.mid(posBegin, posEnd - posBegin);
			posFound = posEnd;
		}
	}
	else if (strBegin.isEmpty() && !strEnd.isEmpty())
	{
		int posBegin = 0;
		int posEnd = strExtractFrom.indexOf(strEnd, posBegin, Qt::CaseSensitive);
		extracted += strExtractFrom.mid(posBegin, posEnd - posBegin);
		posFound = posEnd;
	}
	else if (!strBegin.isEmpty() && strEnd.isEmpty())
	{
		int posBegin = strExtractFrom.indexOf(strBegin, posFound, Qt::CaseSensitive) + strBegin.length();
		int posEnd = strExtractFrom.length();
		extracted += strExtractFrom.mid(posBegin, posEnd - posBegin);
		posFound = posEnd;
	}
	return extracted;
}

QStringList GameplayScreen::extractSubstringInbetweenQtLoopList(const QString strBegin, const QString strEnd, const QString &strExtractFrom)
{
	QStringList extracted;
	int posFound = 0;

	if (!strBegin.isEmpty() && !strEnd.isEmpty())
	{
		while (strExtractFrom.indexOf(strBegin, posFound, Qt::CaseSensitive) != -1)
		{
			int posBegin = strExtractFrom.indexOf(strBegin, posFound, Qt::CaseSensitive) + strBegin.length();
			int posEnd = strExtractFrom.indexOf(strEnd, posBegin, Qt::CaseSensitive);
			extracted.append(strExtractFrom.mid(posBegin, posEnd - posBegin));
			posFound = posEnd;
		}
	}
	else if (strBegin.isEmpty() && !strEnd.isEmpty())
	{
		int posBegin = 0;
		int posEnd = strExtractFrom.indexOf(strEnd, posBegin, Qt::CaseSensitive);
		extracted.append(strExtractFrom.mid(posBegin, posEnd - posBegin));
		posFound = posEnd;
	}
	else if (!strBegin.isEmpty() && strEnd.isEmpty())
	{
		int posBegin = strExtractFrom.indexOf(strBegin, posFound, Qt::CaseSensitive) + strBegin.length();
		int posEnd = strExtractFrom.length();
		extracted.append(strExtractFrom.mid(posBegin, posEnd - posBegin));
		posFound = posEnd;
	}
	return extracted;
}

void GameplayScreen::modLoadThemeIfExists()
{
	const QString modThemePath = appExecutablePath + "/Mods/Theme/theme.MoxyStyle";
	if (!QFile(modThemePath).exists())
		return;

	QFile fileRead(modThemePath);
	if (fileRead.open(QIODevice::ReadOnly))
	{
		QTextStream qStream(&fileRead);
		while (!qStream.atEnd())
		{
			QString line = qStream.readLine();

			// If a line has // at the beginning, we treat it as a comment and move on.
			// Doing this as a precaution in the event of a comment accidentally containing
			// a key phrase that is used to find something we want and triggering a false positive.
			// And so you can fiddle around with theme file, commenting stuff out to test,
			// without needing to delete it entirely.
			if (line[0] == '/' && line[1] == '/')
				continue;

			if (line.contains("::useTheme="))
			{
				bool useTheme = QVariant(extractSubstringInbetweenQt("::useTheme=", "::", line)).toBool();
				if (!useTheme)
				{
					fileRead.close();
					return;
				}
			}
			else if (line.contains("::standardFontFamily="))
			{
				standardFontFamily = extractSubstringInbetweenQt("::standardFontFamily=", "::", line);
			}
			else if (line.contains("::standardFontStyleStrategy="))
			{
				standardFontStyleStrategy = fontStrategyToEnum(extractSubstringInbetweenQt("::standardFontStyleStrategy=", "::", line));
			}
			else if (line.contains("::standardFontWeight="))
			{
				standardFontWeight = fontWeightToEnum(extractSubstringInbetweenQt("::standardFontWeight=", "::", line));
			}
			else if (line.contains("::uiGameplayFontGroupTitle_fontPointSize="))
			{
				uiGameplayFontGroupTitle_fontPointSize = extractSubstringInbetweenQt("::uiGameplayFontGroupTitle_fontPointSize=", "::", line).toInt();
			}
			else if (line.contains("::uiGameplayFontTextBox_fontPointSize="))
			{
				uiGameplayFontTextBox_fontPointSize = extractSubstringInbetweenQt("::uiGameplayFontTextBox_fontPointSize=", "::", line).toInt();
			}
			else if (line.contains("::uiMenuFontBtn_fontPointSize="))
			{
				uiMenuFontBtn_fontPointSize = extractSubstringInbetweenQt("::uiMenuFontBtn_fontPointSize=", "::", line).toInt();
			}

			// Due to the possibility of a stylesheet having :: as an actual part of its syntax, 
			// we use :::: as the front/back identifer instead.
			for (auto& style : styleMap)
			{
				QString identifier = "::::" + style.first + "=";
				if (line.contains(identifier))
				{
					// Images that show up in stylesheets need to have the app's executable path inserted at the beginning
					// of the path, since full path varies by user. %1 checks for insertion point, if present.
					// QString allows us to do a simple replace operation based on presence of %1.
					// If we were to have more than one kind of replacement we need to do, we can set up %2, %3, etc.
					if (line.contains("%1"))
						style.second = extractSubstringInbetweenQt(identifier, "::::", line).arg(appExecutablePath);
					else
						style.second = extractSubstringInbetweenQt(identifier, "::::", line);
					continue;
				}
			}

			for (auto& img : imgMap)
			{
				QString identifier = "::" + img.first + "=";
				if (line.contains(identifier))
				{
					img.second = QPixmap(appExecutablePath + extractSubstringInbetweenQt(identifier, "::", line));
					continue;
				}
			}
		}
		fileRead.close();
	}
}