#pragma once

#include <QCoreApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDirIterator>
#include <QTextStream>
#include <QShortcut>
#include <QKeyEvent>
#include <QGridLayout>
#include <QGroupBox>
#include <QFont>
#include <QTextBrowser>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QFileInfo>
#include <QInputDialog>
#include <algorithm>

class GameplayScreen : public QGraphicsView
{
	Q_OBJECT

public:
	GameplayScreen(QWidget *parent = nullptr);
	void prefSave();

protected:
	void keyReleaseEvent(QKeyEvent *event);

private:

	std::unique_ptr<QGraphicsScene> scene = std::make_unique<QGraphicsScene>();

	bool firstTimeSetup = true;

	const QString distributorName = "Robert Jay Myers Apps";
	const QString windowTitleProgramName = "Moxybox";
	const QString appExecutablePath = QCoreApplication::applicationDirPath();
	const QString windowsHomePath = QDir::homePath() + "/Documents/" + distributorName + "/" + windowTitleProgramName;
	const QString levelFolderName = "LevelData";
	const QString savesFolderName = "Saves";

	const QString levelDataPath = appExecutablePath + "/" + levelFolderName;
	const QString levelDataPathExtra = windowsHomePath + "/" + levelFolderName;
	const QString levelDataFileExtension = "MoxyLvl";

	QString fileDirLastSaved = windowsHomePath + "/" + savesFolderName;
	QString fileDirLastOpened = windowsHomePath + "/" + savesFolderName;

	enum class GameState { TITLE, PLAYING, PAUSED, KEYBINDING, LEVEL_COMPLETE, LEVEL_FAILED, LEVEL_ALL_DONE };
	GameState gameState = GameState::TITLE;

	// This is a turn-based game, so we process moves in order: Player -> Patrollers -> Player -> Patrollers -> Etc.
	// Player controls the start of a turn and patrollers end it.
	// Turn ownership determines who is expected to move next. We initialize it as none, since the game starts on
	// an intro splash screen and player should not be able to move until everything is ready for the first level.
	// Once the first level is ready, turn ownership should be set to Player, so they can begin the game.
	enum class TurnOwner { NONE, PLAYER, PATROLLER };
	TurnOwner turnOwner = TurnOwner::NONE;

	// These are the miscellaneous theme files where putting them in a map would be overkill.
	// Other theme files are below in image and style maps.
	QString standardFontFamily = "Pixellari";
	QFont::StyleStrategy standardFontStyleStrategy = QFont::NoAntialias;
	QFont::Weight standardFontWeight = QFont::Normal;
	int uiGameplayFontGroupTitle_fontPointSize = 12;
	int uiGameplayFontTextBox_fontPointSize = 12;
	int uiMenuFontBtn_fontPointSize = 14;

	// Using maps for images and style with string keys allows us to greatly simplify and standardize the logic
	// for setting them initially and in relation to themes. We can use the string key as the variable reference
	// in theme files. And load them more procedurally, in a loop, rather than going through a boilerplate series
	// of if statements. This also makes it easier to keep track, at a glance, what all modifiable images and stylesheets
	// are involved in the program.

