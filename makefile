release:
	mkdir -p build/release
	qtchooser -run-tool=qmake -qt=qt5 qt-youtube-dl.pro CONFIG+=release -o build/release/Makefile
	cd build/release && make
	cp -r youtube_dl/ build/release/

debug:
	mkdir -p build/debug
	qtchooser -run-tool=qmake -qt=qt5 qt-youtube-dl.pro CONFIG+=debug -o build/debug/Makefile
	cd build/debug && make
	cp -r youtube_dl/ build/debug/

clean:
	rm -rf build
