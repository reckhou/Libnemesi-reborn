#!/usr/bin/perl -w

use strict;

$SIG{INT} = \&int_handler;

my $preffix = shift
    or die "usage: $0 <file-preffix> [elapse]\n";
my $elapse = shift || 20; 
my ($es, $stop);
my $url = 'rtsp://192.168.0.119/axis-media/media.amp?videocodec=h264';

$elapse = $ARGV[1] if defined $ARGV[1];

foreach (1..20) {
    last if $stop;

    $es = $preffix . $_;
    print "start dump $es\n";
    system "./dump_stream -f $es $url &";

    sleep $elapse;
}

print "\nstot dump $es\n";
&kill_it();

sub kill_it {
	my @info = `ps`;
	for (@info) {
		if (m/dump_stream/) {
			my ($pid, $tty, $time, $cmd) = split;
			system "kill $pid";
		}
	}
}

sub int_handler {
    $stop = 1;
}