	//---------
	// IMAGES
	//---------
	// Note that we could fiddle around with manually rotating a single QPixmap, rather than using multiple images,
	// for facing. But, this would mean if someone chooses to do an art style that is not top-down, they'll be stuck with
	// using the rotated versions of a single image for Left/Right/Up/Down. In some art styles, this means
	// it won't look right at all. The back of something might look drastically different from the front and need
	// to be drawn specifically to convey that perspective.
	std::map<QString, QPixmap> imgMap =
	{
		{ "imgImmobileKey", QPixmap(":/Moxybox/Resources/tokenImmobileKey.png") },
		{ "imgImmobileGate", QPixmap(":/Moxybox/Resources/tokenImmobileGate.png") },
		{ "imgImmobileBlock", QPixmap(":/Moxybox/Resources/tokenImmobileBlock.png") },
		{ "imgImmobileHazard", QPixmap(":/Moxybox/Resources/tokenImmobileHazard.png") },
		{ "imgImmobileTeleport", QPixmap(":/Moxybox/Resources/tokenImmobileTeleport.png") },
		{ "imgPlayerNeutral", QPixmap(":/Moxybox/Resources/tokenPlayerNeutral.png") },
		{ "imgPlayerLeft", QPixmap(":/Moxybox/Resources/tokenPlayerLeft.png") },
		{ "imgPlayerRight", QPixmap(":/Moxybox/Resources/tokenPlayerRight.png") },
		{ "imgPlayerUp", QPixmap(":/Moxybox/Resources/tokenPlayerUp.png") },
		{ "imgPlayerDown", QPixmap(":/Moxybox/Resources/tokenPlayerDown.png") },
		{ "imgPatrollerPusherLeft", QPixmap(":/Moxybox/Resources/tokenPatrollerPusherLeft.png") },
		{ "imgPatrollerPusherRight", QPixmap(":/Moxybox/Resources/tokenPatrollerPusherRight.png") },
		{ "imgPatrollerPusherUp", QPixmap(":/Moxybox/Resources/tokenPatrollerPusherUp.png") },
		{ "imgPatrollerPusherDown", QPixmap(":/Moxybox/Resources/tokenPatrollerPusherDown.png") },
		{ "imgPatrollerSuckerLeft", QPixmap(":/Moxybox/Resources/tokenPatrollerSuckerLeft.png") },
		{ "imgPatrollerSuckerRight", QPixmap(":/Moxybox/Resources/tokenPatrollerSuckerRight.png") },
		{ "imgPatrollerSuckerUp", QPixmap(":/Moxybox/Resources/tokenPatrollerSuckerUp.png") },
		{ "imgPatrollerSuckerDown", QPixmap(":/Moxybox/Resources/tokenPatrollerSuckerDown.png") },
		{ "imgUtilPusherInactive", QPixmap(":/Moxybox/Resources/tokenUtilPusherInactive.png") },
		{ "imgUtilPusherActive", QPixmap(":/Moxybox/Resources/tokenUtilPusherActive.png") },
		{ "imgUtilSuckerInactive", QPixmap(":/Moxybox/Resources/tokenUtilSuckerInactive.png") },
		{ "imgUtilSuckerActive", QPixmap(":/Moxybox/Resources/tokenUtilSuckerActive.png") },
		{ "imgGridCornerUpL", QPixmap(":/Moxybox/Resources/gridTextureCornerUpL.png") },
		{ "imgGridCornerUpR", QPixmap(":/Moxybox/Resources/gridTextureCornerUpR.png") },
		{ "imgGridCornerDownL", QPixmap(":/Moxybox/Resources/gridTextureCornerDownL.png") },
		{ "imgGridCornerDownR", QPixmap(":/Moxybox/Resources/gridTextureCornerDownR.png") },
		{ "imgGridInner", QPixmap(":/Moxybox/Resources/gridTextureInner.png") },
		{ "imgGridEdgeUpX", QPixmap(":/Moxybox/Resources/gridTextureEdgeUpX.png") },
		{ "imgGridEdgeDownX", QPixmap(":/Moxybox/Resources/gridTextureEdgeDownX.png") },
		{ "imgGridEdgeLeftY", QPixmap(":/Moxybox/Resources/gridTextureEdgeLeftY.png") },
		{ "imgGridEdgeRightY", QPixmap(":/Moxybox/Resources/gridTextureEdgeRightY.png") },
		{ "imgSplashTitle", QPixmap(":/Moxybox/Resources/splashTitle.png") },
		{ "imgSplashLevelComplete", QPixmap(":/Moxybox/Resources/splashLevelComplete.png") },
		{ "imgSplashLevelFailed", QPixmap(":/Moxybox/Resources/splashLevelFailed.png") },
		{ "imgSplashLevelAllDone", QPixmap(":/Moxybox/Resources/splashVictoryOverAll.png") },
	};

