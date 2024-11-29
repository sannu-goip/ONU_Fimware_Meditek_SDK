#!/usr/bin/perl
use strict;

my $profile_selest_num=@ARGV;
my $profile_selest;
my $profile_num = 0;
while($profile_num<$profile_selest_num) {
	$profile_selest = "$profile_selest $ARGV[$profile_num]";
	$profile_num++;
}

printf "\r\n$profile_selest\n";

system("perl MS_SDK_BSP.pl $profile_selest");
system("perl MS_SDK_SDK.pl $profile_selest");

