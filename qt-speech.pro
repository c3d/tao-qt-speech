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
macx {
    # Make sure QtSpeech references the Qt libraries bundled with the application
    # and not the ones that may be installed on the target system, otherwise
    # they may clash
    FIX_QT_REFS = $${TAOTOPSRC}/modules/fix_qt_refs
    QMAKE_POST_LINK = $$FIX_QT_REFS "$(TARGET)" \"$$[QT_INSTALL_LIBS]\"
}
