# Copyright: (C) 2010, Keith Stribley <devel@thanlwinsoft.org>
#
# License:
#
# You are free to distribute this software under the terms of
# the GNU Lesser General Public License  either version 2.1 of the
# License, or (at your option) any later version.

kmnBase=pa-oh
kmnFile=$(kmnBase).kmn
keyboardDir=../../ekaya/kmfl
packageName=kmfl-keyboards-paoh
keyboardVersion=

include version.mk

all: lint



lint: $(packageName)_$(pkgVersion)_all.deb
	lintian -i -I --display-experimental --pedantic --show-overrides --checksums --color auto $(packageName)_$(pkgVersion)_`dpkg-architecture -qDEB_BUILD_ARCH`.changes

deb: $(packageName)_$(pkgVersion)_all.deb

$(packageName)_$(pkgVersion)_all.deb : debian/* $(packageName)_$(keyboardVersion).tar.gz
	cp $(packageName)_$(keyboardVersion).tar.gz $(packageName)_$(keyboardVersion).orig.tar.gz
	mkdir $(packageName)-$(keyboardVersion)
	tar -zxf $(packageName)_$(keyboardVersion).orig.tar.gz
	cp -rf debian $(packageName)-$(keyboardVersion)
	cd $(packageName)-$(keyboardVersion) && dpkg-buildpackage
	rm -rf $(packageName)-$(keyboardVersion)

tar : test $(packageName)_$(keyboardVersion).tar.gz

$(packageName)_$(keyboardVersion).tar.gz: $(keyboardDir)/$(kmnFile) $(keyboardDir)/$(kmnBase)*
	mkdir -p $(packageName)-$(keyboardVersion)
	cp changelog $(packageName)-$(keyboardVersion)
	cp $(keyboardDir)/$(kmnFile) $(packageName)-$(keyboardVersion)
	cp $(keyboardDir)/$(kmnBase)*.html $(packageName)-$(keyboardVersion)
	cp $(keyboardDir)/$(kmnBase)*.png $(packageName)-$(keyboardVersion)
	cp $(keyboardDir)/$(kmnBase)*.bmp $(packageName)-$(keyboardVersion)
	tar -zcf $(packageName)_$(keyboardVersion).tar.gz $(packageName)-$(keyboardVersion)
	rm -rf $(packageName)-$(keyboardVersion)

version.mk : $(keyboardDir)/$(kmnFile) debian/changelog
	grep NAME $(keyboardDir)/$(kmnFile) | sed "s/.*Pa\-Oh \(.*\) Unicode.*/keyboardVersion=\\1/" >version.mk
	echo pkgVersion=`head -n 1 debian/changelog | sed 's/[^\(]*(\(.*\)).*/\1/'` >> $@

clean:
	rm -rf $(packageName)-$(keyboardVersion)
	rm -f $(packageName)_$(pkgVersion)*

cleanall: clean
	rm -rf $(packageName)_$(keyboardVersion)*tar.gz

install:
	dpkg -i $(packageName)_$(pkgVersion)_all.deb

uninstall:
	dpkg -r $(packageName)

test:
	kmfltest	$(keyboardDir)/$(kmnFile) $(kmnBase)Test.txt

