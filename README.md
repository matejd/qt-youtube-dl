qt-youtube-dl
-------------

This is a tiny Qt/C++ GUI frontend for the [youtube-dl](https://github.com/rg3/youtube-dl/) tool.
youtube-dl downloads videos from youtube.com and *many* other video hosting platforms.
Windows & Linux supported.


Screenshot
----------

Windows 10
![Screenshot](http://matejd.github.io/qt-youtube-dl/qt-youtube-dl-screenshot-win10.png)



Windows downloads and build
---------------------------

Latest Windows x64 installer can be found under [releases](https://github.com/matejd/qt-youtube-dl/releases).

If you want to build this app yourself on Windows, you'll need a couple of things:
- Qt5, grab an installer [here](http://www.qt.io/download-open-source/)
- Python
- cx\_Freeze ([link](http://cx-freeze.sourceforge.net/))
- Inno Setup, if you want to create an installer

Open the .pro file in QtCreator and run the build process. This doesn't include
the youtube\_dl Python package. Execute ```python setup_cxfreeze.py build``` to
freeze youtube\_dl into an executable + required files. Copy the resulting
```build/platform-freeze/``` folder into ```build/release/youtube_dl_frozen/```. This
is where ```__main__.exe``` needs to end up for the app to find it.



Linux build
-----------

Uses qmake (and make). Simply run ```make release``` from the project's top-level directory.
Requires ```Qt5```. Expects a python installation on the system. youtube-dl package
is bundled with the repository.
