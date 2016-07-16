This program will check if a new version of Chromium is available and if it's the case, it download the installer and launch it. This is very old code, Windows only and tbh, probably not very useful but hey, why not.

Code written in C and can be compiled by a C99 compiler (gcc, ...)
Need libcURL to compile, an old version is in the lib archive but you really should upgrade it.
A Code::Blocks project is included and preconfigured.

Compiler command line (for minGW/GCC):
mingw32-gcc.exe -Wall -DCURL_STATICLIB  -O3     -Ilib\include  -c C:\Users\%USER%\Desktop\updater\main.c -o obj\Release\main.o
mingw32-gcc.exe -Wall -DCURL_STATICLIB  -O3     -Ilib\include  -c C:\Users\%USER%\Desktop\updater\download.c -o obj\Release\download.o

Linker command line:
mingw32-g++.exe -Llib  -o chromium_ugrader.exe obj\Release\main.o obj\Release\download.o   -static-libgcc -static -s -static-libgcc  lib\libcurl.a lib\librtmp.a lib\libidn.a lib\libssl.a lib\libssh2.a lib\libcrypto.a lib\libz.a -lwldap32 -lws2_32 -lwinmm -lgdi32 -luser32 -ldxguid 

(obviously, replace path)

You should put the binary in the "Startup" directory (here: C:\Users\%USER%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup)

This project is licensed under the [MPL licence][mpl-licence].
[mpl-licence]:https://mozilla.org/MPL/2.0/