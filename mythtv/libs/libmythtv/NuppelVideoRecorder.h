#ifndef NUPPELVIDEORECORDER
#define NUPPELVIDEORECORDER

// C headers
#include <stdint.h>

#include <sys/time.h>
#include <time.h>
#include <QThread>
#ifdef MMX
#undef MMX
#define MMXBLAH
#endif
#include <lame/lame.h>
#ifdef MMXBLAH
#define MMX
#endif

#undef HAVE_AV_CONFIG_H
extern "C" {
#include "libavcodec/avcodec.h"
}

// C++ std headers
#include <vector>
using namespace std;

// Qt headers
#include <QString>

// MythTV headers
#include "recorderbase.h"
#include "format.h"
#include "cc608decoder.h"
#include "filter.h"
#include "minilzo.h"

#include "mythtvexp.h"

struct video_audio;
struct VBIData;
struct cc;
class RTjpeg;
class RingBuffer;
class ChannelBase;
class FilterManager;
class FilterChain;
class AudioInput;

class NuppelVideoRecorder;

class NVRWriteThread : public QThread
{
    Q_OBJECT
  public:
    NVRWriteThread() : m_parent(NULL) {}
    void run(void);
    void SetParent(NuppelVideoRecorder *parent) { m_parent = parent; }
  private:
    NuppelVideoRecorder *m_parent;
};

class NVRAudioThread : public QThread
{
    Q_OBJECT
  public:
    NVRAudioThread() : m_parent(NULL) {}
    void run(void);
    void SetParent(NuppelVideoRecorder *parent) { m_parent = parent; }
  private:
    NuppelVideoRecorder *m_parent;
};

class NVRVbiThread : public QThread
{
    Q_OBJECT
  public:
    NVRVbiThread() : m_parent(NULL) {}
    void run(void);
    void SetParent(NuppelVideoRecorder *parent) { m_parent = parent; }
  private:
    NuppelVideoRecorder *m_parent;
};

class MTV_PUBLIC NuppelVideoRecorder : public RecorderBase, public CC608Input
{
    friend class NVRWriteThread;
    friend class NVRAudioThread;
    friend class NVRVbiThread;
 public:
    NuppelVideoRecorder(TVRec *rec, ChannelBase *channel);
   ~NuppelVideoRecorder();

    void SetOption(const QString &name, int value);
    void SetOption(const QString &name, const QString &value)
                  { RecorderBase::SetOption(name, value); }

    void SetOptionsFromProfile(RecordingProfile *profile,
                               const QString &videodev,
                               const QString &audiodev,
                               const QString &vbidev);
 
    void Initialize(void);
    void StartRecording(void);
    void StopRecording(void); 
    
    void Pause(bool clear = true);
    void Unpause(void);
    bool IsPaused(void) const;
 
    bool IsRecording(void);
    bool IsErrored(void);

    long long GetFramesWritten(void); 

    bool Open(void);
    int GetVideoFd(void);
    void Reset(void);

    void SetVideoFilters(QString &filters);
    void SetTranscoding(bool value) { transcoding = value; };

    void SetNextRecording(const ProgramInfo*, RingBuffer*);
    void ResetForNewFile(void);
    void FinishRecording(void);
    void StartNewFile(void);

    // reencode stuff
    void StreamAllocate(void);
    void WriteHeader(void);
    void WriteSeekTable(void);
    void WriteKeyFrameAdjustTable(
        const vector<struct kfatable_entry> &kfa_table);
    void UpdateSeekTable(int frame_num, long offset = 0);

    bool SetupAVCodecVideo(void);
    void SetupRTjpeg(void);
    int AudioInit(bool skipdevice = false);
    void SetVideoAspect(float newAspect) {video_aspect = newAspect; };
    void WriteVideo(VideoFrame *frame, bool skipsync = false, 
                    bool forcekey = false);
    void WriteAudio(unsigned char *buf, int fnum, int timecode);
    void WriteText(unsigned char *buf, int len, int timecode, int pagenr);

    void SetNewVideoParams(double newaspect);

 protected:
    void doWriteThread(void);
    void doAudioThread(void);
    void doVbiThread(void);

 private:
    inline void WriteFrameheader(rtframeheader *fh);

