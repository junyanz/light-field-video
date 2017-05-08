#include "LFVideo.h"
#include "common.h"
#include "Utility.h"
#include "Timer.h"
#include "FastImage.h"

const int GRID_SIZE = 8;

CLFVideo::CLFVideo() {
    Clear();
}

CLFVideo::CLFVideo(string _dataDir) {
    Clear();
    printf("reading (%s)\n", _dataDir.c_str());
    m_imgDir = _dataDir + "lightfield/";
    m_dispDir = _dataDir + "disparity/";
    LoadLFData(m_imgDir);
    LoadDisparity(m_dispDir);
}

void CLFVideo::Clear() {
    m_numFrames = 0;
    m_numViews = 64;
    m_border = 25;
    m_frameHeight = 0;
    m_frameWidth = 0;
    FOR_u (i, m_seq.size())
        m_seq[i].clear();
    m_dispSeq.clear();
    m_seq.clear();
    m_renderLF = Mat::zeros(320, 544, CV_8UC3);
}

Mat CLFVideo::SingleView(int _frameId, int _viewId) {
    CheckFrameId(_frameId);
    CheckViewId(_viewId);
    return m_seq[_frameId][_viewId];
}

Mat CLFVideo::CentralView(int _frameId) {
//    CheckFrameId(_frameId);
    int view_id = 32;
    return SingleView(_frameId, view_id);
}

void CLFVideo::WriteVideo(ImageSet _frames, string _videoPath, float fps) {
    VideoWriter oV;
    Size s = _frames[0].size();
    oV.open(_videoPath, CV_FOURCC('M','J','P','G'), fps, s, true);
    if (!oV.isOpened())
        DEBUG_ERROR("cannot create video %s", _videoPath.c_str());
    FOR_u (i, _frames.size()) {
        oV << _frames[i];
    }
}

void CLFVideo::WriteImages(ImageSet _frames, string _imgDir) {
    utility::CUtility::mkdirs(_imgDir);
    FOR_u (i, _frames.size()) {
        char buf[50];
        sprintf(buf, "frame_%4.4d.png", int(i));
        imwrite(_imgDir+string(buf), _frames[i]);
    }
}

void CLFVideo::CheckFrameId(int _frameId) {
    assert(_frameId >= 0 && _frameId <= m_numFrames-1);
}

void CLFVideo::CheckViewId(int _viewId) {
    assert(_viewId >=0 && _viewId <= m_numViews-1);
}

void CLFVideo::CheckAperture(float _aperture) {
    assert(_aperture >=0 && _aperture <= 1.0);
}

Mat CLFVideo::RenderLFFull(int _frameId, float _alpha) {
    ImageSet views_t(m_numViews);
    ImageSet views = m_seq[_frameId];
    Size sz(views[0].cols, views[0].rows);
    double alpha = double(_alpha);
//    int GRID_SIZE = 8;
    #pragma omp parallel for
    FOR (n, m_numViews) {
        Mat H = Mat::zeros(2, 3, CV_64FC1);
        int u = n % GRID_SIZE;
        int v = (n-u) / GRID_SIZE;
        double deltaX = (u-3) * alpha/4.0;
        double deltaY = (v-3) * alpha/4.0;
        H.at<double>(0, 0) = 1.0; H.at<double>(1, 1) = 1.0;
        H.at<double>(0, 2) = -deltaX; H.at<double>(1, 2) = -deltaY;
        warpAffine(views[n], views_t[n], H, sz, 1, 0, Scalar(0, 0, 0));
    }


    Mat tmpF = FAST::CFastImage::FastAddImages(views_t)/m_numViews;
    tmpF.convertTo(m_renderLF, CV_8UC3);
    return m_renderLF.clone();
}

Mat CLFVideo::RenderLFWeighted(int _frameId, float _alpha, float _aperture) {
    ImageSet views_t(m_numViews);
    ImageSet views = m_seq[_frameId];
    Size sz(views[0].cols, views[0].rows);
    double alpha = double(_alpha);
    float C= (GRID_SIZE-1)/2.0;
    // compute weights
    vectorf weights;
    float sigma = (1 - _aperture) * 4;
    FOR (n, m_numViews) {
        int u = n % GRID_SIZE;
        int v = (n-u) / GRID_SIZE;
        float dist = ((u - C) * (u - C) + (v - C) * (v -C)) / (C*C);
        float weight = exp(-dist * sigma);
        weights.push_back(weight);
    }

    float weight_sum = vecSum(weights);
    FOR (n, m_numViews)
        weights[n] /= weight_sum;

    #pragma omp parallel for
    FOR (n, m_numViews) {
        Mat H = Mat::zeros(2, 3, CV_64FC1);
        int u = n % GRID_SIZE;
        int v = (n-u) / GRID_SIZE;
        double deltaX = (u-3) * alpha/4.0;
        double deltaY = (v-3) * alpha/4.0;
        H.at<double>(0, 0) = 1.0; H.at<double>(1, 1) = 1.0;
        H.at<double>(0, 2) = -deltaX; H.at<double>(1, 2) = -deltaY;
        warpAffine(views[n], views_t[n], H, sz, 1, 0, Scalar(0, 0, 0));
        views_t[n] = views_t[n] * weights[n];
    }


    Mat tmpF = FAST::CFastImage::FastAddImages(views_t);
    tmpF.convertTo(m_renderLF, CV_8UC3);
    return m_renderLF.clone();
}

