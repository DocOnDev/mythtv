#ifndef GRABBERMANAGER_H
#define GRABBERMANAGER_H

#include <QObject>
#include <QDomElement>
#include <QThread>
#include <QMetaType>
#include <QMutex>
#include <QTimer>
#include <QProcess>

#include "parse.h"

class GrabberScript : public QThread
{

    Q_OBJECT

  public:

    GrabberScript(const QString& title,
                  const QString& image,
                  const bool& search,
                  const bool& tree,
                  const QString& commandline);
    ~GrabberScript();

    const QString& GetTitle() const { return m_title; }
    const QString& GetImage() const { return m_image; }
    const bool& GetSearch() const { return m_search; }
    const bool& GetTree() const { return m_tree; }
    const QString& GetCommandline() const { return m_commandline; }

    virtual void run(void);

    typedef QList<GrabberScript *> scriptList;

  signals:

    void finished(void);

  public slots:

    void updateTreeFinished(void);

  private:

    void parseDBTree(const QString &feedtitle, const QString &path,
                     const QString &pathThumb, QDomElement& domElem);
    mutable QMutex m_lock;

    QString   m_title;
    QString   m_image;
    bool      m_rss;
    bool      m_search;
    bool      m_tree;
    QString   m_commandline;
    QProcess *m_getTree;
};
Q_DECLARE_METATYPE(GrabberScript *);

class GrabberManager : public QObject
{
    Q_OBJECT

  public:
    GrabberManager();
    ~GrabberManager();
    void startTimer();
    void stopTimer();
    void doUpdate();

  signals:
    void finished(void);

  private slots:
    void timeout(void);
    void grabberFinished(void);

  private:

    mutable QMutex                 m_lock;
    QTimer                        *m_timer;
    GrabberScript::scriptList      m_scripts;
    uint                           m_updateFreq;
    uint                           m_runningCount;
};

#endif