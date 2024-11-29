#!/usr/bin/perl
#***************************************************************************************************
# [Program]     DailyBuild Linux
# [Date]        2019-04-08
# [Author]      Aaron Zhang
# [Description]
#   The script is used for automatic release.
# [Modify Record]
#
#***************************************************************************************************
use strict;
use threads;
use threads::shared;
use Cwd;
use utf8;
use Encode;
use Net::SMTP;
use POSIX qw(strftime);
use Date::Calc qw/Delta_Days/;#get days
use Date::Calc qw/Delta_DHMS/;#get day hour minite second;

#binmode(STDIN, ':encoding(utf8)');
#binmode(STDOUT, ':encoding(utf8)');
#binmode(STDERR, ':encoding(utf8)');

################################################################################
############################# get parameter ####################################
################################################################################
my $BranchName = "master";

my $DateNoSymbol=strftime "%Y%m%d",localtime;
#my $DateNoSymbol=strftime "%Y%m%d",localtime(time+1*24*3600);;
my $startTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
print "startTime: $startTime\r\n";

################################################################################
############################## git server ######################################
################################################################################
my $gitServer="http://gerrit.mediatek.inc:8080/ecnt";

################################################################################
############################## dailybuild pre  #################################
################################################################################
my @cutFolder;
my $dir = getcwd;
my $datePath="$dir/linux_trunk_log/app/$DateNoSymbol";
my $compileRet="$datePath/compile_result.txt";
my $retValue;
my $curTime=strftime "%Y-%m-%d %H:%M:%S",localtime;

	
print "000000 dir: $dir\r\n";
chdir("$dir");
system("pwd");
if(-e "$dir/.repo" && -e "$dir/tclinux_phoenix" ){
	print "tclinux_phoenix repo exsit\r\n";
}else{
	print "repo not exsit\r\n";
}

if(-e "$datePath"){
	&system_cmd("rm -rf $datePath");
	&system_cmd("mkdir -p $datePath");
}else{
	&system_cmd("mkdir -p $datePath");
}				
system("echo release start: $curTime >> $compileRet");

my $isReleaseFail_exit = $ARGV[0];
my $profile_selest_num=@ARGV;
my $profile_selest;
my $profile_num = 1;
while($profile_num<$profile_selest_num) {
	$profile_selest = "$profile_selest $ARGV[$profile_num]";
	$profile_num++;
}

my $buildBaseSrc="$dir/tclinux_phoenix";     
chdir("$buildBaseSrc");	 
my $release_all_check_log="$dir/release_all_check_log";     

#$retValue = &system_cmd("(echo $profile_selest && echo y) | fakeroot make -f Project/MakeFile_Release_APP CUSTOM=CT RELEASEBSP=y MSDK=1 confirm_infor");
#$curTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
#if($retValue eq 0){
#		print "confirm_infor success\r\n";
#		system("echo release_all_check success:$curTime >> $compileRet");
#}else {
#		print "confirm_infor fail\r\n";
#		system("echo release_all_check fail:$curTime >> $compileRet");
#		exit(1);
#}

#my $relList="Compile_List.txt";
my $relList="./.profile_select.tmp";
my @relCommand;
open(relfile,"$relList");
while(my $line=<relfile>) {
	push @relCommand,$line;
}
close relfile;
foreach my $relCommand (@relCommand){
	$relCommand =~ s/\n//g;
	print "000000 relCommand:$relCommand\r\n";
}

my $imagePath="$datePath/image";
my $logPath="$datePath/success";
my $logFailPath="$datePath/fail";
my $compileRel="$datePath/compile_success.txt";
my $compileFailRel="$datePath/compile_fail.txt";
if(! -e "$imagePath"){
	&system_cmd("mkdir -p $imagePath");
}
if(! -e "$logPath"){
	&system_cmd("mkdir -p $logPath");
}
if(! -e "$logFailPath"){
	&system_cmd("mkdir -p $logFailPath");
}

################################################################################
############################## dailybuild task #################################
################################################################################
my @threads;
my $countCom :shared=0;
my $countThread :shared=0;
my $queueCom :shared;
my $timeCom :shared;
my $count=0;
                
foreach my $relCommand (@relCommand){
	$count++;
	$relCommand =~ s/\n//g;
	print "000000 count:$count relCommand:$relCommand\r\n";
	my $thread=threads->create(\&msdk_releasebsp_flow,$dir,$DateNoSymbol,$relCommand,$count);
	print "000000 thread: $thread\r\n";
	push @threads,$thread;
	if($count >= 1){
		sleep(5);
	}		
}

foreach my $thread (@threads){
	$thread->join();
	print "000000 Over release thread $thread\r\n";
}