	// Unlike other images used in the program, "invisible" and "error" do not have a thematic purpose.
	// Meaning, there is no expectation of, or point to, them being changed as part of a mod theme.
	// So we make them const, unlike the rest of the images.
	const QPixmap imgInvisible = QPixmap(":/MoxyboxLevelCreator/Resources/invisible.png");
	const QPixmap imgError = QPixmap(":/MoxyboxLevelCreator/Resources/error.png");

	// -------------
	// STYLESHEETS
	// -------------
	std::map<QString, QString> styleMap =
	{
		{
			"baseStyle",
			"border: 0px; background-color: #000000;"
		},
		{
			"uiGameplayGroupBoxStyle",
			"QGroupBox { border-width: 1px;  border-style: solid;  border-color: #8E7320; background-color: #674003; }"
			"QGroupBox::title { border-width: 1px;  border-top-width: 0px;  border-style: solid;  border-color: #191405;  background-color: #8D9E45;  color: #191405;  subcontrol-origin: margin;  left: 5px; }"
		},
		{
			"uiGameplayTextBoxStyle",
			"QTextEdit { border: 0px;  background-color: #674003;  color: #C4BB81; }"
		},
		{
			"uiGameplayLabelStyle",
			"QLabel { border: 0px;  background-color: #674003;  color: #C4BB81; }"
		},
		{
			"uiGameplayKeymapBtnStyle",
			"QPushButton { border-width: 1px;  border-style: solid;  border-color: #674003;  background-color: #674003;  color: #C4BB81; }"
		},
		{
			"uiGameplayKeymapModifBtnStyle",
			"QPushButton { border-width: 1px;  border-style: solid;  border-color: #191405;  background-color: #8D9E45;  color: #191405; }"
		},
		{
			"uiMenuGroupBoxStyle",
			"QGroupBox { background-image: url(:/Moxybox/Resources/uiMenuBtsBg.png); }"
		},
		{
			"uiMenuBtnStyle",
			"QPushButton { background-color: rgba(255, 255, 255, 0);  border: none;  color: #C4BB81;  image: url(:/Moxybox/Resources/invisible.png); }"
		},
		{
			"uiMessageBoxStyle",
			"QMessageBox"
			"{"
				"border-width: 1px;"
				"border-style: solid;"
				"border-color: #8E7320;"
				"background-color: #674003;"
				"color: #C4BB81;"
			"}"
			"QMessageBox QLabel"
			"{"
				"border-width: 1px;"
				"border-style: solid;"
				"border-color: #674003;"
				"background-color: #674003;"
				"color: #C4BB81;"
				"padding: 3px;"
			"}"
			"QMessageBox QPushButton"
			"{"
				"border-width: 2px;"
				"border-style: solid;"
				"border-color: #191405;"
				"background-color: #8D9E45;"
				"color: #191405;"
				"padding: 2px 12px 2px 12px;"
			"}"
		}
	};

	// -------
	// TOKEN
	// -------
	// Currently there is only one possible player, so the player object accessed
	// should always be the first (and only) one in the players object vector (index 0).
	// You may be wondering: Why store the object in a vector at all if there is only one.
	// Reason: This is done to simplify the data storing/loading process for levels and keep it the same for all types of data.
	// It also leaves less obstacles to changing players to have more than one in the future, if desired.
	int pIndex = 0;

	// Z value for mobile tokens should be higher than that of grid pieces to be atop them.
	// And higher than that of immobile tokens (ex: player would be atop utility token).
	const int tokenMobileZ = 2;
	const int tokenImmobileZ = 1;