Mat CLFVideo::RenderLF(int _frameId, float _alpha, float _aperture) {
    CheckFrameId(_frameId);
    CheckAperture(_aperture);
//    DEBUG_INFO("aperture = %3.3f", _aperture);

    if (_aperture < 1.0)
        return RenderLFWeighted(_frameId, _alpha, _aperture);
    else
        return RenderLFFull(_frameId, _alpha);
}

void CLFVideo::ChangeFocusVideo(string _videoPath, bool _fixFrame) {
    int N = 117;
    float alpha_0 = -3;
    float alpha_1 = 3;
    float alpha_step = (alpha_1 - alpha_0) / N;
    ImageSet results;
    FOR (n, N) {
        float alpha_n = alpha_0 + alpha_step * n;
        Mat img;
        if (_fixFrame)
            img = RenderLF(0, alpha_n, 3.0);
        else
            img = RenderLF(min(n, m_numFrames-1), alpha_n, 3.0);
        results.push_back(img);
    }
    WriteVideo(results, _videoPath);
//    WriteImages(results, _videoPath);
}

float CLFVideo::Disparity(int _frameId, PointF _pos) {
    CheckFrameId(_frameId);
    Mat disp = m_dispSeq[_frameId];
    float max_disp = 10.0;
    float min_disp = -10.0;
    float alpha = float(disp.at<uchar>(int(_pos.y), int(_pos.x))) / 255.0 * (max_disp-min_disp)+min_disp;
    return alpha;
}

Mat CLFVideo::DisparityMap(int _frameId) {
    CheckFrameId(_frameId);
    return m_dispSeq[_frameId];
}

PointF CLFVideo::TrackPoint(PointF _p, int _frameId) {
    CheckFrameId(_frameId);
    Mat prevIm_u, nextIm_u;
    Mat prevIm = CentralView(_frameId);
//    cvtColor(prevIm, prevGray, CV_BGR2GRAY);
    prevIm.convertTo(prevIm_u, CV_8U);
    Mat nextIm = CentralView(_frameId+1);
//    cvtColor(nextIm, nextGray, CV_BGR2GRAY);
    nextIm.convertTo(nextIm_u, CV_8U);
    vector<PointF> prevPnts;
    prevPnts.push_back(_p);
    vector<PointF> nextPnts;
    vector<uchar> featuresFound;
    Mat err;
//    cout << _p << endl;
//    SHOW_IMG_WAIT(prevGray_u);
//    SHOW_IMG_WAIT(nextGray_u);
    int win_size = 41;
    calcOpticalFlowPyrLK(prevIm_u, nextIm_u, prevPnts, nextPnts,
                         featuresFound, err, Size(win_size, win_size));
    PointF outP = nextPnts[0];
    DEBUG_INFO("tracking point (%3.3f, %3.3f)", outP.x, outP.y);
    return outP;
}

Mat CLFVideo::SingleView(int _frameId, float _uf, float _vf) {
    int u = round(_uf * (GRID_SIZE-1));
    int v = round(_vf * (GRID_SIZE-1));
    printf("(u, v) = (%d, %d)\n", u, v);
    fflush(stdout);
    int viewId = u + v * GRID_SIZE;
    Mat img_f =  SingleView(_frameId, viewId);
    Mat img;
    img_f.convertTo(img, CV_8UC3);
    return img;
}

CLFVideo::~CLFVideo() {
    Clear();
}


void CLFVideo::LoadLFData(string _imgDir) {
    CTimer timer("loading light field data");
    vectorString imgNames;
    vectorString imgExts;
    utility::CUtility::FindImageFiles(_imgDir, imgNames, imgExts);
    m_seq.clear();
//    m_seqPad.clear();
    int H = 8;
    int W = 8;
//    int height = 320;
//    int width = 544;
    m_numFrames = (int)imgNames.size();

    FOR_u (i, imgNames.size()) {
        string imgPath = _imgDir + imgNames[i] + imgExts[i];
        printf("parsing image %s\n", imgPath.c_str());
        Mat largeIm = imread(imgPath);
        int height = largeIm.rows / H;
        int width = largeIm.cols / W;
        m_frameHeight = height;
        m_frameWidth = width;
//        ImageSet viewsPad;
        ImageSet views;

        FOR (h, H) {
            FOR (w, W) {
                Mat view = largeIm(Rect(w*width, h*height, width, height));
                Mat view_f;
                view.convertTo(view_f, CV_32FC3);
//                SHOW_IMG_WAIT(view_f);
//                views.push_back(view_f);
//                Mat view_pad;
//                copyMakeBorder(view_f, view_pad, m_border, m_border, m_border, m_border, BORDER_CONSTANT, Scalar(0.0, 0.0, 0.0));
                views.push_back(view_f);
//                SHOW_IMG_WAIT(view_pad);
//                viewsPad.push_back(view_pad);
            }
        }
        m_seq.push_back(views);
//        m_seqPad.push_back(viewsPad);
    }
}

void CLFVideo::LoadDisparity(string _dispDir) {
    CTimer timer("loading disparity data");
    vectorString imgNames;
    vectorString imgExts;
    utility::CUtility::FindImageFiles(_dispDir, imgNames, imgExts);
    m_dispSeq.clear();

    FOR_u (i, imgNames.size()) {
        string imgPath = _dispDir + imgNames[i] + imgExts[i];
        printf("loading image %s\n", imgPath.c_str());
        Mat disp = imread(imgPath, 0);
//        cout << disp.channels() << endl;
        m_dispSeq.push_back(disp);
    }
}
