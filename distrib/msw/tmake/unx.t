#!################################################################################
#! File:    unx.t
#! Purpose: tmake template file from which makefile.unx.in is generated by running
#!          tmake -t unx wxwin.pro -o makefile.unx.in
#! Author:  Vadim Zeitlin, Robert Roebling, Julian Smart
#! Created: 14.07.99
#! Version: $Id$
#!################################################################################

#${
    #! include the code which parses filelist.txt file and initializes
    #! %wxCommon, %wxGeneric, %wxHtml, %wxUnix and %wxGTK hashes.
    IncludeTemplate("filelist.t");

    #! now transform these hashes into $project tags
    foreach $file (sort keys %wxGeneric) {
        next if $wxGeneric{$file} =~ /\bR\b/;

        $file =~ s/cp?p?$/\o/;
        $project{"WXGTK_GENERICOBJS"} .= "../generic/" . $file . " "
    }

    #! now transform these hashes into $project tags
    foreach $file (sort keys %wxGeneric) {
        next if $wxCommon{$file} =~ /\bX\b/;

        $file =~ s/cp?p?$/\o/;
        $project{"WXMOTIF_GENERICOBJS"} .= "../generic/" . $file . " "
    }

    foreach $file (sort keys %wxCommon) {
        next if $wxCommon{$file} =~ /\bR\b/;

        $file =~ s/cp?p?$/\o/;
        $project{"WXGTK_COMMONOBJS"} .= "../common/" . $file . " "
    }

    foreach $file (sort keys %wxCommon) {
        next if $wxCommon{$file} =~ /\bX\b/;

        $file =~ s/cp?p?$/\o/;
        $project{"WXMOTIF_COMMONOBJS"} .= "../common/" . $file . " "
    }

    foreach $file (sort keys %wxGTK) {
        $file =~ s/cp?p?$/\o/;
        $project{"WXGTK_GUIOBJS"} .= "../gtk/" . $file . " "
    }

    foreach $file (sort keys %wxMOTIF) {
        $file =~ s/cp?p?$/\o/;
        $project{"WXMOTIF_GUIOBJS"} .= "../motif/" . $file . " "
    }

    foreach $file (sort keys %wxHTML) {
        $file =~ s/cp?p?$/\o/;
        $project{"WXHTMLOBJS"} .= "../html/" . $file . " "
    }

    foreach $file (sort keys %wxUNIX) {
        $file =~ s/cp?p?$/\o/;
        $project{"WXUNIXOBJS"} .= "../unix/" . $file . " "
    }
    
#$}
#
#    I want this to be:
#	$(INSTALL_DATA) $(INCDIR)/wx/window.h $(includedir)/wx/window.h
#
#    foreach $file (sort keys %wxINCLUDE) {
#        next if $wxINCLUDE{$file} =~ /\b(GTK|MSW|MOT|PM|MAC|GEN|HTM|UNX)\b/;
#    
#        $project{"WXINSTALLWX"} .= "$(INSTALL_DATA)" . " " . "\$(INCDIR)/wx/" . $file . " \$(includedir)/wx/" . $file . "\n"
#    }
# 
#
#
# This file was automatically generated by tmake at #$ Now()
# DO NOT CHANGE THIS FILE, YOUR CHANGES WILL BE LOST! CHANGE UNX.T!

#
# File:     makefile.unx
# Author:   Julian Smart, Robert Roebling, Vadim Zeitlin
# Created:  1993
# Updated:  1999
# Copyright:(c) 1993, AIAI, University of Edinburgh,
# Copyright:(c) 1999, Vadim Zeitlin
# Copyright:(c) 1999, Robert Roebling
#
# Makefile for libwx_gtk.a, libwx_motif.a and libwx_msw.a

###################################################################

include ../make.env

############## override make.env for PIC ##########################

# Clears all default suffixes
.SUFFIXES:	.o .cpp .c .cxx

.c.o :
	$(CCC) -c $(CFLAGS) $(PICFLAGS) -o $@ $<

.cpp.o :
	$(CC) -c $(CPPFLAGS) $(PICFLAGS) -o $@ $<

.cxx.o :
	$(CC) -c $(CPPFLAGS) $(PICFLAGS) -o $@ $<

########################### Paths #################################

srcdir = @srcdir@

VPATH = :$(srcdir)

top_srcdir = @top_srcdir@
prefix = @prefix@
exec_prefix = @exec_prefix@

bindir = @bindir@
sbindir = @sbindir@
libexecdir = @libexecdir@
datadir = @datadir@
sysconfdir = @sysconfdir@
sharedstatedir = @sharedstatedir@
localstatedir = @localstatedir@
libdir = @libdir@
infodir = @infodir@
mandir = @mandir@
includedir = @includedir@
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/@PACKAGE@
pkglibdir = $(libdir)/@PACKAGE@
pkgincludedir = $(includedir)/@PACKAGE@

