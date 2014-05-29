	pocc.exe -out:output/ -machine:x86 ^
		-IV:/lang/C/pellesc/Lib/Win ^
		ext/sapi.lib src/pnglib/pnglib_safe.lib src/pnglib/pnglib.lib src/hunspell/libhunspell.lib ^
		kernel32.lib user32.lib gdi32.lib comctl32.lib comdlg32.lib advapi32.lib delayimp.lib shlwapi.lib shell32.lib version.lib winmm.lib ole32.lib uuid.lib msimg32.lib wininet.lib olepro32.lib crypt32.lib ws2_32.lib
