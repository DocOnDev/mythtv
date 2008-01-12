/* ============================================================
 * File  : mythflix.h
 * Author: John Petrocik <john@petrocik.net>
 * Date  : 2005-10-28
 * Description :
 *
 * Copyright 2005 by John Petrocik

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef MYTHFLIX_H
#define MYTHFLIX_H


#include <qhttp.h>
#include <mythtv/uitypes.h>
#include <mythtv/uilistbtntype.h>
#include <mythtv/xmlparse.h>
#include <mythtv/mythdialogs.h>

#include "newsengine.h"

class QTimer;
class UIListBtnType;

class MythFlix : public MythDialog
{
    Q_OBJECT

public:

    MythFlix(MythMainWindow *parent, const char *name = 0);
    ~MythFlix();

private:

    void loadTheme();
    void loadWindow(QDomElement &element);
    void paintEvent(QPaintEvent *e);

    void updateBackground();
    void updateSitesView();
    void updateArticlesView();
    void updateInfoView();
    
    void keyPressEvent(QKeyEvent *e);
    void cursorUp(bool page=false);
    void cursorDown(bool page=false);
    void cursorRight();
    void cursorLeft();
    void displayOptions();

    void cancelRetrieve();
    void processAndShowNews(NewsSite *site);
    void InsertMovieIntoQueue(bool atTop = false);

    QString executeExternal(const QStringList& args, const QString& purpose);

    XMLParse      *m_Theme;

    UIListBtnType *m_UISites;
    UIListBtnType *m_UIArticles;

    QPixmap        m_background;

    QRect          m_SitesRect;
    QRect          m_ArticlesRect;
    QRect          m_InfoRect;
    unsigned int   m_InColumn;
    MythPopupBox  *popup;
    QString        zoom;
    QString        browser;
    bool           expectingPopup;
    NewsSite::List m_NewsSites;

private slots:
    void slotViewArticle();
    void slotViewArticleTop();
    void slotRetrieveNews();
    void slotNewsRetrieved(NewsSite* site);

    void slotSiteSelected(UIListBtnTypeItem *item);
    void slotArticleSelected(UIListBtnTypeItem *item);
    void slotShowNetFlixPage();
    void slotCancelPopup();
};

#endif /* MYTHFLIX_H */