top_builddir = ../..

INSTALL = @INSTALL@
INSTALL_PROGRAM = @INSTALL_PROGRAM@
INSTALL_DATA = @INSTALL_DATA@
INSTALL_SCRIPT = @INSTALL_SCRIPT@
transform = @program_transform_name@

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_alias = @build_alias@
build_triplet = @build@
host_alias = @host_alias@
host_triplet = @host@
target_alias = @target_alias@
target_triplet = @target@

############################# Dirs #################################

WXDIR = $(srcdir)/../..

# Subordinate library possibilities

GENDIR  = $(WXDIR)/src/generic
COMMDIR = $(WXDIR)/src/common
HTMLDIR = $(WXDIR)/src/html
UNIXDIR = $(WXDIR)/src/unix
PNGDIR  = $(WXDIR)/src/png
JPEGDIR = $(WXDIR)/src/jpeg
ZLIBDIR = $(WXDIR)/src/zlib
GTKDIR  = $(WXDIR)/src/gtk
MOTIFDIR = $(WXDIR)/src/motif
INCDIR  = $(WXDIR)/include

DOCDIR = $(WXDIR)/docs

############################## Files ##################################

GTK_GENERICOBJS = \
                #$ ExpandList("WXGTK_GENERICOBJS");

GTK_COMMONOBJS  = \
		parser.o \
		#$ ExpandList("WXGTK_COMMONOBJS");

GTK_GUIOBJS     = \
		#$ ExpandList("WXGTK_GUIOBJS");

MOTIF_GENERICOBJS = \
                #$ ExpandList("WXMOTIF_GENERICOBJS");

MOTIF_COMMONOBJS  = \
		parser.o \
		#$ ExpandList("WXMOTIF_COMMONOBJS");

MOTIF_GUIOBJS     = \
                ../motif/xmcombo/xmcombo.o \
		#$ ExpandList("WXMOTIF_GUIOBJS");

HTMLOBJS = \
                #$ ExpandList("WXHTMLOBJS");

UNIXOBJS     = \
		#$ ExpandList("WXUNIXOBJS");

ZLIBOBJS    = \
		../zlib/adler32.o \
		../zlib/compress.o \
		../zlib/crc32.o \
		../zlib/gzio.o \
		../zlib/uncompr.o \
		../zlib/deflate.o \
		../zlib/trees.o \
		../zlib/zutil.o \
		../zlib/inflate.o \
		../zlib/infblock.o \
		../zlib/inftrees.o \
		../zlib/infcodes.o \
		../zlib/infutil.o \
		../zlib/inffast.o

PNGOBJS     = \
		../png/png.o \
		../png/pngread.o \
		../png/pngrtran.o \
		../png/pngrutil.o \
		../png/pngpread.o \
		../png/pngtrans.o \
		../png/pngwrite.o \
		../png/pngwtran.o \
		../png/pngwutil.o \
		../png/pngerror.o \
		../png/pngmem.o \
		../png/pngwio.o \
		../png/pngrio.o \
		../png/pngget.o \
		../png/pngset.o


JPEGOBJS    = \
		../jpeg/jcomapi.o \
		../jpeg/jutils.o \
		../jpeg/jerror.o \
		../jpeg/jmemmgr.o \
		../jpeg/jmemnobs.o \
		../jpeg/jcapimin.o \
		../jpeg/jcapistd.o \
		../jpeg/jctrans.o \
		../jpeg/jcparam.o \
		../jpeg/jdatadst.o \
		../jpeg/jcinit.o \
		../jpeg/jcmaster.o \
		../jpeg/jcmarker.o \
		../jpeg/jcmainct.o \
		../jpeg/jcprepct.o \
		../jpeg/jccoefct.o \
		../jpeg/jccolor.o \
		../jpeg/jcsample.o \
		../jpeg/jchuff.o \
		../jpeg/jcphuff.o \
		../jpeg/jcdctmgr.o \
		../jpeg/jfdctfst.o \
		../jpeg/jfdctflt.o \
		../jpeg/jfdctint.o \
		../jpeg/jdapimin.o \
		../jpeg/jdapistd.o \
		../jpeg/jdtrans.o \
		../jpeg/jdatasrc.o \
		../jpeg/jdmaster.o \
		../jpeg/jdinput.o \
		../jpeg/jdmarker.o \
		../jpeg/jdhuff.o \
		../jpeg/jdphuff.o \
		../jpeg/jdmainct.o \
		../jpeg/jdcoefct.o \
		../jpeg/jdpostct.o \
		../jpeg/jddctmgr.o \
		../jpeg/jidctfst.o \
		../jpeg/jidctflt.o \
		../jpeg/jidctint.o \
		../jpeg/jidctred.o \
		../jpeg/jdsample.o \
		../jpeg/jdcolor.o \
		../jpeg/jquant1.o \
		../jpeg/jquant2.o \
		../jpeg/jdmerge.o


