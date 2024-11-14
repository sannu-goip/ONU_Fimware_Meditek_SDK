#!/bin/sh

echo "run script for MT7592 anti interference config"
/userfs/bin/tcapi set WLan_Common BSSAifsn "\"1;3;7;2\""
/userfs/bin/tcapi set WLan_Common BSSCwmin "\"2;4;3;2\""
/userfs/bin/tcapi set WLan_Common BSSCwmax "\"2;10;4;3\""
/userfs/bin/tcapi set WLan_Common BSSTxop "\"47;0;94;49\""
/userfs/bin/tcapi set WLan_Common APAifsn "\"7;7;1;1\""
/userfs/bin/tcapi set WLan_Common APCwmax "\"10;10;4;3\""
/userfs/bin/tcapi save
sleep 1

if [ "TCSUPPORT_ECNT_MAP" != "" ] ;then
echo "tcapi commit mesh_dat"
/userfs/bin/tcapi commit mesh_dat
else
echo "tcapi commit WLan_Entry0"
/userfs/bin/tcapi commit WLan_Entry0
fi

sleep 60
/userfs/bin/iwpriv ra0 set manual_txop=1
/userfs/bin/iwpriv ra0 mac 60130014=ffffffff
/userfs/bin/iwpriv ra0 mac 60130010=ffffffff