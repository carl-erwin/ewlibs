#!/usr/bin/perl -w

use strict;

if ( @ARGV ne 3 )
{
    print STDERR "usage $0 start nr_line line_width\n";
    exit(1);
}

my $start_line=$ARGV[0];
my $nr=$ARGV[1];
my $sz=$ARGV[2];

my $line="";
for (my $j=1; $j <= ${sz}; $j++)
{
        $line .= "x";
}

my $line_num=$start_line;
for (my $i=1; $i <= ${nr}; $i++)
{
    print STDOUT "$line_num $line\n";
    $line_num++;
}
