#!/bin/sh
/userfs/bin/iwpriv ra0 set manual_txop=1
/userfs/bin/iwpriv ra0 set TxBurst=0
/userfs/bin/iwpriv ra0 mac 60130014=ffffffff
/userfs/bin/iwpriv ra0 mac 60130010=ffffffff
/userfs/bin/iwpriv ra0 set be_to_vo=0
/bin/echo 1 0 0 0 > /proc/tc3162/sw_rps