HEADERS=track.hpp track-host.hpp track-users.hpp ipc.hpp block.hpp whitelist.hpp
MACROS=-DLISTEN_LIMIT=100 -DSOCKET_USER=0 -DSOCKET_GROUP=0 -DSOCKET_MODE=0600 -DBUFFER_SIZE=1024 -DMIN_UID=1000 -DVERBOSE -DLOGNAME=\"pampfd\"
CXXFLAGS=$(MACROS) -Wall -g -O3 -pipe
INSTALL_PATH=/usr/local

.PHONY: default clean install uninstall

default: pampfd pampf


pampfd: pampfd.cpp track-host.o track-users.o ipc.o block.o whitelist.o $(HEADERS) Makefile
	c++ $(CXXFLAGS) pampfd.cpp track-host.o track-users.o ipc.o block.o whitelist.o -o pampfd -lrt

pampf: pampf.cpp ipc.o $(HEADERS) Makefile
	c++ $(CXXFLAGS) pampf.cpp ipc.o -o pampf


track-host.o: track-host.cpp track-host.hpp track.hpp Makefile
	c++ $(CXXFLAGS) -c track-host.cpp

track-users.o: track-users.cpp track-users.hpp track.hpp Makefile
	c++ $(CXXFLAGS) -c track-users.cpp

ipc.o: ipc.cpp ipc.hpp track.hpp Makefile
	c++ $(CXXFLAGS) -c ipc.cpp

block.o: block.cpp block.hpp track.hpp Makefile
	c++ $(CXXFLAGS) -c block.cpp

whitelist.o: whitelist.cpp whitelist.hpp track.hpp Makefile
	c++ $(CXXFLAGS) -c whitelist.cpp


install: $(INSTALL_PATH)/sbin/pampfd $(INSTALL_PATH)/sbin/pampf $(INSTALL_PATH)/etc/rc.d/pampfd

uninstall:
	rm -fv $(INSTALL_PATH)/sbin/pampfd $(INSTALL_PATH)/sbin/pampf $(INSTALL_PATH)/etc/rc.d/pampfd


$(INSTALL_PATH)/sbin/pampfd: pampfd
	cp -v pampfd $(INSTALL_PATH)/sbin/pampfd && chown 0:0 $(INSTALL_PATH)/sbin/pampfd && chmod 700 $(INSTALL_PATH)/sbin/pampfd

$(INSTALL_PATH)/sbin/pampf: pampf
	cp -v pampf $(INSTALL_PATH)/sbin/pampf && chown 0:0 $(INSTALL_PATH)/sbin/pampf && chmod 755 $(INSTALL_PATH)/sbin/pampf

$(INSTALL_PATH)/etc/rc.d/pampfd: pampfd-rc
	cp -v pampfd-rc $(INSTALL_PATH)/etc/rc.d/pampfd && chown 0:0 $(INSTALL_PATH)/etc/rc.d/pampfd && chmod 750 $(INSTALL_PATH)/etc/rc.d/pampfd


clean:
	rm -fv pampfd pampf *~ *.o
