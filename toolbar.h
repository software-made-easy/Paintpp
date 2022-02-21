#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include <QAction>
#include <QList>

#include "constants.h"


class QWidget;

class ToolBar : public QToolBar
{
public:
    ToolBar(QWidget *parent = nullptr);
    ToolBar(const QList<QAction*> &imageActions,
            const QList<QAction*> &toolActions,
            QWidget *parent = nullptr);
    void showActions(const QList<QAction*> &imageActions,
                     const QList<QAction*> &toolActions);

private:
    /** Don't allow copying */
    ToolBar(const ToolBar&);
    ToolBar& operator=(const ToolBar&);
};

#endif // TOOLBAR_H
