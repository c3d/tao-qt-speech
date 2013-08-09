MODINSTDIR = speech
include(../../modules_defs.pri)

include(QtSpeech-dll.pro)
TARGET = QtSpeech
INSTALLS = target
target.path = $${MODINSTPATH}/lib

win32 {
    # Remove the .a file after installation
    postinstall.path     = $${target.path}
    postinstall.depends  = install_target
    postinstall.commands = rm $${target.path}/lib$${TARGET}.a
    INSTALLS += postinstall
}
