#
# getpeercon_server makefile
#
# Copyright (c) 2012 Red Hat <pmoore@redhat.com>
# Author: Paul Moore <paul@paul-moore.com>
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

.PHONY: build
build: getpeercon_server

.PHONY: install
install: build
	install -m 0755 getpeercon_server $(DESTDIR)/usr/bin/getpeercon_server

.PHONY: selinux
selinux: install
	restorecon $(DESTDIR)/usr/bin/getpeercon_server

getpeercon_server: getpeercon_server.c
	$(CC) $(CFLAGS) -g -o $@ $< -lselinux

.PHONY: clean
clean:
	$(RM) getpeercon_server
