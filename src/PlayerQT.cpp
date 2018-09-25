#include "PlayerQT.h"
#include "Timer.h"
#include <QtWidgets>
#include <QLayout>
#include <QPainter>
#include <QPainterPath>
#include "LFVideo.h"

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif
void qSleep(int ms)
{
    assert(ms > 0);

#ifdef Q_OS_WIN
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

CPlayerView::CPlayerView(QWidget *parent, CLFVideo* _lfv, float _s)
    : ArthurFrame(parent) {
    Clear();
    setAttribute(Qt::WA_MouseTracking);
    m_scale = _s;
    m_aperture = 1.0;
    m_isTrack = false;
    m_showFocus = true;
    m_alpha = 0.0;
    m_fps = 20.0;
    m_lfv = _lfv;
    m_numFrames = _lfv->NumFrames();
    m_frameH = _lfv->FrameHeight();
    m_frameW = _lfv->FrameWidth();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
}


void CPlayerView::mousePressEvent(QMouseEvent* e) {

    Qt::MouseButtons mouseButtons = e->buttons();
    if (mouseButtons == Qt::LeftButton) {
        m_cursor= e->pos();
        m_changeFocus = true;
        updateFocus(m_cursor, true);
    }
    if (mouseButtons == Qt::RightButton) {
        m_changeView = true;
        m_oldCursor = e->pos();
    }
}

void CPlayerView::mouseMoveEvent(QMouseEvent *e) {
    if (m_changeFocus) {
        m_cursor = e->pos();
        if (!m_isPlay)
            updateFocus(m_cursor, true);
    }

    if (m_changeView) {
        m_cursor = e->pos();
        if (!m_isPlay)
            update();
    }
}

void CPlayerView::mouseReleaseEvent(QMouseEvent *e) {
    m_changeFocus = false;
    m_changeView = false;
}

void CPlayerView::keyPressEvent( QKeyEvent* e ) {
    switch (e->key()) {
    case Qt::Key_Q:
        quit();
        break;
    case Qt::Key_R:
        reset();
        break;
    case Qt::Key_F:
        showFocus();
        break;
    case Qt::Key_P:
        play();
    default:
        DEBUG_WARN("wrong input key\n");
        break;
    }
}

void CPlayerView::resizeEvent(QResizeEvent* e) {
    ArthurFrame::resizeEvent(e);
}

void CPlayerView::paint(QPainter* _p) {
    double max_t = 1000.0 / m_fps;
    CTimer* timer = NULL;
    if (m_isPlay)
        timer = new CTimer("fps", false);

    _p->save();
    _p->setRenderHint(QPainter::Antialiasing);
    _p->setRenderHint(QPainter::SmoothPixmapTransform);
    Mat tmp;
    if (m_changeView) {
        float dx = (m_cursor.x() - m_oldCursor.x()) / m_scale;
        float dy = (m_cursor.y() - m_oldCursor.y()) / m_scale;
        float win_size = 30.0;
        float uf = std::min(1.0f, std::max(0.0f, (dx + win_size) / (2 * win_size)));
        float vf = std::min(1.0f, std::max(0.0f, (dy + win_size) / (2 * win_size)));
        tmp = m_lfv->SingleView(m_frameId, uf, vf);
    } else
        tmp = m_lfv->RenderLF(m_frameId, m_alpha, m_aperture);
    if (!tmp.empty()) {
        cvtColor(tmp, tmp, CV_BGR2RGB);
        cv::resize(tmp, tmp, Size(), m_scale, m_scale);
        QImage img = QImage(tmp.data, tmp.cols, tmp.rows, tmp.cols*3, QImage::Format_RGB888);
        _p->drawImage(QPoint(0,0), img);
    }
    if (m_showFocus && m_pos.x >= 0 && m_pos.y >= 0) {
        QPen pen(QColor(255, 0, 0, 255));
        pen.setWidth(5);
        _p->setPen(pen);
        _p->setBrush(QColor(255, 0, 0, 50));
        float brushSize = 10.0;
        QPointF pos(m_pos.x*m_scale, m_pos.y*m_scale);
        _p->drawEllipse(QRectF(pos.x() - brushSize,
            pos.y() - brushSize, brushSize*2, brushSize*2));

    }
    _p->restore();
    if (m_isPlay) {
        double t = timer->Time() * 1000;
        int remain_t = max_t -t;
        qSleep(remain_t);
    }

    DELETE_OBJECT(timer);
}

QSize CPlayerView::sizeHint() const {
    return QSize(m_frameW*m_scale, m_frameH*m_scale);
}


void CPlayerView::changeAperture(int _s) {
    setAperture(qreal(_s) / 1000);
}

void CPlayerView::setFixedFocus() {
    DEBUG_INFO("set fixed focus mode");
    m_isTrack = false;
}

void CPlayerView::setTrackingFocus() {
    DEBUG_INFO("set tracking focus mode");
    m_isTrack = true;
}

void CPlayerView::quit() {
    DEBUG_INFO("quit the program");
    exit(0);
}

void CPlayerView::changeFocus(int _s) {
    setAlpha(qreal(_s) / 500);
}

void CPlayerView::setAlpha(float s) {
    m_alpha = (s-0.5)*10;
    printf("set alpha = %3.3f\n", m_alpha);
    fflush(stdout);
    update();
}

void CPlayerView::Clear() {
    m_pos = PointF(-1, -1);
    m_cursor = QPointF(-1, -1);
    m_oldCursor = QPointF(-1, -1);
    m_lfv = NULL;
    m_numFrames = 0;
    m_isPlay = false;
    m_changeFocus = false;
    m_changeView = false;
    m_scale = 1.0;
    m_frameId = 0;
}

void CPlayerView::updateFocus(QPointF _p, bool _click) {
    int x = round(_p.x()/m_scale);
    int y = round(_p.y()/m_scale);

    if (_click) {
        m_pos = PointF(x, y);
        m_alpha = m_lfv->Disparity(m_frameId, m_pos);
    } else {
        if (m_isTrack && m_isPlay) {
            m_pos = m_lfv->TrackPoint(m_pos, m_frameId);
            m_alpha = m_lfv->Disparity(m_frameId, m_pos);
        }
    }

    int alpha_i = (m_alpha / 10.0 + 0.5)*500;
    emit(focusChanged(alpha_i));
    printf("focus (%d, %d), alpha = %3.3f\n", x, y, m_alpha);
    fflush(stdout);
    update();
}

void CPlayerView::setAperture(float s) {
    m_aperture = s;
    update();
}

void CPlayerView::timerEvent(QTimerEvent *e) {
    if (e->timerId() == m_timer.timerId()) {
        if (m_isPlay) {

            if (m_isTrack)
                m_frameId = (m_frameId+1) % (m_numFrames-1);
            else
                m_frameId = (m_frameId+1) % m_numFrames;
            update();

            if (m_frameId == 0) {
                m_isPlay = false;
                m_timer.stop();
            }
            if (m_isTrack)
                updateFocus(m_cursor, false);
        }
    }
}

void CPlayerView::wheelEvent(QWheelEvent *e) {
    m_aperture += e->delta() / qreal(1000);
    m_aperture = qMax(qreal(0), qMin(qreal(1), m_aperture));
    emit apertureChanged(int(m_aperture*1000));
}

void CPlayerView::reset() {
    emit focusChanged(250);
    emit apertureChanged(1000);
    m_isPlay = false;
    m_changeFocus = false;
    m_changeView = false;
    m_frameId = 0;
    m_pos = PointF(-1.0, -1.0);
}

void CPlayerView::play() {
    DEBUG_INFO("press play button");
    if (!m_isPlay) {
        m_isPlay = true;
        m_timer.stop();
        m_timer.start(1, this);
    } else {
        m_isPlay = false;
        m_timer.stop();
    }
}

void CPlayerView::showFocus() {
    m_showFocus = !m_showFocus;
    update();
}


CPlayerQT::CPlayerQT(QWidget *parent, CLFVideo* _lfv, float _s)
    : QWidget(parent) {
    setWindowTitle(tr("Light Field Video Viewer"));

    m_view = new CPlayerView(this, _lfv, _s);
    m_view->setMinimumSize(256, 256);

    QGroupBox *mainGroup = new QGroupBox(this);
    mainGroup->setFixedWidth(180);
    mainGroup->setTitle(tr("Control Panel"));

    QGroupBox* focusGroup = new QGroupBox(mainGroup);
    focusGroup->setTitle(tr("Focal Plane"));
    QSlider* focusSlider = new QSlider(Qt::Horizontal, focusGroup);
    focusSlider->setRange(0, 500);
    focusSlider->setValue(250);
    focusSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QGroupBox* apertureGroup = new QGroupBox(mainGroup);
    apertureGroup->setTitle(tr("Aperture"));
    QSlider* apertureSlider = new QSlider(Qt::Horizontal, apertureGroup);
    apertureSlider->setRange(0, 1000);
    apertureSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QGroupBox* typeGroup = new QGroupBox(mainGroup);
    typeGroup->setTitle(tr("Focal Type"));
    QRadioButton* fixedType = new QRadioButton(typeGroup);
    QRadioButton* trackingType = new QRadioButton(typeGroup);
    fixedType->setText(tr("Fixed Point"));
    trackingType->setText(tr("Tracking Point"));
    fixedType->setChecked(true);

    QPushButton *restartButton = new QPushButton(mainGroup);
    restartButton->setText(tr("Restart"));


    QPushButton *playButton = new QPushButton(mainGroup);
    playButton->setText(tr("Play/Stop"));
    playButton->setCheckable(true);

    QPushButton *quitButton = new QPushButton(mainGroup);
    quitButton->setText(tr("Quit"));
    quitButton->setCheckable(true);

    QHBoxLayout *viewLayout = new QHBoxLayout(this);
    viewLayout->addWidget(m_view);
    viewLayout->addWidget(mainGroup);



    QVBoxLayout *focusGroupLayout = new QVBoxLayout(focusGroup);
    focusGroupLayout->addWidget(focusSlider);

    QVBoxLayout *apertureGroupLayout = new QVBoxLayout(apertureGroup);
    apertureGroupLayout->addWidget(apertureSlider);

    QVBoxLayout *typeGroupLayout = new QVBoxLayout(typeGroup);
    typeGroupLayout->addWidget(fixedType);
    typeGroupLayout->addWidget(trackingType);
    typeGroupLayout->addSpacing(4);
    QVBoxLayout *mainGroupLayout = new QVBoxLayout(mainGroup);
    mainGroupLayout->addWidget(focusGroup);
    mainGroupLayout->addWidget(apertureGroup);
    mainGroupLayout->addWidget(typeGroup);
    mainGroupLayout->addStretch(1);
    mainGroupLayout->addWidget(restartButton);
    mainGroupLayout->addWidget(playButton);
    mainGroupLayout->addWidget(quitButton);

    connect(apertureSlider, SIGNAL(valueChanged(int)), m_view, SLOT(changeAperture(int)));
    connect(focusSlider, SIGNAL(valueChanged(int)), m_view, SLOT(changeFocus(int)));
    connect(fixedType, SIGNAL(clicked()), m_view, SLOT(setFixedFocus()));
    connect(trackingType, SIGNAL(clicked()), m_view, SLOT(setTrackingFocus()));

    connect(m_view, SIGNAL(focusChanged(int)), focusSlider, SLOT(setValue(int)));
    connect(m_view, SIGNAL(apertureChanged(int)), apertureSlider, SLOT(setValue(int)));
    connect(playButton, SIGNAL(clicked()), m_view, SLOT(play()));
    connect(restartButton, SIGNAL(clicked()), m_view, SLOT(reset()));
    connect(quitButton, SIGNAL(clicked()), m_view, SLOT(quit()));
    m_view->reset();
}
