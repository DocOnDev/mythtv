#ifndef PARSE_H
#define PARSE_H

#include <vector>
using namespace std;

#include <QString>
#include <QList>
#include <QObject>
#include <QProcess>
#include <QDomDocument>
#include <QDateTime>
#include <QPair>
#include <QMap>
#include <QVariant>

class QDomDocument;

   /** Describes an enclosure associated with an item.
    */
   struct Enclosure
   {
       QString URL_;
       QString Type_;
       qint64 Length_;
       QString Lang_;
   };

   struct MRSSThumbnail
   {
       QString URL_;
       int Width_;
       int Height_;
       QString Time_;
   };

   struct MRSSCredit
   {
       QString Role_;
       QString Who_;
   };

   struct MRSSComment
   {
       QString Type_;
       QString Comment_;
   };

   struct MRSSPeerLink
   {
       QString Type_;
       QString Link_;
   };

   struct MRSSScene
   {
       QString Title_;
       QString Description_;
       QString StartTime_;
       QString EndTime_;
   };

   struct MRSSEntry
   {
       QString URL_;
       qint64 Size_;
       QString Type_;
       QString Medium_;
       bool IsDefault_;
       QString Expression_;
       int Bitrate_;
       double Framerate_;
       double SamplingRate_;
       int Channels_;
       int Duration_;
       int Width_;
       int Height_;
       QString Lang_;
       int Group_;
       QString Rating_;
       QString RatingScheme_;
       QString Title_;
       QString Description_;
       QString Keywords_;
       QString CopyrightURL_;
       QString CopyrightText_;
       int RatingAverage_;
       int RatingCount_;
       int RatingMin_;
       int RatingMax_;
       int Views_;
       int Favs_;
       QString Tags_;
       QList<MRSSThumbnail> Thumbnails_;
       QList<MRSSCredit> Credits_;
       QList<MRSSComment> Comments_;
       QList<MRSSPeerLink> PeerLinks_;
       QList<MRSSScene> Scenes_;
   };

class ResultVideo
{

public:

    typedef QList<ResultVideo *> resultList;
    typedef vector<ResultVideo> List;

    ResultVideo(const QString& title, const QString& desc,
              const QString& URL, const QString& thumbnail,
              const QString& mediaURL, const QString& author,
              const QDateTime& date, const QString& time, 
              const QString& rating, const off_t& filesize,
              const QString& player, const QStringList& playerargs,
              const QString& download, const QStringList& downloadargs,
              const uint& width, const uint& height, const QString& language,
              const bool& downloadable);
    ResultVideo();
    ~ResultVideo();

    const QString& GetTitle() const { return m_title; }
    const QString& GetDescription() const { return m_desc; }
    const QString& GetURL() const { return m_URL; }
    const QString& GetThumbnail() const { return m_thumbnail; }
    const QString& GetMediaURL() const { return m_mediaURL; }
    const QString& GetAuthor() const { return m_enclosure; }
    const QDateTime& GetDate() const { return m_date; }
    const QString& GetTime() const { return m_time; }
    const QString& GetRating() const { return m_rating; }
    const off_t& GetFilesize() const { return m_filesize; }
    const QString& GetPlayer() const { return m_player; }
    const QStringList& GetPlayerArguments() const { return m_playerargs; }
    const QString& GetDownloader() const { return m_download; }
    const QStringList& GetDownloaderArguments() const { return m_downloadargs; }
    const uint& GetWidth() const { return m_width; }
    const uint& GetHeight() const { return m_height; }
    const QString& GetLanguage() const { return m_language; }
    const bool& GetDownloadable() const { return m_downloadable; }

private:
    QString   m_title;
    QString   m_desc;
    QString   m_URL;
    QString   m_thumbnail;
    QString   m_mediaURL;
    QString   m_enclosure;
    QDateTime m_date;
    QString   m_time;
    QString   m_rating;
    off_t     m_filesize;
    QString   m_player;
    QStringList  m_playerargs;
    QString   m_download;
    QStringList  m_downloadargs;
    uint      m_width;
    uint      m_height;
    QString   m_language;
    bool      m_downloadable;
};
Q_DECLARE_METATYPE(ResultVideo*)

class Parse : public QObject
{
    Q_OBJECT
    friend class MRSSParser;
public:

    Parse();
    virtual ~Parse();

    ResultVideo::resultList parseRSS(QDomDocument domDoc);
    ResultVideo* ParseItem(const QDomElement& item) const;

    QString GetLink(const QDomElement&) const;
    QString GetAuthor(const QDomElement&) const;
    QString GetCommentsRSS(const QDomElement&) const;
    QString GetCommentsLink(const QDomElement&) const;
    QDateTime GetDCDateTime(const QDomElement&) const;
    QDateTime FromRFC3339(const QString&) const;
    QDateTime RFC822TimeToQDateTime (const QString&) const;
    int GetNumComments (const QDomElement&) const;
    QStringList GetAllCategories (const QDomElement&) const;
    QPair<double, double> GetGeoPoint (const QDomElement&) const;
    QList<MRSSEntry> GetMediaRSS (const QDomElement&) const;
    QList<Enclosure> GetEnclosures(const QDomElement& entry) const;
    static QString UnescapeHTML (const QString&);

private:

    QMap<QString, int> TimezoneOffsets_;

protected:
    static const QString DC_;
    static const QString WFW_;
    static const QString Atom_;
    static const QString RDF_;
    static const QString Slash_;
    static const QString Enc_;
    static const QString ITunes_;
    static const QString GeoRSSSimple_;
    static const QString GeoRSSW3_;
    static const QString MediaRSS_;

};

#endif