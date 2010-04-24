#******************************************************************************
#FILE:				~/src/tools/Makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This Makefile tells how to compile the tools.
#
#	Input variables are:
#
#       $(MAKEDIR)      where the makefiles are located.
#
#       $(PREFIX)       where the libraries are installed.
#                       It will be created a subdirectory of 
#                       $(PREFIX)/lib for each library compiled
#                       by this makefile.
#
#	Output:
#
#		$(PREFIX)/bin/$(PROGRAM)           the executables.
#		$(PREFIX)/lib/$(MODULE)/interfaces/    the interface of the module,
#		$(PREFIX)/lib/$(MODULE)/documentation/ the documentation of the module,
#		$(PREFIX)/lib/$(MODULE)/libraries/     the libraries of the module.
#
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#   2001-06-19 <PJB> Reorganized and simplified.
#LEGAL
#	Copyright Pascal J. Bourguignon 1992 - 2001
#
#   This script is free software; you can redistribute it and/or
#   modify it under the terms of the GNU  General Public
#   License as published by the Free Software Foundation; either
#   version 2 of the License, or (at your option) any later version.
#
#   This script is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this library; see the file COPYING.LIB.
#   If not, write to the Free Software Foundation,
#   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#******************************************************************************
TARGET  := $(shell uname)
PREFIX  := /usr/local
MAKEDIR := $(HOME)/src/public/common/makedir
#MAKEDIR := $(shell dirname `pwd`)/common/makedir

MODULES=\
	ascii-utilities \
	base \
	bocrypt \
	cookie \
	cplusinherited \
	dosname \
	encoding \
	environment \
	extractbit1 \
	filldisk \
	formats \
	geturls \
	line-utilities \
	mpa \
	network \
	newmoon \
	newton \
	objc-tools \
	od-tools \
	primes \
	random-stat \
	romain \
	segment \
	sum32 \
	sumascii \
	truncate 


include $(MAKEDIR)/project

m1:
	make PREFIX=$(HOME)/install \
		 COMMON=$(HOME)/src-new/common \
		 MAKEDIR=$(HOME)/src-new/common/makedir \
		cvsclean depend install

m1i:
	make PREFIX=$(HOME)/install \
		 COMMON=$(HOME)/src-new/common \
		 MAKEDIR=$(HOME)/src-new/common/makedir \
		install

m2:
	make PREFIX=/local \
		 COMMON=$(HOME)/src-new/common \
		 MAKEDIR=$(HOME)/src-new/common/makedir \
		cvsclean depend install 

#### Makefile                         --                     --          ####