my $buildLog_releasTar="$dir/linux_trunk_log/app/$DateNoSymbol/releasTar.log";
if($isReleaseFail_exit eq 1){
	if( -e "$compileFailRel"){
		print "release SDK fail:release_backup fail\r\n";
		exit(1);
	}
}

chdir("$buildBaseSrc");	 
#&system_cmd("rm -rf $dir/BSP_*");

$curTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
system("echo before release_tar $curTime  >> $compileRet");
$retValue = &system_cmd("osbnq64 fakeroot make -f Project/MakeFile_Release_APP CUSTOM=CT RELEASEBSP=y MSDK=1 release_profile_app release_makefile_app release_webpage release_swept_app release_tar",$buildLog_releasTar);
$curTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
if($retValue eq 0){
		print "release SDK success\r\n";
		system("echo release SDK success:$curTime  >> $compileRet");
}else {
		print "release SDK fail\r\n";
		system("echo release SDK:$curTime >> $compileRet");
}

$curTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
system("echo release end:$curTime  >> $compileRet");

my $endTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
#my $spendTime=&get_spend_time($startTime,$endTime);
#print "spendTime:$spendTime.\r\n";

sub msdk_releasebsp_flow(){
	my $codePath=shift;
	my $date=shift;
	my $buildProfile=shift;
	my $countNum=shift;
	my $custom;
	my $profile;
	my $buildBase;	
	my $buildCom="osbnq64 fakeroot make -f Project/MakeFile_Release_APP CUSTOM=CT PROFILE=$buildProfile MSDK=1 release_backup_app BAK_PATH=$buildBaseSrc";
	my $value_ret;
	my $startTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
	my $str = "7523";

	if($buildProfile =~ /$str/)
	{
		$buildCom="osbnq64 fakeroot make -f Project/MakeFile_Release_APP CUSTOM=CT PROFILE=$buildProfile MSDK=1 release_backup_app BAK_PATH=$buildBaseSrc";
	}
	print "compilecmd=$buildCom";

	print "countNum:$countNum startTime:$startTime.\r\n";
	if($buildCom =~ /CUSTOM=(.*?)\s+PROFILE=(.*?)\s+(.*)/){
		$custom=$1;
		$profile=$2;
		$profile=~ s/\s+//g;
	}
	print "$countNum custom:$custom profile:$profile\r\n";

	#&system_cmd("chmod 777 -R $codePath/BSP_$countNum");
	
	$buildBase="$codePath/BSP_$countNum/tclinux_phoenix";

	my $buildLog=$logPath."/releasebsp_".$countNum."_".$custom."_".$profile.".log";
	my $buildLog_rm=$logPath."/releasebsp_".$countNum."_rm_".$custom."_".$profile.".log";

	my $imageBackPath=$imagePath."/releasebsp_".$countNum."_".$custom."_".$profile."_Version";
	print "$countNum $codePath/BSP_$countNum $profile compiling ...\r\n";
	while(1){
		my $nowTime=strftime "%d%H%M%S",localtime;
		my $lastTime;
		lock($timeCom);
		if($timeCom =~ /(.*),(.*),/){
			$lastTime=$2;
		}elsif($timeCom =~ /(.*),/){
			$lastTime=$1;
		}	
		print "$countNum bsp nowTime:$nowTime lastTime:$lastTime timeCom:$timeCom\r\n";
		if($timeCom =~ /$nowTime/){
			print "$countNum bsp timeCom contai nowTime, sleep 1s\r\n";
			sleep(1);
	}else{
			if($nowTime-$lastTime > 20){
				my $nowTime=strftime "%d%H%M%S",localtime;
				lock($timeCom);
			$timeCom.=$nowTime.",";
			last;
	}
	}
		sleep(1);
	}
	my $nowTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
	print "$countNum bsp nowTime:$nowTime\r\n";
	print "$countNum bsp timeCom:$timeCom\r\n";	 
	chdir("$buildBase");	                                         	
	&system_cmd_echo("echo '$buildBase'",$buildLog);
	
	print "buildCom=$buildCom\r\n";
	#&system_cmd("fakeroot make -f app_bsp/MakeFile_Main_BSP CUSTOM=CT PROFILE=$buildProfile copy_project_tools_for_app_clean");

	my $value=&system_cmd("$buildCom",$buildLog);
	my $endTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
	my $spendTime=&get_spend_time($startTime,$endTime);
	print "countNum:$countNum endTime:$endTime.\r\n";
	print "countNum:$countNum spendTime:$spendTime.\r\n";
	if($value eq 0){
		if(! -e " $buildBaseSrc/release_app"){
			print "$buildBaseSrc/release_app is not exist\r\n";
			system("pwd");
			&system_cmd("mkdir $buildBaseSrc/release_app");	
		}
		system_cmd("cp -rf $buildBase/release_app/* $buildBaseSrc/release_app/");
		print "$countNum $codePath/BSP_$countNum $custom $profile releasebsp compile success\r\n";
		system("echo $custom $profile releasebsp compile success,  startTime:$startTime >> $compileRel");
		system("echo $custom $profile releasebsp compile success,  spendTime:$spendTime >> $compileRel");
		system("echo $custom $profile releasebsp compile success,  endTime:$endTime >> $compileRel");

#		system("mkdir $imageBackPath");
#		system("cp -rf $buildBase/Project/images/tc* $imageBackPath");
		
		$value_ret=&system_cmd("rm -rf $codePath/BSP_$countNum",$buildLog_rm);	
		$curTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
		system("echo $custom $profile releasebsp compile success,  rm $codePath/BSP_$countNum:$curTime,value_ret:$value_ret >> $compileRel");
	
	}else {
		print "$countNum $codePath/BSP_$countNum $custom $profile releasebsp compile fail\r\n";
		system("echo $custom $profile releasebsp compile success,  startTime:$startTime >> $compileFailRel");
		system("echo $custom $profile releasebsp compile fail, spendTime:$spendTime >> $compileFailRel");
		system("echo $custom $profile releasebsp compile success,  endTime:$endTime >> $compileFailRel");

		system("mv $buildLog $logFailPath");
#		$value_ret=&system_cmd("rm -rf $codePath/BSP_$countNum",$buildLog_rm);	
		$curTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
		system("echo $custom $profile releasebsp compile fail,  rm $codePath/BSP_$countNum curTime:$curTime ,value_ret:$value_ret >> $compileFailRel");
	}
	
return 0;
}