OBJECTS = $(@GUIOBJS@) $(@COMMONOBJS@) $(@GENERICOBJS@) $(HTMLOBJS) $(UNIXOBJS) \
	  $(JPEGOBJS) $(PNGOBJS) $(ZLIBOBJS)


REQUIRED_DIRS = ../../lib ../../src ../../src/common ../../src/gtk ../../src/motif \
                ../../src/generic ../../src/unix ../../src/motif/xmombo ../../src/html \
		../../src/zlib ../../src/jpeg ../../src/png

all:    $(REQUIRED_DIRS) $(OBJECTS) @WX_TARGET_LIBRARY@ @WX_CREATE_LINKS@

$(REQUIRED_DIRS):	$(WXDIR)/include/wx/defs.h $(WXDIR)/include/wx/object.h $(WXDIR)/include/wx/setup.h
	@if test ! -d ../../lib; then mkdir ../../lib; fi
	@if test ! -d ../../src; then mkdir ../../src; fi
	@if test ! -d ../../src/common; then mkdir ../../src/common; fi
	@if test ! -d ../../src/gtk; then mkdir ../../src/gtk; fi
	@if test ! -d ../../src/motif; then mkdir ../../src/motif; fi
	@if test ! -d ../../src/motif/xmcombo; then mkdir ../../src/motif/xmcombo; fi
	@if test ! -d ../../src/generic; then mkdir ../../src/generic; fi
	@if test ! -d ../../src/unix; then mkdir ../../src/unix; fi
	@if test ! -d ../../src/html; then mkdir ../../src/html; fi
	@if test ! -d ../../src/png; then mkdir ../../src/png; fi
	@if test ! -d ../../src/jpeg; then mkdir ../../src/jpeg; fi
	@if test ! -d ../../src/zlib; then mkdir ../../src/zlib; fi

@WX_LIBRARY_NAME_STATIC@:  $(OBJECTS)
	$(AR) $(AROPTIONS) ../../lib/$@ $(OBJECTS)
	$(RANLIB) ../../lib/$@

@WX_LIBRARY_NAME_SHARED@:  $(OBJECTS)
	$(SHARED_LD) ../../lib/$@ $(OBJECTS) $(EXTRALIBS)
	
CREATE_LINKS:  $(OBJECTS)
	@if test -e ../../lib/@WX_LIBRARY_LINK1@; then rm -f ../../lib/@WX_LIBRARY_LINK1@; fi
	@if test -e ../../lib/@WX_LIBRARY_LINK2@; then rm -f ../../lib/@WX_LIBRARY_LINK2@; fi
	@if test -e ../../lib/@WX_LIBRARY_LINK3@; then rm -f ../../lib/@WX_LIBRARY_LINK3@; fi
	$(LN_S) @WX_TARGET_LIBRARY@ ../../lib/@WX_LIBRARY_LINK1@
	$(LN_S) @WX_TARGET_LIBRARY@ ../../lib/@WX_LIBRARY_LINK2@
	$(LN_S) @WX_TARGET_LIBRARY@ ../../lib/@WX_LIBRARY_LINK3@
	
$(OBJECTS):	$(WXDIR)/include/wx/defs.h $(WXDIR)/include/wx/object.h $(WXDIR)/include/wx/setup.h

parser.o:    parser.c lexer.c
	$(CCLEX) -c $(CFLAGS) -o $@ parser.c

parser.c:	$(COMMDIR)/parser.y lexer.c
	$(YACC) $(COMMDIR)/parser.y
	@sed -e "s;$(COMMDIR)/y.tab.c;parser.y;g" < y.tab.c | \
	sed -e "s/BUFSIZ/5000/g"            | \
	sed -e "s/YYLMAX 200/YYLMAX 5000/g" | \
	sed -e "s/yy/PROIO_yy/g"            | \
	sed -e "s/input/PROIO_input/g"      | \
	sed -e "s/unput/PROIO_unput/g"      > parser.c
	@$(RM) y.tab.c

lexer.c:	$(COMMDIR)/lexer.l
	$(LEX) $(COMMDIR)/lexer.l
	@sed -e "s;$(COMMDIR)/lex.yy.c;lexer.l;g" < lex.yy.c | \
	sed -e "s/yy/PROIO_yy/g"            | \
	sed -e "s/input/PROIO_input/g"      | \
	sed -e "s/unput/PROIO_unput/g"      > lexer.c
	@$(RM) lex.yy.c

samples: $(OBJECTS)
	@if test ! -e ../../samples/dialog/dialog.cpp; \
	    then cp -f -r $(WXDIR)/samples ../..;  \
	fi

install: @WX_TARGET_LIBRARY@
	#$ ExpandList("WXINSTALLWX");

clean:
	rm -f *.o
	rm -f *.lo
	rm -f parser.c
	rm -f lexer.c
	rm -f *.a
	rm -f *.la

cleanall: clean