	const int tokenSize = 20; // Standard image size of all tokens. Unused, from SDL2 version of game.
	struct tokenPlayer
	{
		const int initialX;
		const int initialY;
		enum class Facing { NEUTRAL, UP, DOWN, LEFT, RIGHT };
		Facing facing = Facing::NEUTRAL;
		int heldKeys = 0;
		std::vector<int> heldUtilPushIndex;
		std::vector<int> heldUtilSuckIndex;
		int movementSpeed = 1; // This only checks collision for square landed on, so keep this in mind if changing it (it could break level design)
		int knockbackAmount = 2; // Be careful about what this is set to. Collision detection is likely expensive and runs for each square pushed back
		std::unique_ptr<QGraphicsPixmapItem> item = std::make_unique<QGraphicsPixmapItem>(nullptr);
	};
	struct tokenPatroller
	{
		// Patrollers use initial Y/X values along with bound points to leash their patrol path.
		// Make sure bound points keep each patroller within the bounds of the grid and not colliding with anything silly.
		// Patrollers have no collision detection for screen or objects (other than player), so you must consciously set 
		// their patrol bounds to avoid silly behavior. Level creation tool should leash their path within grid bound
		// on save, automatically.

		// Note: Patrol direction, type, patrol bound, are const at level load, so
		// we don't need to save or load them as part of save game file.
		// (not to be confused with level data file)

		const int initialX;
		const int initialY;
		enum class Type { PUSHER, SUCKER, ERROR };
		const Type type = Type::PUSHER;
		enum class Facing { UP, DOWN, LEFT, RIGHT, ERROR };
		const Facing facingInitial = Facing::UP;
		Facing facing = Facing::UP;
		enum class PatrolDir { VERTICAL, HORIZONTAL, ERROR };
		const PatrolDir patrolDir = PatrolDir::VERTICAL;
		const int patrolBoundUp = 3;
		const int patrolBoundDown = 3;
		const int patrolBoundLeft = 3;
		const int patrolBoundRight = 3;
		int movementSpeed = 1;
		std::unique_ptr<QGraphicsPixmapItem> item = std::make_unique<QGraphicsPixmapItem>(nullptr);

		static const Facing facingToEnum(const QString &str)
		{
			if (str == "UP")
				return Facing::UP;
			else if (str == "DOWN")
				return Facing::DOWN;
			else if (str == "LEFT")
				return Facing::LEFT;
			else if (str == "RIGHT")
				return Facing::RIGHT;
			else
				return Facing::ERROR;
		}

		static const QString facingToString(const Facing &facing)
		{
			if (facing == Facing::UP)
				return "UP";
			else if (facing == Facing::DOWN)
				return "DOWN";
			else if (facing == Facing::LEFT)
				return "LEFT";
			else if (facing == Facing::RIGHT)
				return "RIGHT";
			else
				return "ERROR";
		}

		static const PatrolDir patrolDirToEnum(const QString &str)
		{
			if (str == "VERTICAL")
				return PatrolDir::VERTICAL;
			else if (str == "HORIZONTAL")
				return PatrolDir::HORIZONTAL;
			else
				return PatrolDir::ERROR;
		}

		static const QString patrolDirToString(const PatrolDir &patrolDir)
		{
			if (patrolDir == PatrolDir::VERTICAL)
				return "VERTICAL";
			else if (patrolDir == PatrolDir::HORIZONTAL)
				return "HORIZONTAL";
			else
				return "ERROR";
		}

		static const Type typeToEnum(const QString &str)
		{
			if (str == "PUSHER")
				return Type::PUSHER;
			else if (str == "SUCKER")
				return Type::SUCKER;
			else
				return Type::ERROR;
		}

		static const QString typeToString(const Type &type)
		{
			if (type == Type::PUSHER)
				return "PUSHER";
			else if (type == Type::SUCKER)
				return "SUCKER";
			else
				return "ERROR";
		}
	};
	struct tokenImmobile
	{
		const int initialX;
		const int initialY;
		enum class Type { BLOCK, KEY, GATE, HAZARD, TELEPORT, ERROR };
		Type type = Type::BLOCK;
		enum class State { ACTIVE, INVISIBLE, HELD, ERROR };
		State state = State::ACTIVE;
		std::unique_ptr<QGraphicsPixmapItem> item = std::make_unique<QGraphicsPixmapItem>(nullptr);

