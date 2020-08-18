
# beepwarn-win makefile

.SUFFIXES:

!IFNDEF	VC6
MNFT	= mt
!ELSE
CDEFS	= /D_WIN32_WINNT=0x0500
!ENDIF
TLIB	= lib

main : help beepwarn.exe cleanobj

help:
	@echo "PLATFORM: WINDOWS. Linux is not suppoted."
	@echo You will need VISUAL Studio (v9.0 tested) with SDK (^>=v6.0A).
	@echo USAGE:
	@echo NMAKE ^[^<target-name^>^]
	@echo Targets: help, main (default), clean, test, cleanall
	@echo The main module is 'beepwarn.exe'.

test: abeeptst.exe

COMNFLAGS = /nologo
CFLAGS	= /Gz /Oxs $(CDEFS)

beepwarn.exe: beepwarn.c asynbeep.c ntdllexp.lib
	$(CC) $(COMNFLAGS) $(CFLAGS) beepwarn.c asynbeep.c ntdllexp.lib kernel32.lib \
	user32.lib shell32.lib shlwapi.lib /link /ENTRY:mainStartup
!IFDEF	MNFT
	$(MNFT) $(COMNFLAGS) -manifest beepwarn.exe.manifest -outputresource:beepwarn.exe
!ENDIF

abeeptst.exe: abeeptst.c asynbeep.c ntdllexp.lib
	$(CC) $(COMNFLAGS) $(CFLAGS) abeeptst.c asynbeep.c ntdllexp.lib kernel32.lib
!IFDEF	MNFT
	$(MNFT) $(COMNFLAGS) -manifest abeeptst.exe.manifest -outputresource:abeeptst.exe
!ENDIF

ntdllexp.lib: ntdllexp.c ntdllexp.def
	$(CC) $(COMNFLAGS) /Gz /c $(CDEFS) ntdllexp.c
	$(TLIB) $(COMNFLAGS) /NODEFAULTLIB /DEF:ntdllexp.def /NAME:ntdll ntdllexp.obj

clean: cleanobj

cleanobj:
	del *.obj *.exp *.lib *.manifest

cleanall: cleanobj
	del *.exe
