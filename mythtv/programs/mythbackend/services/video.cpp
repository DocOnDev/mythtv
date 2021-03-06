//////////////////////////////////////////////////////////////////////////////
// Program Name: video.cpp
// Created     : Apr. 21, 2011
//
// Copyright (c) 2011 Robert McNamara <rmcnamara@mythtv.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or at your option any later version of the LGPL.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////

#include <QList>

#include <math.h>

#include "video.h"

#include "videometadata.h"

#include "compat.h"
#include "mythversion.h"
#include "mythcorecontext.h"

#include "serviceUtil.h"

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

DTC::VideoMetadataInfoList* Video::GetVideos( bool bDescending,
                                              int nStartIndex,
                                              int nCount       )
{
    VideoMetadataListManager::metadata_list videolist;
    VideoMetadataListManager::loadAllFromDatabase(videolist);

    std::vector<VideoMetadataListManager::VideoMetadataPtr> videos(videolist.begin(), videolist.end());

    // ----------------------------------------------------------------------
    // Build Response
    // ----------------------------------------------------------------------

    DTC::VideoMetadataInfoList *pVideoMetadataInfos = new DTC::VideoMetadataInfoList();

    nStartIndex   = min( nStartIndex, (int)videos.size() );
    nCount        = (nCount > 0) ? min( nCount, (int)videos.size() ) : videos.size();
    int nEndIndex = min((nStartIndex + nCount), (int)videos.size() );

    for( int n = nStartIndex; n < nEndIndex; n++ )
    {
        DTC::VideoMetadataInfo *pVideoMetadataInfo = pVideoMetadataInfos->AddNewVideoMetadataInfo();

        VideoMetadataListManager::VideoMetadataPtr metadata = videos[n];

        if (metadata)
        {
            pVideoMetadataInfo->setId(metadata->GetID());
            pVideoMetadataInfo->setTitle(metadata->GetTitle());
            pVideoMetadataInfo->setSubTitle(metadata->GetSubtitle());
            pVideoMetadataInfo->setTagline(metadata->GetTagline());
            pVideoMetadataInfo->setDirector(metadata->GetDirector());
            pVideoMetadataInfo->setStudio(metadata->GetStudio());
            pVideoMetadataInfo->setDescription(metadata->GetPlot());
            pVideoMetadataInfo->setCertification(metadata->GetRating());
            pVideoMetadataInfo->setInetRef(metadata->GetInetRef());
            pVideoMetadataInfo->setHomePage(metadata->GetHomepage());
            pVideoMetadataInfo->setReleaseDate(QDateTime(metadata->GetReleaseDate()));
            pVideoMetadataInfo->setAddDate(QDateTime(metadata->GetInsertdate()));
            pVideoMetadataInfo->setUserRating(metadata->GetUserRating());
            pVideoMetadataInfo->setLength(metadata->GetLength());
            pVideoMetadataInfo->setSeason(metadata->GetSeason());
            pVideoMetadataInfo->setEpisode(metadata->GetEpisode());
            pVideoMetadataInfo->setParentalLevel(metadata->GetShowLevel());
            pVideoMetadataInfo->setVisible(metadata->GetBrowse());
            pVideoMetadataInfo->setWatched(metadata->GetWatched());
            pVideoMetadataInfo->setProcessed(metadata->GetProcessed());
            pVideoMetadataInfo->setFileName(metadata->GetFilename());
            pVideoMetadataInfo->setHash(metadata->GetHash());
            pVideoMetadataInfo->setHost(metadata->GetHost());
            pVideoMetadataInfo->setCoverart(metadata->GetCoverFile());
            pVideoMetadataInfo->setFanart(metadata->GetFanart());
            pVideoMetadataInfo->setBanner(metadata->GetBanner());
            pVideoMetadataInfo->setScreenshot(metadata->GetScreenshot());
            pVideoMetadataInfo->setTrailer(metadata->GetTrailer());
        }
    }

    int curPage = 0, totalPages = 0;
    if (nCount == 0)
        totalPages = 1;
    else
        totalPages = (int)ceil((float)videos.size() / nCount);

    if (totalPages == 1)
        curPage = 1;
    else
    {
        curPage = (int)ceil((float)nStartIndex / nCount) + 1;
    }

    pVideoMetadataInfos->setStartIndex    ( nStartIndex     );
    pVideoMetadataInfos->setCount         ( nCount          );
    pVideoMetadataInfos->setCurrentPage   ( curPage         );
    pVideoMetadataInfos->setTotalPages    ( totalPages      );
    pVideoMetadataInfos->setTotalAvailable( videos.size()   );
    pVideoMetadataInfos->setAsOf          ( QDateTime::currentDateTime() );
    pVideoMetadataInfos->setVersion       ( MYTH_BINARY_VERSION );
    pVideoMetadataInfos->setProtoVer      ( MYTH_PROTO_VERSION  );

    return pVideoMetadataInfos;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

DTC::VideoMetadataInfo* Video::GetVideoById( int Id )
{
    VideoMetadataListManager::metadata_list videolist;
    VideoMetadataListManager::loadAllFromDatabase(videolist);
    VideoMetadataListManager *mlm = new VideoMetadataListManager();
    mlm->setList(videolist);
    VideoMetadataListManager::VideoMetadataPtr metadata = mlm->byID(Id);

    if ( !metadata )
        throw( QString( "No metadata found for selected ID!." ));

    DTC::VideoMetadataInfo *pVideoMetadataInfo = GetInfoFromMetadata(metadata);

    delete mlm;

    return pVideoMetadataInfo;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

DTC::VideoMetadataInfo* Video::GetVideoByFilename( const QString &Filename )
{
    VideoMetadataListManager::metadata_list videolist;
    VideoMetadataListManager::loadAllFromDatabase(videolist);
    VideoMetadataListManager *mlm = new VideoMetadataListManager();
    mlm->setList(videolist);
    VideoMetadataListManager::VideoMetadataPtr metadata = mlm->byFilename(Filename);

    if ( !metadata )
        throw( QString( "No metadata found for selected filename!." ));

    DTC::VideoMetadataInfo *pVideoMetadataInfo = GetInfoFromMetadata(metadata);

    delete mlm;

    return pVideoMetadataInfo;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

bool Video::RemoveVideoFromDB( int Id )
{
    bool bResult = false;

    VideoMetadataListManager::metadata_list videolist;
    VideoMetadataListManager::loadAllFromDatabase(videolist);
    VideoMetadataListManager *mlm = new VideoMetadataListManager();
    mlm->setList(videolist);
    VideoMetadataListManager::VideoMetadataPtr metadata = mlm->byID(Id);

    if (metadata)
        bResult = metadata->DeleteFromDatabase();

    delete mlm;

    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

DTC::VideoMetadataInfo* Video::GetInfoFromMetadata(
                      VideoMetadataListManager::VideoMetadataPtr metadata)
{
    DTC::VideoMetadataInfo *pVideoMetadataInfo = new DTC::VideoMetadataInfo();

    if (metadata)
    {
        pVideoMetadataInfo->setId(metadata->GetID());
        pVideoMetadataInfo->setTitle(metadata->GetTitle());
        pVideoMetadataInfo->setSubTitle(metadata->GetSubtitle());
        pVideoMetadataInfo->setTagline(metadata->GetTagline());
        pVideoMetadataInfo->setDirector(metadata->GetDirector());
        pVideoMetadataInfo->setStudio(metadata->GetStudio());
        pVideoMetadataInfo->setDescription(metadata->GetPlot());
        pVideoMetadataInfo->setCertification(metadata->GetRating());
        pVideoMetadataInfo->setInetRef(metadata->GetInetRef());
        pVideoMetadataInfo->setHomePage(metadata->GetHomepage());
        pVideoMetadataInfo->setReleaseDate(QDateTime(metadata->GetReleaseDate()));
        pVideoMetadataInfo->setAddDate(QDateTime(metadata->GetInsertdate()));
        pVideoMetadataInfo->setUserRating(metadata->GetUserRating());
        pVideoMetadataInfo->setLength(metadata->GetLength());
        pVideoMetadataInfo->setSeason(metadata->GetSeason());
        pVideoMetadataInfo->setEpisode(metadata->GetEpisode());
        pVideoMetadataInfo->setParentalLevel(metadata->GetShowLevel());
        pVideoMetadataInfo->setVisible(metadata->GetBrowse());
        pVideoMetadataInfo->setWatched(metadata->GetWatched());
        pVideoMetadataInfo->setProcessed(metadata->GetProcessed());
        pVideoMetadataInfo->setFileName(metadata->GetFilename());
        pVideoMetadataInfo->setHash(metadata->GetHash());
        pVideoMetadataInfo->setHost(metadata->GetHost());
        pVideoMetadataInfo->setCoverart(metadata->GetCoverFile());
        pVideoMetadataInfo->setFanart(metadata->GetFanart());
        pVideoMetadataInfo->setBanner(metadata->GetBanner());
        pVideoMetadataInfo->setScreenshot(metadata->GetScreenshot());
        pVideoMetadataInfo->setTrailer(metadata->GetTrailer());
    }

    return pVideoMetadataInfo;
}
