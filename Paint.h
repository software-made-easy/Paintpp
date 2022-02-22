#ifndef PAINT_H
#define PAINT_H

#include <QMainWindow>
#include <QList>
#include <QAction>
#include <QWidget>
#include <QSettings>

#include "dialog_windows.h"
#include "draw_area.h"
#include "toolbar.h"


class Tool;

class MainWindow: public QMainWindow
{
	Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    MainWindow(const char* name, QWidget* parent = nullptr);
    ~MainWindow();

    /** mouse event handler */
    void virtual mousePressEvent (QMouseEvent*) override;

    /** perform task when closing */
    void virtual closeEvent(QCloseEvent *event) override;

public slots:
    /** toolbar actions */
    void OnNewImage();
	void OnLoadImage();
    void OnSaveImage();
    void OnSaveAsImage();
    void OnResizeImage();
    void OnPickColor(int);
    void OnChangeTool(int);
    /** tool dialogs */
    void OnPenDialog();
    void OnLineDialog();
    void OnEraserDialog();
    void OnRectangleDialog();
    void OnAboutDialog();

private:
    void createMenuActions();
    void createMenuAndToolBar();

    /** tool dialog dispatcher */
    void openToolDialog();

    /** translations */
    void retranslate();
    void language(QString language);

    /** settings */
    QString currLang;
    QSettings *settings;
    void loadSettings();
    void saveSettings();

    /** the drawArea */
    DrawArea* drawArea;

    /** actions */
    QList<QAction*> imageActions;
    QList<QAction*> toolActions;

    /** main toolbar */
    ToolBar* toolbar;

    /** current tool */
    Tool* currentTool;

    /** dialog pointers */
    PenDialog* penDialog;
    LineDialog* lineDialog;
    EraserDialog* eraserDialog;
    RectDialog* rectDialog;

    /** Don't allow copying */
    MainWindow(const MainWindow&);
    MainWindow& operator=(const MainWindow&);

    /** Save file path */
    QString path;

    /** Actions and menus */
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *toolsMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *clearAction;
    QAction *resizeAction;
    QAction *fColorAction;
    QAction *bColorAction;
    QAction *penAction;
    QAction *lineAction;
    QAction *eraserAction;
    QAction *rectAction;
    QAction *toggleToolbar;
    QAction *helpAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
};

#endif // PAINT_H
