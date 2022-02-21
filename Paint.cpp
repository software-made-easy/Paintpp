#include <iostream>
using namespace std;

#include <QDesktopWidget>
#include <QMouseEvent>
#include <QFileDialog>
#include <QColorDialog>
#include <QSignalMapper>
#include <QMenuBar>
#include <QMenu>
#include <QApplication>
#include <QImageWriter>
#include <QScrollArea>
#include <QTranslator>
#include <QStandardPaths>

#include "Paint.h"
#include "commands.h"
#include "draw_area.h"
#include "about.h"

#ifdef Q_OS_ANDROID
#include <QtColorWidgets/color_dialog.hpp>
#endif

#if defined(Q_OS_BLACKBERRY) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WP)
#define Q_OS_MOBILE
#else
#define Q_OS_DESKTOP
#endif
#if (defined (LINUX) || defined (__linux__)) && !(defined (ANDROID) || defined (__ANDROID__))
#define JUST_LINUX
#endif

QTranslator translator, qtTranslator;


/**
 * @brief MainWindow::MainWindow - the main window, parent to every other
 *                                 widget.
 */
MainWindow::MainWindow(const char* name, QWidget *parent)
    :QMainWindow(parent)
{
    // create the DrawArea, which will receive the draw mouse events
    drawArea = new DrawArea(this);
    drawArea->setStyleSheet("background-color:transparent");

    // get default tool
    currentTool = drawArea->getCurrentTool();

    // create the menu and toolbar
    createMenuAndToolBar();

    // init dialog pointers to 0
    penDialog = 0;
    lineDialog = 0;
    eraserDialog = 0;
    rectDialog = 0;

    // adjust window size, name, & stop context menu
    setWindowTitle(tr(name));
    resize(QDesktopWidget().availableGeometry(this).size()*.6);
    setContextMenuPolicy(Qt::PreventContextMenu);
    setCentralWidget(drawArea);

    // settings
#ifdef Q_OS_ANDROID
    settings = new QSettings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/SME_PAINT++.ini",
                             QSettings::IniFormat, this);
#else
    settings = new QSettings("SME", "Paint++", this);
#endif

    loadSettings();
}

MainWindow::~MainWindow()
{
    delete penDialog;
    delete lineDialog;
    delete eraserDialog;
    delete rectDialog;

    foreach (QAction *action, imageActions << toolActions) {
        action->deleteLater();
    }
    imageActions.clear();
    toolActions.clear();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings();
    event->accept();
}

void MainWindow::retranslate() {
    fileMenu->setTitle(QApplication::translate("MainWindow", "File"));
}

void MainWindow::language(QString language) {
    if (language != "en") {
        if (translator.load(":/i18n/Paint_" + language)) {
            qApp->installTranslator(&translator);
        }
        if (qtTranslator.load(":/translation/qtbase_" + language)) {
            qApp->installTranslator(&qtTranslator);
        }
    }
    else {
        qApp->removeTranslator(&translator);
        qApp->removeTranslator(&qtTranslator);
    }
    currLang = language;
    retranslate();
}

// Settings management
void MainWindow::loadSettings() {
    currLang = settings->value("lang", QString()).toString();
    if (currLang.isEmpty() || currLang.isNull()) {
        currLang = QLocale::system().name().split("_").first();
    }

    if (currLang != "en") {
        language(currLang);
    }
    restoreGeometry(settings->value("geometry", QByteArray()).toByteArray());
    restoreState(settings->value("state", QByteArray()).toByteArray());
}

void MainWindow::saveSettings() {
    settings->setValue("lang", currLang);
    settings->setValue("geometry", saveGeometry());
    settings->setValue("state", saveState());
    settings->sync();
}

/**
 * @brief MainWindow::mousePressEvent - On mouse right click, open dialog menu.
 *
 */

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton) {
        openToolDialog();
    }
}

/**
 * @brief MainWindow::OnNewImage - Open a NewCanvasDialogue prompting user to
 *                                 enterthe dimensions for a new image.
 */
