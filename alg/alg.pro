######################################################################
# Automatically generated by qmake (2.01a) ?? ?? 11 22:37:20 2014
######################################################################

TEMPLATE = lib
CONFIG += dll console debug
TARGET = alg
DEPENDPATH += .
INCLUDEPATH += .

QMAKE_LIBDIR +=  ./             \
        	  		 $(RUNHOME)/lib \
	          		 $(RUNHOME)/bin \


# Input
HEADERS += alg.h
SOURCES += alg.cpp


win32{
DEFINES += WIN32 _AFXDLL
DEFINES -= _USRDLL
DESTDIR = $$(RUNHOME)\tmp\alg\obj
}

win32{
	MY_DEST_LIB_VAR = $${DESTDIR} $${TARGET}.lib
	MY_DEST_LIB = $$join( MY_DEST_LIB_VAR, "\\" )
	MY_DEST_DLL_VAR = $${DESTDIR} $${TARGET}.dll
	MY_DEST_DLL = $$join( MY_DEST_DLL_VAR, "\\" )

	QMAKE_POST_LINK = copy $${MY_DEST_LIB} $$(RUNHOME)\lib \
                        & copy $${MY_DEST_DLL} $$(RUNHOME)\dll
}