		static const State stateToEnum(const QString &str)
		{
			if (str == "ACTIVE")
				return State::ACTIVE;
			else if (str == "INVISIBLE")
				return State::INVISIBLE;
			else if (str == "HELD")
				return State::HELD;
			else
				return State::ERROR;
		}

		static const QString stateToString(const State &state)
		{
			if (state == State::ACTIVE)
				return "ACTIVE";
			else if (state == State::INVISIBLE)
				return "INVISIBLE";
			else if (state == State::HELD)
				return "HELD";
			else
				return "ERROR";
		}

		static const Type typeToEnum(const QString &str)
		{
			if (str == "BLOCK")
				return Type::BLOCK;
			else if (str == "KEY")
				return Type::KEY;
			else if (str == "GATE")
				return Type::GATE;
			else if (str == "HAZARD")
				return Type::HAZARD;
			else if (str == "TELEPORT")
				return Type::TELEPORT;
			else
				return Type::ERROR;
		}

		static const QString typeToString(const Type &type)
		{
			if (type == Type::BLOCK)
				return "BLOCK";
			else if (type == Type::KEY)
				return "KEY";
			else if (type == Type::GATE)
				return "GATE";
			else if (type == Type::HAZARD)
				return "HAZARD";
			else if (type == Type::TELEPORT)
				return "TELEPORT";
			else
				return "ERROR";
		}
	};
	struct tokenUtil
	{
		// Note:
		// Traps don't change patrol route of patrollers.
		// Suck trap can change the axis the patroller is patrolling on, if it's an axis that isn't their patrol axis.
		// e.g. if patrolling on X axis, they can be sucked down or up to another Y row in the grid.
		// They will stay on that Y unless sucked again in another Y direction,
		// because their patrol route is leashed to X axis, not to Y axis.

		const int initialX;
		const int initialY;
		enum class Type { PUSHER, SUCKER, ERROR };
		Type type = Type::PUSHER;
		enum class State { INACTIVE, ACTIVE, HELD, ERROR };
		const State stateBase = State::INACTIVE; // Unchanged state, pulled from level data file, reset state to this.
		State stateModified = State::INACTIVE; // Changeable state, for use in the middle of playing a level.
		std::unique_ptr<QGraphicsPixmapItem> item = std::make_unique<QGraphicsPixmapItem>(nullptr);

		static Type typeToEnum(const QString &str)
		{
			if (str == "PUSHER")
				return Type::PUSHER;
			else if (str == "SUCKER")
				return Type::SUCKER;
			else
				return Type::ERROR;
		}

		static const QString typeToString(const Type &type)
		{
			if (type == Type::PUSHER)
				return "PUSHER";
			else if (type == Type::SUCKER)
				return "SUCKER";
			else
				return "ERROR";
		}

		static const State stateToEnum(const QString &str)
		{
			if (str == "INACTIVE")
				return State::INACTIVE;
			else if (str == "ACTIVE")
				return State::ACTIVE;
			else if (str == "HELD")
				return State::HELD;
			else
				return State::ERROR;
		}

		static const QString stateToString(const State &state)
		{
			if (state == State::INACTIVE)
				return "INACTIVE";
			else if (state == State::ACTIVE)
				return "ACTIVE";
			else if (state == State::HELD)
				return "HELD";
			else
				return "ERROR";
		}
	};

	// ------
	// GRID
	// ------
	struct gridPiece
	{
		QPoint gridPoint;
		QPointF pos;
		std::unique_ptr<QGraphicsPixmapItem> item = std::make_unique<QGraphicsPixmapItem>(nullptr);
	};
	std::vector<gridPiece> gridPiecesAll;

