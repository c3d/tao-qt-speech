/*  QtSpeech -- a small cross-platform library to use TTS
    Copyright (C) 2010-2011 LynxLine.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General
    Public License along with this library; if not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301 USA */

#include <QtCore>
#include <QtSpeech>
#include <QtSpeech_unx.h>
#include <festival.h>

namespace QtSpeech_v1 { // API v1.0

// some defines for throwing exceptions
#define Where QString("%1:%2:").arg(__FILE__).arg(__LINE__)
#define SysCall(x,e) {\
    int ok = x;\
    if (!ok) {\
        QString msg = #e;\
        msg += ":"+QString(__FILE__);\
        msg += ":"+QString::number(__LINE__)+":"+#x;\
        throw e(msg);\
    }\
}

WavePlayer::WavePlayer(QObject * p)
    : QProcess(p), tmp("/tmp/qtspeechXXXXXX.wav")
{
    tmp.open();
}

QString WavePlayer::filePath()
{
    return tmp.fileName();
}

void WavePlayer::play()
{
    const char * p = getenv("QTSPEECH_CMD");
    if (!p)
        p = "aplay %f";
    start(QString(p).replace("%f", filePath()));
}

void WavePlayer::stop()
{
    terminate();
    if (!waitForFinished(3000))
    {
        kill();
        waitForFinished(3000);
    }
}

// qobject for speech process
bool QtSpeech_proc::init = false;
QPointer<WavePlayer> QtSpeech_proc::player = 0;
void QtSpeech_proc::say(QString text) {
    try {
        if (player)
        {
            player->stop();
            delete player;
        }
        player = new WavePlayer;

        if (!init) {
            int heap_size = FESTIVAL_HEAP_SIZE;
            festival_initialize(true,heap_size);
            init = true;
        }
        has_error = false;
        EST_String est_text(text.toUtf8());
        EST_Wave est_wave;
        SysCall(festival_text_to_wave(est_text, est_wave), QtSpeech::LogicError);
        EST_String est_file(player->filePath().toUtf8());
        est_wave.save(est_file);

        connect(player, SIGNAL(finished(int)), this, SIGNAL(finished()));
        connect(player, SIGNAL(finished(int)), player, SLOT(deleteLater()));
        player->play();
    }
    catch(QtSpeech::LogicError e) {
        has_error = true;
        err = e;
    }
}

// internal data
class QtSpeech::Private {
public:
    Private() {}
    VoiceName name;
    static const QString VoiceId;
};
const QString QtSpeech::Private::VoiceId = QString("festival:%1");

// implementation
QtSpeech::QtSpeech(QObject * parent)
    :QObject(parent), d(new Private)
{
    VoiceName n = {Private::VoiceId.arg("english"), "English"};
    if (n.id.isEmpty())
        throw InitError(Where+"No default voice in system");

    d->name = n;
}

QtSpeech::QtSpeech(VoiceName n, QObject * parent)
    :QObject(parent), d(new Private)
{
    if (n.id.isEmpty()) {
        VoiceName def = {Private::VoiceId.arg("english"), "English"};
        n = def;
    }

    if (n.id.isEmpty())
        throw InitError(Where+"No default voice in system");

    d->name = n;
}

QtSpeech::~QtSpeech()
{
    //if ()
    delete d;
}

const QtSpeech::VoiceName & QtSpeech::name() const {
    return d->name;
}

QtSpeech::VoiceNames QtSpeech::voices()
{
    VoiceNames vs;
    VoiceName n = {Private::VoiceId.arg("english"), "English"};
    vs << n;
    return vs;
}

void QtSpeech::tell(QString text)
{
    QtSpeech_proc * th = new QtSpeech_proc(this);
    connect(th, SIGNAL(finished()), this, SIGNAL(finished()));
    connect(th, SIGNAL(finished()), th, SLOT(deleteLater()));
    th->say(text);
}

void QtSpeech::say(QString text)
{
    QEventLoop el;
    QtSpeech_proc th;
    connect(&th, SIGNAL(finished()), &el, SLOT(quit()), Qt::QueuedConnection);
    QMetaObject::invokeMethod(&th, "say", Qt::QueuedConnection, Q_ARG(QString,text));
    el.exec();

    if (th.has_error)
        throw th.err;
}

void QtSpeech::timerEvent(QTimerEvent * te)
{
    QObject::timerEvent(te);
}

} // namespace QtSpeech_v1
