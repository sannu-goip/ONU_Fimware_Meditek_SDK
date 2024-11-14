#!/bin/sh
/userfs/bin/iwpriv rai0 set manual_txop=1
/userfs/bin/iwpriv rai0 mac 820F4010=00fe00fe
/userfs/bin/iwpriv rai0 mac 820F4014=00fe00fe
/userfs/bin/iwpriv rai0 set TxBurst=0