	// Note that there's some framework lingering here from a scrapped feature to have grid size modifiable by level.
	// As well as a scrapped framework to have the size of grid pieces modifiable (for zooming and the like).
	// (Both are from an SLD2 version of the game, where implementation details are different.)
	// This has been scrapped, setting everything to const, though some of the framework details may linger.
	// If you want to make grid row/col size modifiable, for example, you'll need to remove their const flag.
	// If you want to change grid piece size, it's probably easier to use QGraphicsView's "scaling".
	const int gridPieceZ = 0; // Z value for grid pieces should be lower than that of tokens to be under them.
	const int gridRowSize = 20; // This should not be higher than screenWidth / gridPieceSize
	const int gridColSize = 10; // This should not be higher than ((2/3rds of screenHeight) / gridPieceSize) to have room for UI below it
	const int gridPieceSizeDefault = 40;
	const int gridPieceSize = gridPieceSizeDefault;
	const int gridPieceSizeDstMultiplier = 2;
	const int gridPieceSizeSrc = 40;
	const int gridAnchorX = 0;
	const int gridAnchorY = 0;
	const int gridWidth = gridRowSize * gridPieceSize;
	const int gridHeight = gridColSize * gridPieceSize;
	const int gridBoundUp = gridAnchorY;
	const int gridBoundDown = gridHeight + gridAnchorY;
	const int gridBoundLeft = gridAnchorX;
	const int gridBoundRight = gridWidth + gridAnchorX;

	const int screenWidth = 800;
	const int screenHeight = 600;

	// ------------
	// LEVEL DATA
	// ------------
	int levelCurrent = 0;
	int levelsFound;
	int levelsComplete = 0;
	int levelsRemaining = 0;
	struct levelData
	{
		QString id;
		QString creator;
		QString name;
		int difficulty;
		int turnsInitial;
		int turnsRemaining;
		enum class State { COMPLETE, STARTED, UNTOUCHED };
		State state = State::UNTOUCHED;

		std::vector<tokenPlayer> players;
		std::vector<tokenPatroller> pushers;
		std::vector<tokenPatroller> suckers;
		std::vector<tokenImmobile> blocks;
		std::vector<tokenImmobile> keys;
		std::vector<tokenImmobile> gates;
		std::vector<tokenImmobile> hazards;
		std::vector<tokenImmobile> teleports;
		std::vector<tokenUtil> utils;
	};
	std::vector<levelData> levelsAll;

	// --------------
	// SPLASHSCREEN
	// --------------
	const int splashZ = 10; // Should have a higher Z value than other things to make sure splash screen shows.
	std::unique_ptr<QGraphicsPixmapItem> splashItem = std::make_unique<QGraphicsPixmapItem>(nullptr);

	// -------------
	// UI GAMEPLAY
	// -------------
	const int uiGameplayKeymapBtnSize = 20;

	QFont uiGameplayFontGroupTitle;
	QFont uiGameplayFontTextBox;

	std::unique_ptr<QGroupBox> uiGameplayGroup = std::make_unique<QGroupBox>(this);
	std::unique_ptr<QGridLayout> uiGameplayLayout = std::make_unique<QGridLayout>();

	std::unique_ptr<QGroupBox> uiGameplayStatsGroup = std::make_unique<QGroupBox>();
	std::unique_ptr<QGroupBox> uiGameplayObjectivesGroup = std::make_unique<QGroupBox>();
	std::unique_ptr<QGroupBox> uiGameplayMessagesGroup = std::make_unique<QGroupBox>();
	std::unique_ptr<QGroupBox> uiGameplayKeymapGroup = std::make_unique<QGroupBox>();

	std::unique_ptr<QGridLayout> uiGameplayStatsLayout = std::make_unique<QGridLayout>();
	std::unique_ptr<QGridLayout> uiGameplayObjectivesLayout = std::make_unique<QGridLayout>();
	std::unique_ptr<QGridLayout> uiGameplayMessagesLayout = std::make_unique<QGridLayout>();
	std::unique_ptr<QGridLayout> uiGameplayKeymapLayout = std::make_unique<QGridLayout>();