void MainWindow::OnNewImage()
{
    CanvasSizeDialog *newCanvas = new CanvasSizeDialog(this, "New Canvas");
    newCanvas->exec();
    // if user hit 'OK' button, create new image
    if (newCanvas->result())
    {
        QSize size = QSize(newCanvas->getWidthValue(),
                           newCanvas->getHeightValue());
        drawArea->createNewImage(size);
    }
    // done with the dialog, free it
    delete newCanvas;
}

/**
 * @brief MainWindow::OnLoadImage - Open a QFileDialogue prompting user to
 *                                  browse for a file to open.
 */
void MainWindow::OnLoadImage()
{
    QString types;
    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        types = types + " *." + QString(format);
    }
    QString format = QApplication::translate("MainWindow", "Images") + "(" + types + ")";
    // Get the file to open from a dialog
    // QApplication::tr sets the window title to Open File
    // QDir opens the current dirctory
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    QApplication::translate("MainWindow", "Open File"),
                                                    QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).last(),
                                                    format);

    if (!(fileName.isEmpty() || fileName.isNull()))
	{
        drawArea->loadImage(fileName);
	}
}

/**
 * @brief MainWindow::OnSaveImage - Save Image, call OnSaveAsImage
 *                                  if path is empty
 */
void MainWindow::OnSaveImage()
{
    if(drawArea->getImage()->isNull())
        return;

    if (path.isEmpty() || path.isNull()) {
        return OnSaveAsImage();
    }

    drawArea->saveImage(path);
}

/**
 * @brief MainWindow::OnSaveImage - Open a QFileDialogue prompting user to
 *                                  enter a filename and save location.
 */
void MainWindow::OnSaveAsImage() {
    if(drawArea->getImage()->isNull())
        return;

    QString types;
    QList<QByteArray> supportedImageFormats = QImageWriter::supportedImageFormats();
    foreach (QByteArray format, supportedImageFormats) {
        types.append(QString(QString(format).toUpper() + " (*." + QString(format) + ")"
                             ));
        if (format != supportedImageFormats.last()) {
            types.append(";; ");
        }
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    QApplication::translate("MainWindow", "Open File"),
                                                    "", types, new QString("PNG (*.png)")
                                                    );

    // If we have a file name load the image and place
    // it in the scribbleArea
    if (!fileName.isEmpty() || !fileName.isNull()) {
        path = fileName;
        drawArea->saveImage(fileName);
    }

}

/**
 * @brief MainWindow::OnResizeImage - Change the dimensions of the image.
 *
 */
void MainWindow::OnResizeImage()
{
    QPixmap *image = drawArea->getImage();
    if(image->isNull())
        return;

    CanvasSizeDialog* newCanvas = new CanvasSizeDialog(this,
                                                       QApplication::translate("MainWindow", "Resize Image"),
                                                       image->width(),
                                                       image->height());
    newCanvas->exec();
    // if user hit 'OK' button, create new image
    if (newCanvas->result())
    {
         drawArea->resizeImage(QSize(newCanvas->getWidthValue(),
                                     newCanvas->getHeightValue()));
    }
    // done with the dialog, free it
    delete newCanvas;
}

/**
 * @brief MainWindow::OnPickColor - Open a QColorDialog prompting the user to
 *                                  select a color.
 *
 */
void MainWindow::OnPickColor(int which)
{
    QColor foregroundColor = drawArea->getForegroundColor();
    QColor backgroundColor = drawArea->getBackgroundColor();
    QColor newColor;
#ifdef Q_OS_ANDROID
    color_widgets::ColorDialog dialog(this);
    dialog.setColor(which == foreground ? foregroundColor
                                        : backgroundColor);
    dialog.setButtonMode(color_widgets::ColorDialog::OkCancel);
    connect(&dialog, QOverload<QColor>::of(&color_widgets::ColorDialog::colorSelected), this,
            [&newColor](QColor color){newColor = color;});
    dialog.exec();
#else
    newColor = QColorDialog::getColor(which == foreground ? foregroundColor
                                                              : backgroundColor,
                                          this,
                                          tr(which == foreground ? "Foreground Color"
                                                              : "Background Color"
                                          ));
#endif

    // if user hit 'OK' button, change the color
    if (newColor.isValid())
        drawArea->updateColorConfig(newColor, which);
}

