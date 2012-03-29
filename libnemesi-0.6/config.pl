#!/usr/bin/perl -w
# distclean and re-configure libnemesi in sh4-linux-uclibc environment.
# it need libnetembyro-0.0.5 in upper directory.

use strict;
use File::Find;

system "make distclean" and die "\n";

$ENV{PKG_CONFIG_PATH} = "$ENV{PKG_CONFIG_PATH}:../netembryo-0.0.5";

system "./configure --host=sh4-linux-uclibc" and die "\n";

find(\&proc_makefile, "./");
exit 0;

sub proc_makefile {
    my $full_name = $File::find::name;
    {
        open FH, "<", $full_name or die "Can't open file: $full_name\n";
        local $/ = undef;
        my $content = <FH>;
        close FH;
    }

    $content =~ s{
        (\b DEFAULT_INCLUDES \s+ \+?= .*)
    }{$1 -I\$top_builddir/../netembryo-0.0.5/include}x;

    $content =~ s{
        (\b LDFL \s+ \+?= .*)
    }{$1 -I\$top_builddir/../netembryo-0.0.5/include}x;
}

