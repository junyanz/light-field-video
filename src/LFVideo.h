#ifndef LFVIDEO_H_
#define LFVIDEO_H_
#include "common.h"

class CLFVideo {
public:
    CLFVideo(string _dataDir);
    CLFVideo();
    ~CLFVideo();
    Mat RenderLF(int _frameId, float _alpha=0.5, float _aperture=1.0);
    void ChangeFocusVideo(string _videoPath, bool _fixFrame=true);
    float Disparity(int _frameId, PointF _pos);
    Mat DisparityMap(int _frameId);
    int NumFrames() { return  m_numFrames; }
    PointF TrackPoint(PointF _p, int _frameId);
    int FrameHeight() { return m_frameHeight; }
    int FrameWidth() { return m_frameWidth; }
    Mat SingleView(int _frameId, float _uf, float _vf);
    void WriteVideo(ImageSet _frames, string _videoPath, float fps=30.0);
    void WriteImages(ImageSet _frames, string _imgDir);

private:
    void LoadLFData(string _imgDir);
    void LoadDisparity(string _dispDir);
    void Clear();
    Mat SingleView(int _frameId, int _viewId);
    Mat CentralView(int _frameId);

    void CheckFrameId(int _frameId);
    void CheckViewId(int _viewId);
    void CheckAperture(float _aperture);
    Mat RenderLFFull(int _frameId, float _alpha=0.5);
    Mat RenderLFWeighted(int _frameId, float _alpha=0.5, float _aperture=1.0);
private:
    int m_numFrames;
    int m_numViews;
    int m_frameHeight;
    int m_frameWidth;
    int m_border;
    string m_imgDir;
    string m_dispDir;
    vector<ImageSet> m_seq;
    ImageSet m_dispSeq;
    Mat m_renderLF;
};

#endif //LFVIDEO_H_
