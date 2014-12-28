release:
	mkdir -p build/release
	qtchooser -run-tool=qmake -qt=qt5 qt-youtube-dl.pro -o build/release/Makefile
	cd build/release && make
	cp -r youtube_dl/ build/release/

clean:
	rm -rf build
