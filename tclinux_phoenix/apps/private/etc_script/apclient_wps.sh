#!/bin/sh
/userfs/bin/iwpriv $1 set ApCliEnable=1
/userfs/bin/iwpriv $1 set WscConfMode=1			
/userfs/bin/iwpriv $1 set WscMode=2
/userfs/bin/iwpriv $1 set WscGetConf=1