/**
 * @brief MainWindow::OnChangeTool - Sets the current tool based on argument.
 *
 */
void MainWindow::OnChangeTool(int newTool)
{
    currentTool = drawArea->setCurrentTool(newTool); // notify observer
}

/**
 * @brief MainWindow::OnPenDialog - Open a PenDialog prompting the user to
 *                                  change pen settings.
 *
 */
void MainWindow::OnPenDialog()
{
    if(!penDialog)
        penDialog = new PenDialog(this, drawArea);

    if(penDialog && penDialog->isVisible())
        return;

    penDialog->show();
}

/**
 * @brief MainWindow::OnLineDialog - Open a LineDialog prompting the user to
 *                                   change line tool settings.
 *
 */
void MainWindow::OnLineDialog()
{
    if(!lineDialog)
        lineDialog = new LineDialog(this, drawArea);

    if(lineDialog && lineDialog->isVisible())
        return;

    lineDialog->show();
}

/**
 * @brief MainWindow::OnEraserDialog - Open a EraserDialog prompting the user
 *                                     to change eraser settings.
 *
 */
void MainWindow::OnEraserDialog()
{
    if (!eraserDialog)
        eraserDialog = new EraserDialog(this, drawArea);

    if(eraserDialog->isVisible())
        return;

    eraserDialog->show();
}

/**
 * @brief MainWindow::OnRectangleDialog - Open a RectDialog prompting the user
 *                                        to change rect tool settings.
 *
 */
void MainWindow::OnRectangleDialog()
{
    if (!rectDialog)
        rectDialog = new RectDialog(this, drawArea);

    if(rectDialog->isVisible())
        return;

    rectDialog->show();
}

/**
 * @brief MainWindow::openToolDialog - call the appropriate dialog function
 *                                     based on the current tool.
 *
 */
void MainWindow::openToolDialog()
{
    switch(currentTool->getType())
    {
        case pen: OnPenDialog();             break;
        case line: OnLineDialog();           break;
        case eraser: OnEraserDialog();       break;
        case rect_tool: OnRectangleDialog(); break;
    }
}

/**
 * @brief MainWindow::OnAboutDialog() - show about dialog
 */
void MainWindow::OnAboutDialog()
{
    About *dialog = new About(this);
    dialog->exec();
    delete dialog;
}

/**
 * @brief ToolBar::createMenuAndToolBar() - ensure that everything gets
 *                                          created in the correct order
 *
 */
void MainWindow::createMenuAndToolBar()
{
    // create the toolbar
    toolbar = new ToolBar(this);
    toolbar->setObjectName("toolbar");
#ifdef Q_OS_MOBILE
    toolbar->setIconSize(QSize(128, 128));
#endif
    addToolBar(toolbar);

    // create actions and add them to the menu
    createMenuActions();

    // add actions to the tool bar
    toolbar->showActions(imageActions, toolActions);
}

/**
 * @brief MainWindow::createMenu - create the actions that appear in the menu
 *
 */