sub system_cmd() {
	my $cmd = shift;
	my $log = shift;
	my $startTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
	my $value;
	if($log eq ""){
		$value=system("$cmd");
	}else{
		system("echo \"++ $cmd\" >> $log 2>&1");
		$value=system("$cmd >> $log 2>&1");
	}
	if($value == 0) {
		if($log ne ""){
			my $endTime=strftime "%Y-%m-%d %H:%M:%S",localtime;
			my $spendTime=&get_spend_time($startTime,$endTime);
			system("echo \"$cmd ok, spend $spendTime.\" >> $log 2>&1");
		}
		print "$cmd ok\r\n";
		return 0;	
	} else {
		if($log ne ""){
			system("echo '$cmd fail' >> $log 2>&1");
		}
		print "$cmd fail\r\n";
		return 1;
	}		 
}

sub system_cmd_echo() {
	my $cmd = shift;
	my $log = shift;
	my $value;
	if($log eq ""){
		$value=system("$cmd");
	}else{
		$value=system("$cmd >> $log 2>&1");
	}
	if($value == 0) {
#		print "$cmd ok\r\n";
		return 0;	
	} else {
		print "$cmd fail\r\n";
		return 1;
	}		 
}

sub get_spend_time(){
	my $startTime=shift;
	my $endTime=shift;
	my ($startYear,$startMonth,$startDay,$startHour,$startMin,$startSecond);
	my ($endYear,$endMonth,$endDay,$endHour,$endMin,$endSecond);
	my $spendTime;
 	if($startTime =~ /(.*)-(.*)-(.*)\s(.*):(.*):(.*)/){
 		($startYear,$startMonth,$startDay,$startHour,$startMin,$startSecond)=($1,$2,$3,$4,$5,$6);
 	}
 	if($endTime =~ /(.*)-(.*)-(.*)\s(.*):(.*):(.*)/){
 		($endYear,$endMonth,$endDay,$endHour,$endMin,$endSecond)=($1,$2,$3,$4,$5,$6);
 	}
# my $dayT=Delta_Days(2018,03,06,2018,03,06);
#	my ($day,$hour,$min,$sec)=Delta_DHMS(2018,03,06,03,03,03,2018,03,06,23,23,23);
	my ($day,$hour,$min,$sec)=Delta_DHMS($startYear,$startMonth,$startDay,$startHour,$startMin,$startSecond,$endYear,$endMonth,$endDay,$endHour,$endMin,$endSecond);
	if($day ne 0){
		$spendTime.=$day." d ";
	}
	if($hour ne 0){
		$spendTime.=$hour." h ";
	}else{
		if($day ne 0){
			$spendTime.=$hour." h ";
		}
	}
	$spendTime.=$min." m ".$sec." s";
	return $spendTime;
}