    void WriteFileHeader(void);

    void InitBuffers(void);
    void InitFilters(void);   
    void ResizeVideoBuffers(void);

    bool MJPEGInit(void);
 
    bool SpawnChildren(void);
    void KillChildren(void);
    
    void BufferIt(unsigned char *buf, int len = -1, bool forcekey = false);
    
    int CreateNuppelFile(void);

    void ProbeV4L2(void);
    bool SetFormatV4L2(void);
    void DoV4L(void);
    void DoV4L2(void);
    void DoMJPEG(void);

    void FormatTeletextSubtitles(struct VBIData *vbidata);
    void FormatCC(struct cc *cc);
    void AddTextData(unsigned char *buf, int len,
                     int64_t timecode, char type);
    void UpdateResolutions(void);
    
    bool encoding;
    
    int fd; // v4l input file handle
    signed char *strm;
    unsigned int lf, tf;
    int M1, M2, Q;
    int width, height;
    int pip_mode;
    int pid, pid2;
    int inputchannel;
    int compression;
    int compressaudio;
    AudioInput *audio_device;
    unsigned long long audiobytes;
    int audio_channels; 
    int audio_bits;
    int audio_bytes_per_sample;
    int audio_samplerate; // rate we request from sounddevice
    int effectivedsp; // actual measured rate

    int usebttv;
    float video_aspect;

    bool transcoding;

    int mp3quality;
    char *mp3buf;
    int mp3buf_size;
    lame_global_flags *gf;

    RTjpeg *rtjc;

#define OUT_LEN (1024*1024 + 1024*1024 / 64 + 16 + 3)    
    lzo_byte out[OUT_LEN];
#define HEAP_ALLOC(var,size) \
    long __LZO_MMODEL var [ ((size) + (sizeof(long) - 1)) / sizeof(long) ]    
    HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);

    vector<struct vidbuffertype *> videobuffer;
    vector<struct audbuffertype *> audiobuffer;
    vector<struct txtbuffertype *> textbuffer;

    int act_video_encode;
    int act_video_buffer;

    int act_audio_encode;
    int act_audio_buffer;
    long long act_audio_sample;
   
    int act_text_encode;
    int act_text_buffer;
 
    int video_buffer_count;
    int audio_buffer_count;
    int text_buffer_count;

    long video_buffer_size;
    long audio_buffer_size;
    long text_buffer_size;

    struct timeval stm;
    struct timezone tzone;

    bool childrenLive;

    NVRWriteThread WriteThread;
    NVRAudioThread AudioThread;
    NVRVbiThread   VbiThread;

    bool recording;
    bool errored;

    int keyframedist;
    vector<struct seektable_entry> *seektable;
    long long lastPositionMapPos;

    long long extendeddataOffset;

    long long framesWritten;

    bool livetv;
    bool writepaused;
    bool audiopaused;
    bool mainpaused;

    double framerate_multiplier;
    double height_multiplier;

    int last_block;
    int firsttc;
    long int oldtc;
    int startnum;
    int frameofgop;
    int lasttimecode;
    int audio_behind;
    
    bool useavcodec;

    AVCodec *mpa_vidcodec;
    AVCodecContext *mpa_vidctx;
    AVFrame mpa_picture;

    int targetbitrate;
    int scalebitrate;
    int maxquality;
    int minquality;
    int qualdiff;
    int mp4opts;
    int mb_decision;
    /// Number of threads to use for MPEG-2 and MPEG-4 encoding
    int encoding_thread_count;

    QString videoFilterList;
    FilterChain *videoFilters;
    FilterManager *FiltMan;

    VideoFrameType inpixfmt;
    PixelFormat picture_format;
    uint32_t v4l2_pixelformat;
    int w_out;
    int h_out;

    bool hardware_encode;
    int hmjpg_quality;
    int hmjpg_hdecimation;
    int hmjpg_vdecimation;
    int hmjpg_maxw;

    bool cleartimeonpause;

    bool usingv4l2;
    int channelfd;

    long long prev_bframe_save_pos;

    ChannelBase *channelObj;

    bool skip_btaudio;

    bool correct_bttv;

    int volume;

    CC608Decoder *ccd;

    bool go7007;
    bool resetcapture;
};

#endif