void MainWindow::createMenuActions()
{
    // load button icons from files/theme
#ifdef JUST_LINUX
    QIcon newIcon = QIcon::fromTheme("document-new");
    QIcon openIcon = QIcon::fromTheme("document-open");
    QIcon saveIcon = QIcon::fromTheme("document-save");
    QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QIcon undoIcon = QIcon::fromTheme("edit-undo");
    QIcon redoIcon = QIcon::fromTheme("edit-redo");
    QIcon clearIcon = QIcon::fromTheme("edit-clear");
    QIcon resizeIcon = QIcon(":/icons/edit-resize-dark");
    QIcon fColorIcon = QIcon::fromTheme("color-management");
    QIcon bColorIcon = QIcon::fromTheme("background-color");
    QIcon penIcon = QIcon::fromTheme("edit");
    QIcon lineIcon = QIcon::fromTheme(":icons/lineIcon");
    QIcon eraserIcon = QIcon(":/icons/eraser");
    QIcon rectIcon = QIcon::fromTheme(":/icons/rectIcon");
    QIcon exitIcon = QIcon::fromTheme("application-exit");
    QIcon aboutIcon(":/icons/Icon");
#else
    QIcon newIcon(":/icons/newIcon");
    QIcon openIcon(":/icons/openIcon");
    QIcon saveIcon(":/icons/saveIcon");
    QIcon saveAsIcon(":/icons/document-save-as");
    QIcon undoIcon(":/icons/undoIcon");
    QIcon redoIcon(":/icons/redoIcon");
    QIcon clearIcon(":/icons/clearAllIcon");
    QIcon resizeIcon(":/icons/edit-resize");
    QIcon fColorIcon(":/icons/color-managment");
    QIcon bColorIcon(":/icons/bColorIcon");
    QIcon penIcon(":/icons/penIcon");
    QIcon lineIcon(":/icons/lineIcon");
    QIcon eraserIcon(":/icons/eraser");
    QIcon rectIcon(":/icons/rectIcon");
    QIcon exitIcon(":/icons/application-exit");
    QIcon aboutIcon(":/icons/Icon");
#endif

    // File
    fileMenu = new QMenu(QApplication::translate("MainWindow", "File"), this);
    newAction = fileMenu->addAction(newIcon, QApplication::translate("MainWindow", "New"),
                                    this, SLOT(OnNewImage()), QKeySequence("Ctrl+N"));
    openAction = fileMenu->addAction(openIcon, QApplication::translate("MainWindow", "Load"),
                                     this, SLOT(OnLoadImage()), QKeySequence("Ctrl+O"));
    saveAction = fileMenu->addAction(saveIcon, QApplication::translate("MainWindow", "Save"),
                                     this, SLOT(OnSaveImage()), QKeySequence("Ctrl+S"));
    saveAsAction = fileMenu->addAction(saveAsIcon, QApplication::translate("MainWindow", "Save as"), this,
                                       SLOT(OnSaveAsImage()), QKeySequence("Ctrl+Shift+S"));
    exitAction = fileMenu->addAction(exitIcon, QApplication::translate("MainWindow", "Exit"),
                                     this, SLOT(close()), QKeySequence("Ctrl+Q"));

    // Edit
    editMenu = new QMenu(QApplication::translate("MainWindow", "Edit"), this);
    undoAction = editMenu->addAction(undoIcon, QApplication::translate("MainWindow", "Undo"),
                                     drawArea, SLOT(OnUndo()), QKeySequence("Ctrl+Z"));
    redoAction = editMenu->addAction(redoIcon, QApplication::translate("MainWindow", "Redo"),
                                     drawArea, SLOT(OnRedo()), QKeySequence("Ctrl+Y"));
    clearAction = editMenu->addAction(clearIcon, QApplication::translate("MainWindow", "Clear Canvas"),
                                      drawArea, SLOT(OnClearAll()), QKeySequence("Ctrl+C"));
    resizeAction = editMenu->addAction(resizeIcon, QApplication::translate("MainWindow", "Resize Image..."),
                                       this, SLOT(OnResizeImage()), QKeySequence("Ctrl+R"));

    // color pickers (still under >Edit)
    QSignalMapper *signalMapper = new QSignalMapper(this);

    fColorAction = new QAction(fColorIcon, QApplication::translate("MainWindow", "Foreground Color..."), this);
    connect(fColorAction, SIGNAL(triggered()),
            signalMapper, SLOT(map()));
    fColorAction->setShortcut(QKeySequence("Ctrl+F"));

    bColorAction = new QAction(bColorIcon, QApplication::translate("MainWindow", "Background Color..."), this);
    connect(bColorAction, SIGNAL(triggered()),
            signalMapper, SLOT(map()));
    bColorAction->setShortcut(QKeySequence("Ctrl+B"));

    signalMapper->setMapping(fColorAction, foreground);
    signalMapper->setMapping(bColorAction, background);

    connect(signalMapper, SIGNAL(mapped(int)),
            this, SLOT(OnPickColor(int)));

    editMenu->addAction(fColorAction);
    editMenu->addAction(bColorAction);

    // Tool pickers

    QSignalMapper *signalMapperT = new QSignalMapper(this);

    penAction = new QAction(penIcon, QApplication::translate("MainWindow", "Pen Tool"), this);
    connect(penAction, SIGNAL(triggered()),
            signalMapperT, SLOT(map()));
    penAction->setShortcut(QKeySequence("W"));

    lineAction = new QAction(lineIcon, QApplication::translate("MainWindow", "Line Tool"), this);
    connect(lineAction, SIGNAL(triggered()),
            signalMapperT, SLOT(map()));
    lineAction->setShortcut(QKeySequence("F"));

    eraserAction = new QAction(eraserIcon, QApplication::translate("MainWindow", "Eraser"), this);
    connect(eraserAction, SIGNAL(triggered()),
            signalMapperT, SLOT(map()));
    eraserAction->setShortcut(QKeySequence("E"));

    rectAction = new QAction(rectIcon, QApplication::translate("MainWindow", "Rectangle Tool"), this);
    connect(rectAction, SIGNAL(triggered()),
            signalMapperT, SLOT(map()));
    rectAction->setShortcut(QKeySequence("R"));

    signalMapperT->setMapping(penAction, pen);
    signalMapperT->setMapping(lineAction, line);
    signalMapperT->setMapping(eraserAction, eraser);
    signalMapperT->setMapping(rectAction, rect_tool);

    connect(signalMapperT, SIGNAL(mapped(int)), this, SLOT(OnChangeTool(int)));

    // Tool dialogs
    toolsMenu = new QMenu(QApplication::translate("MainWindow", "Tools"), this);
    toolsMenu->addAction(penAction);
    toolsMenu->addAction(lineAction);
    toolsMenu->addAction(eraserAction);
    toolsMenu->addAction(rectAction);
    toolsMenu->addAction(QApplication::translate("MainWindow", "Pen Properties..."),
                     this, SLOT(OnPenDialog()));
    toolsMenu->addAction(QApplication::translate("MainWindow", "Line Properties..."),
                     this, SLOT(OnLineDialog()));
    toolsMenu->addAction(QApplication::translate("MainWindow", "Eraser Properties..."),
                     this, SLOT(OnEraserDialog()));
    toolsMenu->addAction(QApplication::translate("MainWindow", "Rectangle Properties..."),
                     this, SLOT(OnRectangleDialog()));

    // add a toolbar toggle action to the menu
    // view
    viewMenu = new QMenu(QApplication::translate("MainWindow", "View"), this);
    toggleToolbar = toolbar->toggleViewAction();
    toggleToolbar->setText(QApplication::translate("MainWindow", "Show &Toolbar"));
    toggleToolbar->setShortcut(QKeySequence("Ctrl+T"));

    viewMenu->addAction(toggleToolbar);




    // help
    helpMenu = new QMenu(QApplication::translate("MainWIndow", "&Help"), this);
    aboutAction = new QAction(aboutIcon, QApplication::translate("MainWindow", "About"), this);
    aboutAction->setMenuRole(QAction::AboutRole);
    connect(aboutAction, &QAction::triggered,
            this, &MainWindow::OnAboutDialog);
    aboutQtAction = new QAction(QIcon::fromTheme("help-about"), QApplication::translate("MainWindow", "About &Qt"), this);
    aboutQtAction->setMenuRole(QAction::AboutQtRole);
    connect(aboutQtAction, &QAction::triggered, this, &QApplication::aboutQt);

    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);


    // store the actions in QLists for convenience
    imageActions.append(newAction);
    imageActions.append(openAction);
    imageActions.append(saveAction);
    imageActions.append(undoAction);
    imageActions.append(redoAction);
    imageActions.append(clearAction);
    imageActions.append(resizeAction);
    imageActions.append(fColorAction);
    imageActions.append(bColorAction);

    toolActions.append(penAction);
    toolActions.append(lineAction);
    toolActions.append(eraserAction);
    toolActions.append(rectAction);

    // populate the menubar with menu items
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(toolsMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}
