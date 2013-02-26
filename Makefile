prefix=/usr

game: web.c
	gcc web.c -o web `pkg-config --cflags gtk+-2.0 --libs webkit-1.0`

install: web
	install -g dialout web $(prefix)/bin

remove: 
	rm $(prefix)/bin/web

clean:

tar: web.c Makefile
	tar -czvf web_1.0-1_armhf.deb.tar.gz Makefile web.c web_1.0-1_armhf.deb INSTALL COPYING debian

deb: web
	cp web ./debian/usr/bin/web
	dpkg-deb --build debian
	mv debian.deb web_1.0-1_armhf.deb

install-deb: web_1.0-1_armhf.deb
	sudo dpkg -i ./web_1.0-1_armhf.deb
	

