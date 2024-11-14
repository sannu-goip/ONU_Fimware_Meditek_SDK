#!/bin/sh

/userfs/bin/tcapi set InfoVoIP_Common SC_START_STATUS "No"
/userfs/bin/tcapi set InfoVoIPH248_Common START_STATUS "No"
usr/bin/killall sipclient 2>/dev/null
usr/bin/killall mgapp 2>/dev/null
usr/bin/killall tcVoIPApiServer 2>/dev/null
usr/bin/killall -9 sipclient 2>/dev/null
usr/bin/killall -9 mgapp 2>/dev/null
usr/bin/killall -9 tcVoIPApiServer 2>/dev/null
/bin/rm -f /var/run/voip_sip.pid
sleep 3
rmmod pcmDump
rmmod ovdsp
rmmod foip
rmmod acodec_x
rmmod ortp
rmmod ksocket
rmmod slic3
rmmod slic3_silicon
rmmod slic3_intel
rmmod fxs3
rmmod fxs3_silicon
rmmod spi
rmmod lec
rmmod pcm1
rmmod DSPCore
rmmod sys_mod