	enum class StatCounterType { TURNS_REMAINING, KEYS_HELD, TRAPS_PUSHERS, TRAPS_SUCKERS };
	struct statComponent
	{
		const QString name; // The part that precedes the number, e.g. "Turns Remaining: "
		const int gridLayoutRow;
		const int gridLayoutCol;
		const Qt::Alignment gridLayoutAlign;
		std::unique_ptr<QLabel> widget = std::make_unique<QLabel>();

		void updateCounter(const int &number)
		{
			widget.get()->setText(name + QString::number(number));
		}
	};
	std::map<StatCounterType, statComponent> statCounterMap;

	std::unique_ptr<QTextBrowser> uiGameplayObjectivesTextBox = std::make_unique<QTextBrowser>();
	std::unique_ptr<QTextBrowser> uiGameplayMessagesTextBox = std::make_unique<QTextBrowser>();

	const Qt::Key keybindNextLevel = Qt::Key::Key_Space;
	const Qt::Key keybindResetLevel = Qt::Key::Key_R;

	// Note: We name these as debug to distinguish them as keys that are not a part of the
	// interface communicated to the player. They are available for usage regardless of whether
	// program is technically in debug mode. We put these in so that testing a specific level is easier.
	// Otherwise, it's going to be hard for people to test new levels they make with the level creator.
	const Qt::Key keybindSkipLevel_DEBUG = Qt::Key::Key_F1;
	const Qt::Key keybindLoadLevelByName_DEBUG = Qt::Key::Key_F2;

	// We set up an enum ID for each modifiable keybind, so that when the UI is clicked
	// to modify a key, we know which one to apply the modification to after key input.
	enum class KeybindModifiable
	{
		NONE,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN,
		PLACE_PUSHER_UTIL,
		PLACE_SUCKER_UTIL,
		OPEN_MENU
	};
	KeybindModifiable keybindToModify = KeybindModifiable::NONE;

	struct keybindComponent
	{
		const QString labelText;
		Qt::Key keybind;
		const int uiButtonGridLayoutRow;
		const int uiButtonGridLayoutCol;
		const int uiLabelGridLayoutRow;
		const int uiLabelGridLayoutCol;
		const Qt::Alignment gridLayoutAlign;
		std::unique_ptr<QPushButton> uiButton = std::make_unique<QPushButton>();
		std::unique_ptr<QLabel> uiLabel = std::make_unique<QLabel>();
	};
	std::map<KeybindModifiable, keybindComponent> keybindMap;

	const QString uiGameplayKeymapGroupTitleDefault = "Keybinds";
	const QString uiGameplayKeymapGroupTitleKeybinding = "Hit Key to Set";

	const int minBoxWidth = (screenWidth / 4) - 6;

	const QString uiGameplayMessagesObstacle = "Need force of impact to get past obstacles.";
	const QString uiGameplayMessagesHazard = "Need knockback impact to cross hazards. Careful! If you land on it, you have to start the level over.";
	const QString uiGameplayMessagesTeleport = "You traveled through a special route, coming up on the other end.";
	const QString uiGameplayMessagesKeyObtained = "You got a key! Keys can be used to open pods.";
	const QString uiGameplayMessagesKeyNeeded = "Key needed to open pod.";
	const QString uiGameplayMessagesTrapPusherObtained = "You picked up a Spring Trap! Spring traps will knock back patrollers who hit them.";
	const QString uiGameplayMessagesTrapPusherDeployed = "Spring Trap deployed.";
	const QString uiGameplayMessagesTrapSuckerObtained = "You picked up a Magnet Trap! Magnet Traps will pull in patrollers who come near.";
	const QString uiGameplayMessagesTrapSuckerDeployed = "Magnet Trap deployed.";
	const QString uiGameplayMessagesLevelReset = "Level Reset.";

	// ---------
	// UI MENU
	// ---------
	QFont uiMenuFontBtn;

	const int uiMenuWidth = 214;
	const int uiMenuHeight = 350;

