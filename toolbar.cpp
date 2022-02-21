#include "Paint.h"
#include "draw_area.h"

#if defined(Q_OS_BLACKBERRY) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WP)
#define Q_OS_MOBILE
#else
#define Q_OS_DESKTOP
#endif


/**
 * @brief ToolBar::ToolBar - Wrapper class for QToolBar.
 *                           construct a ToolBar with icons & actions.
 */
ToolBar::ToolBar(const QList<QAction*> &imageActions,
                 const QList<QAction*> &toolActions,
                 QWidget *parent)
    : QToolBar(parent)
{
    addActions(imageActions);
    addSeparator();
    addActions(toolActions);

#ifdef Q_OS_DESKTOP
    setMovable(true);
#else
    setMovable(false);
    setIconSize(QSize(128, 128));
#endif
}

ToolBar::ToolBar(QWidget *parent)
    : QToolBar(parent)
{
#ifdef Q_OS_DESKTOP
    setMovable(true);
#else
    setMovable(false);
    setIconSize(QSize(128, 128));
#endif
}

void ToolBar::showActions(const QList<QAction *> &imageActions, const QList<QAction *> &toolActions)
{
    addActions(imageActions);
    addSeparator();
    addActions(toolActions);
}
