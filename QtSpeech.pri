# QtSpeech -- a small cross-platform library to use TTS
# Copyright (C) 2010-2011 LynxLine.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General
# Public License along with this library; if not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301 USA

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += \
    QtSpeech \
    QtSpeech.h \

macx {
    SOURCES += QtSpeech_mac.cpp
    LIBS *= -framework AppKit
}

win32 {
    HEADERS += sapi.hh sphelper.hh
    SOURCES += QtSpeech_win.cpp guids.c
    LIBS    += -lole32
}

unix:!mac {
    HEADERS += QtSpeech_unx.h
    SOURCES += QtSpeech_unx.cpp

    INCLUDEPATH += $$PWD/festival/speech_tools/include
    INCLUDEPATH += $$PWD/festival/festival/src/include
    INCLUDEPATH += /usr/include/festival
    INCLUDEPATH += /usr/include/speech_tools

    LIBS += -lncurses
    LIBS += -L$$PWD/festival/festival/src/lib -lFestival
    LIBS += -L$$PWD/festival/speech_tools/lib -lestools -lestbase -leststring

    # Linux: use asound 
    LIBS += -lasound
    
    # Mac: use system Frameworks
    #LIBS += -framework CoreAudio -framework AudioUnit -framework AudioToolbox -framework Carbon
}
