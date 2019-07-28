/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "dstyleanimation.h"

#ifndef QT_NO_ANIMATION

#include <qcoreapplication.h>
#include <qwidget.h>
#include <qevent.h>

namespace dstyle {

static const qreal ScrollBarFadeOutDuration = 200.0;
static const qreal ScrollBarFadeOutDelay = 450.0;

DStyleAnimation::DStyleAnimation(QObject *target) : QAbstractAnimation(target),
    _delay(0), _duration(-1), _startTime(QTime::currentTime()), _fps(ThirtyFps), _skip(0)
{
}

DStyleAnimation::~DStyleAnimation()
{
}

QObject *DStyleAnimation::target() const
{
    return parent();
}

int DStyleAnimation::duration() const
{
    return _duration;
}

void DStyleAnimation::setDuration(int duration)
{
    _duration = duration;
}

int DStyleAnimation::delay() const
{
    return _delay;
}

void DStyleAnimation::setDelay(int delay)
{
    _delay = delay;
}

QTime DStyleAnimation::startTime() const
{
    return _startTime;
}

void DStyleAnimation::setStartTime(const QTime &time)
{
    _startTime = time;
}

DStyleAnimation::FrameRate DStyleAnimation::frameRate() const
{
    return _fps;
}

void DStyleAnimation::setFrameRate(FrameRate fps)
{
    _fps = fps;
}

void DStyleAnimation::updateTarget()
{
    QEvent event(QEvent::StyleAnimationUpdate);
    event.setAccepted(false);
    QCoreApplication::sendEvent(target(), &event);
    if (!event.isAccepted())
        stop();
}

void DStyleAnimation::start()
{
    _skip = 0;
    QAbstractAnimation::start(DeleteWhenStopped);
}

bool DStyleAnimation::isUpdateNeeded() const
{
    return currentTime() > _delay;
}

void DStyleAnimation::updateCurrentTime(int)
{
    if (++_skip >= _fps) {
        _skip = 0;
        if (target() && isUpdateNeeded())
            updateTarget();
    }
}

DProgressStyleAnimation::DProgressStyleAnimation(int speed, QObject *target) :
    DStyleAnimation(target), _speed(speed), _step(-1)
{
}

int DProgressStyleAnimation::animationStep() const
{
    return currentTime() / (1000.0 / _speed);
}

int DProgressStyleAnimation::progressStep(int width) const
{
    int step = animationStep();
    int progress = (step * width / _speed) % width;
    if (((step * width / _speed) % (2 * width)) >= width)
        progress = width - progress;
    return progress;
}

int DProgressStyleAnimation::speed() const
{
    return _speed;
}

void DProgressStyleAnimation::setSpeed(int speed)
{
    _speed = speed;
}

bool DProgressStyleAnimation::isUpdateNeeded() const
{
    if (DStyleAnimation::isUpdateNeeded()) {
        int current = animationStep();
        if (_step == -1 || _step != current)
        {
            _step = current;
            return true;
        }
    }
    return false;
}

DNumberStyleAnimation::DNumberStyleAnimation(QObject *target) :
    DStyleAnimation(target), _start(0.0), _end(1.0), _prev(0.0)
{
    setDuration(250);
}

qreal DNumberStyleAnimation::startValue() const
{
    return _start;
}

void DNumberStyleAnimation::setStartValue(qreal value)
{
    _start = value;
}

qreal DNumberStyleAnimation::endValue() const
{
    return _end;
}

void DNumberStyleAnimation::setEndValue(qreal value)
{
    _end = value;
}

qreal DNumberStyleAnimation::currentValue() const
{
    qreal step = qreal(currentTime() - delay()) / (duration() - delay());
    return _start + qMax(qreal(0), step) * (_end - _start);
}

bool DNumberStyleAnimation::isUpdateNeeded() const
{
    if (DStyleAnimation::isUpdateNeeded()) {
        qreal current = currentValue();
        if (!qFuzzyCompare(_prev, current))
        {
            _prev = current;
            return true;
        }
    }
    return false;
}

DBlendStyleAnimation::DBlendStyleAnimation(Type type, QObject *target) :
    DStyleAnimation(target), _type(type)
{
    setDuration(250);
}

QImage DBlendStyleAnimation::startImage() const
{
    return _start;
}

void DBlendStyleAnimation::setStartImage(const QImage& image)
{
    _start = image;
}

QImage DBlendStyleAnimation::endImage() const
{
    return _end;
}

void DBlendStyleAnimation::setEndImage(const QImage& image)
{
    _end = image;
}

QImage DBlendStyleAnimation::currentImage() const
{
    return _current;
}

/*! \internal

    A helper function to blend two images.

    The result consists of ((alpha)*startImage) + ((1-alpha)*endImage)

*/
static QImage blendedImage(const QImage &start, const QImage &end, float alpha)
{
    if (start.isNull() || end.isNull())
        return QImage();

    QImage blended;
    const int a = qRound(alpha*256);
    const int ia = 256 - a;
    const int sw = start.width();
    const int sh = start.height();
    const int bpl = start.bytesPerLine();
    switch (start.depth()) {
    case 32:
        {
            blended = QImage(sw, sh, start.format());
            blended.setDevicePixelRatio(start.devicePixelRatio());
            uchar *mixed_data = blended.bits();
            const uchar *back_data = start.bits();
            const uchar *front_data = end.bits();
            for (int sy = 0; sy < sh; sy++) {
                quint32* mixed = (quint32*)mixed_data;
                const quint32* back = (const quint32*)back_data;
                const quint32* front = (const quint32*)front_data;
                for (int sx = 0; sx < sw; sx++) {
                    quint32 bp = back[sx];
                    quint32 fp = front[sx];
                    mixed[sx] =  qRgba ((qRed(bp)*ia + qRed(fp)*a)>>8,
                                        (qGreen(bp)*ia + qGreen(fp)*a)>>8,
                                        (qBlue(bp)*ia + qBlue(fp)*a)>>8,
                                        (qAlpha(bp)*ia + qAlpha(fp)*a)>>8);
                }
                mixed_data += bpl;
                back_data += bpl;
                front_data += bpl;
            }
        }
    default:
        break;
    }
    return blended;
}

void DBlendStyleAnimation::updateCurrentTime(int time)
{
    DStyleAnimation::updateCurrentTime(time);

    float alpha = 1.0;
    if (duration() > 0) {
        if (_type == Pulse) {
            time = time % duration() * 2;
            if (time > duration())
                time = duration() * 2 - time;
        }

        alpha = time / static_cast<float>(duration());

        if (_type == Transition && time > duration()) {
            alpha = 1.0;
            stop();
        }
    } else if (time > 0) {
        stop();
    }

    _current = blendedImage(_start, _end, alpha);
}

DScrollbarStyleAnimation::DScrollbarStyleAnimation(Mode mode, QObject *target) : DNumberStyleAnimation(target), _mode(mode), _active(false)
{
    switch (mode) {
    case Activating:
        setDuration(ScrollBarFadeOutDuration);
        setStartValue(0.0);
        setEndValue(1.0);
        break;
    case Deactivating:
        setDuration(ScrollBarFadeOutDelay + ScrollBarFadeOutDuration);
        setDelay(ScrollBarFadeOutDelay);
        setStartValue(1.0);
        setEndValue(0.0);
        break;
    }
}

DScrollbarStyleAnimation::Mode DScrollbarStyleAnimation::mode() const
{
    return _mode;
}

bool DScrollbarStyleAnimation::wasActive() const
{
    return _active;
}

void DScrollbarStyleAnimation::setActive(bool active)
{
    _active = active;
}

} // end namespace dstyle

#endif //QT_NO_ANIMATION
