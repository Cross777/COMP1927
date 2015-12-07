#!/usr/bin/perl -w

my %town_ids = ();
my $started = 0;
my $ended = 0;
foreach my $line (<STDIN>) {
    $ended = 1 if $started and $line eq "\n";
    if ($line =~ /define ([A-Z_]+) +([0-9]{1,2})$/) {
        my @parts = ( split /\s{2,}/, $line );
        my $town = $1;
        my $id = $2;
        $started = 0 if $started and $ended;
        $started = 1 if $town =~ /_/ and not $ended;
        if ($started) {
            $town_ids{$town} = $id;
        }
    }
}

my $arr = '{"none"';
foreach my $town (sort keys %town_ids) {
    my $id = $town_ids{$town};
    $arr .= ", ";
    $arr .= '"' . lc $town . '"';
}
print "$arr}\n";