	const int uiMenuBtnWidth = uiMenuWidth;
	const int uiMenuBtnHeight = 50;
	const int uiMenuBtnSpacing = 25;

	std::unique_ptr<QGroupBox> uiMenuGroup = std::make_unique<QGroupBox>(this);
	std::unique_ptr<QGridLayout> uiMenuLayout = std::make_unique<QGridLayout>();

	enum class UiMenuBtnType
	{
		RESUME,
		SAVE,
		LOAD,
		RESET,
		EXIT
	};
	struct uiMenuBtnComponent
	{
		const QString text;
		const int gridLayoutRow;
		const int gridLayoutCol;
		Qt::Alignment gridLayoutAlign;
		std::unique_ptr<QPushButton> widget = std::make_unique<QPushButton>();
	};
	std::map<UiMenuBtnType, uiMenuBtnComponent> uiMenuBtnMap;

	// -----------
	// FUNCTIONS
	// -----------
	void prefLoad();
	void dirIteratorLoadLevelData(const QString &dirPath);
	bool hitSolidObjectPlayerMoving();
	bool hitImmobileObject(const std::vector<tokenImmobile>& immobiles, const int playerNextY, const int playerNextX);
	bool hitImmobileObjectAndDelete(std::vector<tokenImmobile>& immobiles, const int playerNextY, const int playerNextX);
	bool hitUtil(const int playerNextY, const int playerNextX);
	bool hitEnemy(const std::vector<tokenPatroller>& patrollers, const int playerNextY, const int playerNextX);
	void knockbackPlayerMoving();
	void updatePositionPatrollers();
	void updatePositionPatrollerMoves(std::vector<tokenPatroller>& patrollers, const int enemyNum);
	void knockbackEnemyMoving(int enemyNum);
	bool patrollerHitTrapPush(const std::vector<tokenPatroller>& patrollers, const int enemyNum, const int patrollerNextY, const int patrollerNextX);
	void knockbackHitTrap(std::vector<tokenPatroller>& patrollers, const int enemyNum);
	bool hitPlayer(const std::vector<tokenPatroller>& patrollers, const int enemyNum);
	void suckInHitTrap(std::vector<tokenPatroller>& patrollers, const int enemyNum);
	void suckInHitCheck();
	void suckInRange();
	void hazardHitCheck();
	void teleportHitCheck();
	void levelSetFailed();
	void levelSetToDefaults(levelData& level);
	void levelSetComplete();
	int levelLoadValidateId(QFile &file);
	int levelFoundInListAtPos(const QString &id);
	int levelFoundInList(const QString &id);
	bool allGatesOpened();
	void addCurrentLevelToScene();
	void removeCurrentLevelFromScene();
	void uiGameplaySetToDefaults();
	void uiGameplayUpdateStatCounter(const StatCounterType &statCounterType);
	void uiMenuBtnClickResume();
	void uiMenuBtnClickSave();
	void uiMenuBtnClickLoad();
	void uiMenuBtnClickReset();
	void uiMenuBtnClickExit();
	void uiMenuResumePlay();
	void updateWindowTitle();
	QPixmap stateToImg(const tokenImmobile::State &state, const tokenImmobile::Type &type);
	QPixmap stateToImg(const tokenUtil::State &state, const tokenUtil::Type &type);
	QPixmap facingToImg(const tokenPlayer::Facing &facing);
	QPixmap facingToImg(const tokenPatroller::Facing &facing, const tokenPatroller::Type &type);
	const QFont::StyleStrategy fontStrategyToEnum(const QString &str);
	const QString fontStrategyToString(const QFont::StyleStrategy &strat);
	const QFont::Weight fontWeightToEnum(const QString &str);
	const QString fontWeightToString(const QFont::Weight &strat);
	QString extractSubstringInbetweenQt(const QString strBegin, const QString strEnd, const QString &strExtractFrom);
	QStringList extractSubstringInbetweenQtLoopList(const QString strBegin, const QString strEnd, const QString &strExtractFrom);
	void modLoadThemeIfExists();
};