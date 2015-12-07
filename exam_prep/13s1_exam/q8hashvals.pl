#!/usr/bin/perl -w

my @nums = (3, 26, 16, 4, 29, 12, 25, 13, 7, 20);
foreach (@nums) {
	$i = $_ % 13;
	$j = 7-($_ % 7);
	print "$_: $i\n";
	print "$_: $j\n\n";
}
