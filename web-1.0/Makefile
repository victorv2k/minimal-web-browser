prefix=/usr

web: web.c
	gcc web.c -o web `pkg-config --cflags gtk+-2.0 --libs webkit-1.0`

install: web
	sudo install -g dialout web $(prefix)/bin
	sudo cp ./web.desktop $(prefix)/share/applications/web.desktop
	sudo cp ./minimalwebbrowser.png $(prefix)/share/pixmaps/minimalwebbrowser.png

remove: 
	sudo rm $(prefix)/bin/web
	sudo rm $(prefix)/share/applications/web.desktop
	sudo rm $(prefix)/share/pixmaps/minimalwebbrowser.png

clean:
	rm ./web

tar: web.c Makefile
	cd .. && tar -czvf web_1.0.orig.tar.gz ./web-1.0/Makefile ./web-1.0/web.c ./web-1.0/INSTALL ./web-1.0/COPYING ./web-1.0/minimalwebbrowser.png ./web-1.0/web.desktop

deb: web
	debuild -us -uc	

install-deb: web_1.0-*_armhf.deb
	sudo dpkg -i ../web_1.0-*_armhf.deb
