#!/bin/sh -

make distclean

base=`pwd`
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$base/../netembryo-0.0.5
chmod 777 configure
./configure #--host=sh4-linux
			#--enable-static

find -name 'Makefile' | xargs \
	perl -pi -e \
	'if (m/DEFAULT_INCLUDES\s+=/) {
		print $_, "DEFAULT_INCLUDES += -I\${top_builddir}/../netembryo-0.0.5/include\n";
		$_ = "\n";
	}'

perl -pi -e \
	'if (m/LDFLAGS\s+=/) {
		print $_, "LDFLAGS += -L../netembryo-0.0.5/.libs";
		$_ = "\n";
	}' ./Makefile 
