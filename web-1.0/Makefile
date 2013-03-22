prefix=/usr

web: web.c
	gcc -O web.c -o web `pkg-config --cflags gtk+-2.0 --libs webkit-1.0`

install: web
	sudo install -g dialout web $(prefix)/bin
	sudo install -g audio piradio $(prefix)/local/bin
	sudo chmod +x $(prefix)/local/bin/piradio
	sudo cp ./web.desktop $(prefix)/share/applications
	sudo cp ./minimalwebbrowser.png $(prefix)/share/pixmaps
	sudo cp ./web.1.gz $(prefix)/share/man/man1
	sudo cp ./web-omxplayer.sh $(prefix)/local/bin
	sudo chmod +x $(prefix)/local/bin/web-omxplayer.sh

remove: 
	sudo rm $(prefix)/bin/web
	sudo rm $(prefix)/local/bin/piradio
	sudo rm $(prefix)/share/applications/web.desktop
	sudo rm $(prefix)/share/pixmaps/minimalwebbrowser.png
	sudo rm $(prefix)/share/man/man1/web.1.gz
	sudo rm $(prefix)/local/bin/web-omxplayer.sh

clean:
	rm ./web

tar: web.c Makefile
	cd .. && tar -czvf web_1.0.orig.tar.gz ./web-1.0/Makefile ./web-1.0/web.c ./web-1.0/INSTALL ./web-1.0/COPYING ./web-1.0/minimalwebbrowser.png ./web-1.0/web.desktop ./web-1.0/web.1.gz ./web-1.0/web-omxplayer.sh ./web-1.0/piradio

deb: web
	debuild -us -uc	

install-deb: web_1.0-*_armhf.deb
	sudo dpkg -i ../web_1.0-*_armhf.deb
