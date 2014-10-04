#!/usr/bin/perl

# This is a script to convert Berkeley Parser "split" files to Egret stateInfo
# files.

use strict;
use utf8;
use FileHandle;
use Getopt::Long;
use List::Util qw(sum min max shuffle);
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

if(@ARGV != 2) {
    print STDERR "Usage: $0 CURR LAST\n";
    exit 1;
}

open FILE0, "<:utf8", $ARGV[0] or die "Couldn't open $ARGV[0]";
open FILE1, "<:utf8", $ARGV[1] or die "Couldn't open $ARGV[1]";

my ($s0, $s1);
my $id = 0;
print "ROOT\t".($id++)."\t1 [0 [0 [0 [0 [0 [0 0]]]]]]\n";
while(($s0 = <FILE0>) and ($s1 = <FILE1>)) {
    chomp $s0; chomp $s1;
    my ($p0, $t0) = split(/\t/, $s0);
    my ($p1, $t1) = split(/\t/, $s1);
    die "Mismatched tags $p0 $p1" if $p0 ne $p1;
    my $size = 0;
    $t0 =~ /(\d+)\)*$/ or die $t0;
    $size = $1+1;
    # print STDERR "T0=$t0 -> $size\n";
    $t1 =~ tr/()/[]/;
    print "$p0\t".($id++)."\t$size $t1\n";
}
