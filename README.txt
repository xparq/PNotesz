ABOUT
=====

This is Andrey Gruber's nice, but obsoleted PNote (http://pnotes.sourceforge.net/) 
9.3.0 Win32 C codebase, with some notes (and slight modifications) added, making 
it easy to access for tinkeres. (Andrey now develops a .NET version (https://sourceforge.net/projects/pnotes/files/PNotes.NET/) of the original program, 
check that out, too!)


CHANGES
=======

* Renamed the .exe (and the repo) to avoid confusion about (possible, subtle) differences from the original.
* #include <tom.h> -> #include <TOM.h>, so you can compile on a case-sensitive filesystem.


BUILD
=====

Use Pelles C. (Not sure of anything else.)

Setup:

1. Create new "Win32 .EXE" project. Nothing fancy, leave everything default.
   (There's an example Pelles project file in here, too.)

2. Add all the sources (.h, .c, .rc) to the project.

Compile:

- Includes:

	* Add the pnglib & hunspell dirs to the include path.

Link:

- Libs:

	* sapi.lib (I just downloaded & added it to ext/)
	* Add the pnglib (both?!) & hunspell libs.
	* kernel32.lib user32.lib gdi32.lib comctl32.lib comdlg32.lib advapi32.lib delayimp.lib 
	  shlwapi.lib shell32.lib version.lib winmm.lib ole32.lib uuid.lib msimg32.lib wininet.lib 
	  olepro32.lib crypt32.lib ws2_32.lib

- Resources:

	* output/*.res

- Linker command line that worked for me: 

	polink.exe -out:result/pnotesz.exe output/*.obj output/*.res -subsystem:windows -machine:x86 ^
		-libpath:V:/lang/C/pellesc/Lib/Win ^
		ext/sapi.lib src/pnglib/pnglib_safe.lib src/pnglib/pnglib.lib src/hunspell/libhunspell.lib ^
		kernel32.lib user32.lib gdi32.lib comctl32.lib comdlg32.lib advapi32.lib delayimp.lib shlwapi.lib shell32.lib version.lib winmm.lib ole32.lib uuid.lib msimg32.lib wininet.lib olepro32.lib crypt32.lib ws2_32.lib


DEPLOY
======

1. Put these next to the .exe:

	* pnotes.resources	[icons etc.]
	* groups.images
	* smilies.images
	* hunspell.dll

2. Make sure the dir where the .exe is, is writable.
    It'll nicely create the dirs it would need/use.

3. Populate the 

	* dictionaries	[OPTIONAL]
	* lang	[OPTIONAL]
	* fonts

    dirs with the stuff found the binary release.

RUN
===

Just run the .exe, but preferably put a shortcut under Programs/Startup (as usual).
