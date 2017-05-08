#ifndef PLAYERQT_H
#define PLAYERQT_H

#include "common.h"
#include "arthurwidgets.h"
#include <QBasicTimer>
#include <QPolygonF>

class CLFVideo;

class CPlayerView : public ArthurFrame
{
public:
    Q_OBJECT

public:
    CPlayerView(QWidget *parent, CLFVideo* _lfv, float _s);
    void paint(QPainter *) Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE; // { return QSize(m_frameH*m_ratio, m_frameW*m_ratio); }

    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void keyPressEvent( QKeyEvent* e ) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

    qreal focus() const { return m_alpha; }
    qreal aperture() const { return m_aperture; }
    void setAperture(float s);
    void setAlpha(float s);


public slots:
    void changeFocus(int _s);
    void changeAperture(int _s);
    void setFixedFocus();
    void setTrackingFocus();
    void quit();
    void reset();
    void play();
    void showFocus();

signals:
    void focusChanged(int _s);
    void apertureChanged(int _s);

protected:
    void timerEvent(QTimerEvent *e) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;

private:
     void Clear();
     void updateFocus(QPointF _p, bool _click);


private:
    qreal m_aperture;
    qreal m_alpha;
    PointF m_pos;
    QPointF m_cursor;
    QPointF m_oldCursor;
    CLFVideo* m_lfv;
    int m_numFrames;
    bool m_isPlay;
    bool m_isTrack;
    int m_frameId;
    int m_frameH;
    int m_frameW;
    float m_scale;
    bool m_changeFocus;
    bool m_changeView;
    QBasicTimer m_timer;
    double m_fps;
    bool m_showFocus;
};

class CPlayerQT : public QWidget
{
    Q_OBJECT
public:
    CPlayerQT(QWidget *parent, CLFVideo* _lfv, float _s);

private:
    CPlayerView* m_view;
};

#endif // PLAYERQT_H
