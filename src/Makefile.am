# [open]aptx - Makefile.am
# Copyright (c) 2017 Arkadiusz Bokowy

AM_CFLAGS = -I$(top_srcdir)/include -pthread -lm
AM_LDFLAGS = -avoid-version -Wl,-no-undefined -L$(top_builddir)/libavcodec -lavcodec -lthread -lm

lib_LTLIBRARIES = \
	libbt-aptx.la

libbt_aptx_la_SOURCES = \
	bt-aptx-stub-sonar.c \
	bt-aptx-stub.c

libbt_aptx_la_LDFLAGS = -no-undefined --whole-archive\
$(top_builddir)/libavcodec/libavcodec.a\
$(top_builddir)/libavutil/libavutil.a\
$(top_builddir)/libavformat/libavformat.a
	
bt-aptx-stub-sonar.c: $(srcdir)/sonar.aptx
	echo " \
		unsigned char sonar_aptx[] = { `xxd -i < $<` }; \
		unsigned int sonar_aptx_len = sizeof(sonar_aptx);" >$@
