#
# getpeercon utilities makefile
#
# Copyright (c) 2012 Red Hat <pmoore@redhat.com>
# Author: Paul Moore <pmoore@redhat.com>
#

#
# This program is free software: you can redistribute it and/or modify it under
# the terms of version 2 of the GNU General Public License as published by the
# Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

.PHONY: all tarball clean

TARBALL="getpeercon_server-$$(date +%Y%m%d).tar.gz"

all: getpeercon_server

tarball: clean
	cd ..; tar -zcvf $(TARBALL) --exclude "*~" getpeercon_server/

getpeercon_server: getpeercon_server.c
	$(CC) $(CFLAGS) -o $@ -lselinux $<

clean:
	$(RM) getpeercon_server
