######################################################################
# Automatically generated by qmake (2.01a) ?? ?? 11 22:32:44 2014
######################################################################

TEMPLATE = app
CONFIG  += qt warn_on debug console qaxcontainer qtestlib
TARGET = qtverify
RC_FILE = qtverify.rc

QT += sql

DEPENDPATH += . GeneratedFiles							 

QMAKE_LIBDIR = 	$(ADEHOME)/lib \
                $(ADEHOME)/bin \

								
LIBS += -lalgorithm -lsystemsetdlg -ldatatestdlg -lqaxserver -lmasterslaveset -llogindialog \
				-lqtexdb -lflowweight -lflowstandard -ltotalweight -ltotalstandard -lcalcverify \
				-lreport -ltvercomp -ltverparam -lcmbverify -lresultquerydlg -lmd5encode

# Input
HEADERS += include/mainform.h	\
					 include/dbmysql.h	\
					 include/qexcel.h   \
					 include/register.h
					 
FORMS += 	ui/mainform.ui	\
				 	ui/dbmysql.ui		\
				 	ui/register.ui
				 
SOURCES += source/main.cpp	\
					 source/mainform.cpp	\
					 source/dbmysql.cpp		\
					 source/qexcel.cpp    \
					 source/register.cpp
					 
RESOURCES += qtverify.qrc

win32{
DEFINES += WIN32 _AFXDLL
DEFINES -= _USRDLL
DESTDIR = $(ADEHOME)\tmp\qtverify\obj
}

MOC_DIR = $(ADEHOME)/tmp/qtverify/moc
OBJECTS_DIR = $(ADEHOME)/tmp/qtverify/obj
UI_DIR = $(ADEHOME_INC)/include


INCLUDEPATH += 	./include	\
								$${UI_DIR}	\
								$$(ADEHOME_INC)/include		\
								$$(ADEHOME_INC)/include/qextserial \

TRANSLATIONS += ./language/qtverify_en.ts ./language/qtverify_zh.ts

win32{
	MY_DEST_EXE_VAR = $${DESTDIR} $${TARGET}.exe
	MY_DEST_EXE = $$join( MY_DEST_EXE_VAR, "\\" )

	QMAKE_POST_LINK = copy $${MY_DEST_EXE} $(ADEHOME)\bin	\
                  & copy .\language\qtverify_zh.qm $(ADEHOME)\uif\i18n\zh	\
                  & copy .\language\qtverify_en.qm $(ADEHOME)\uif\i18n\en
}
