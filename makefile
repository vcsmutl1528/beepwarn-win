
# beepwarn-win makefile

.SUFFIXES:

main : help beepwarn.exe cleanobj

help:
	@echo "PLATFORM: WINDOWS. Linux is not suppoted."
	@echo You will need VISUAL Studio (v9.0 tested) with SDK (^>=v6.0A).
	@echo USAGE:
	@echo NMAKE ^[^<target-name^>^]
	@echo Targets: help, main (default), clean, test, cleanall
	@echo The main module is 'beepwarn.exe'.

test: abeeptst.exe

comnflags = /nologo

beepwarn.exe: beepwarn.c abeep.c ntdllexp.lib
	cl $(comnflags) /Gz beepwarn.c abeep.c ntdllexp.lib user32.lib shell32.lib shlwapi.lib
	mt $(comnflags) -manifest beepwarn.exe.manifest -outputresource:beepwarn.exe

abeeptst.exe: abeeptst.c abeep.c ntdllexp.lib
	cl $(comnflags) /Gz abeeptst.c abeep.c ntdllexp.lib
	mt $(comnflags) -manifest abeeptst.exe.manifest -outputresource:abeeptst.exe

ntdllexp.lib: ntdllexp.c ntdllexp.def
	cl $(comnflags) /Gz /c ntdllexp.c
	lib $(comnflags) /DEF:ntdllexp.def /NAME:ntdll ntdllexp.obj

clean: cleanobj

cleanobj:
	del *.obj *.exp *.lib *.manifest

cleanall: cleanobj
	del *.exe
