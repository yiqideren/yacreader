# #####################################################################
# Automatically generated by qmake (2.01a) mi� 8. oct 20:54:05 2008
# #####################################################################
TEMPLATE = app
TARGET = YACReader
DEPENDPATH += . \
    release

DEFINES += NOMINMAX

 unix:!macx{
QMAKE_CXXFLAGS += -std=c++11
}

isEqual(QT_MAJOR_VERSION, 5) {
	Release:DESTDIR = ../release5
	Debug:DESTDIR = ../debug5

} else {
	Release:DESTDIR = ../release
	Debug:DESTDIR = ../debug
}

SOURCES += main.cpp
include(YACReader.pri)
include(../QsLog/QsLog.pri)

RC_FILE = icon.rc

macx {
	ICON = YACReader.icns
}

TRANSLATIONS = yacreader_es.ts \
								  yacreader_fr.ts \ 
								  yacreader_ru.ts \
								  yacreader_pt.ts \
								  yacreader_nl.ts \
								  yacreader_tr.ts \
								  yacreader_source.ts  


win32 {
!exists (../compressed_archive/lib7zip) {
	error(You\'ll need 7zip source code to compile YACReader. \
	Please check the compressed_archive folder for further instructions.)
}
}

unix {
exists (../compressed_archive/libp7zip) {
	message(Found p7zip source code...)
	system(patch -d ../compressed_archive -N -p0 -i libp7zip.patch)
} else {
	error(You\'ll need 7zip source code to compile YACReader. \
	Please check the compressed_archive folder for further instructions.)
}
}

unix:!macx {
#set install prefix if it's empty
isEmpty(PREFIX) {
	PREFIX = /usr
}

BINDIR = $$PREFIX/bin
LIBDIR = $$PREFIX/lib
DATADIR = $$PREFIX/share

DEFINES += "LIBDIR=\\\"$$LIBDIR\\\""  "DATADIR=\\\"$$DATADIR\\\""

#MAKE INSTALL

INSTALLS += bin docs icon desktop translation #manpage

bin.path = $$BINDIR
isEmpty(DESTDIR) {
	bin.files = YACReader
} else {
	bin.files = $$DESTDIR/YACReader
}

docs.path = $$DATADIR/doc/yacreader

#rename docs for better packageability
docs.extra = cp ../CHANGELOG.txt ../changelog; cp ../README.txt ../README
docs.files = ../README ../changelog

icon.path = $$DATADIR/yacreader
icon.files = ../images/icon.png

desktop.path = $$DATADIR/applications
desktop.extra = desktop-file-edit --set-icon=$$DATADIR/yacreader/icon.png $$PWD/../YACReader.desktop
desktop.files = ../YACReader.desktop

#TODO: icons should be located at /usr/share/icons and have the same basename as their application

translation.path = $$DATADIR/yacreader/languages
translation.files = ../release/languages/yacreader_*

manpage.path = $$DATADIR/man/man1
manpage.files = ../YACReader.1

#remove leftover doc files when 'make clean' is invoked
QMAKE_CLEAN += "../changelog" "../README"
}
