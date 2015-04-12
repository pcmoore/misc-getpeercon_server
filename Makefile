#
# getpeercon utilities makefile
#
# Copyright (c) 2015 Red Hat <pmoore@redhat.com>
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

.PHONY: all tarball build install clean

SUBDIRS_BUILD = src selinux
SUBDIRS_INSTALL = src selinux

TARBALL="getpeercon_server-$$(date +%Y%m%d).tar.gz"

all:
	@echo ">> make targets: tarball build install clean"

tarball: clean
	dir=$$(basename "$$(pwd)"); \
	cd ..; tar -zcf $(TARBALL) \
		--transform "s/^"$$dir"/getpeercon_server/" \
		--exclude ".git*" --exclude "*~" "$$dir"

build: $(SUBDIRS_BUILD)
	for i in $^; do \
		$(MAKE) -C $$i build; \
	done

install: $(SUBDIRS_INSTALL)
	for i in $^; do \
		$(MAKE) -C $$i install; \
	done
	for i in $^; do \
		$(MAKE) -C $$i selinux; \
	done

clean: $(SUBDIRS_BUILD)
	for i in $^; do \
		$(MAKE) -C $$i clean; \
	done
