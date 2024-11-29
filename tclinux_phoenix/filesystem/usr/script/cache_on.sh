#!/bin/sh

PROFILE_CFG=/userfs/profile.cfg
if [ -f $PROFILE_CFG ] ; then
    chmod 777 $PROFILE_CFG
    . $PROFILE_CFG
fi

if [ "$TCSUPPORT_CPU_EN7580" != "" ];then
/bin/echo 0 > /proc/tc3162/fe_dyn_qdma_wan_txq_cngst
sys memwl bfb570a4 3c002000
sys memwl bfb550a4 3c002000

sys memwl bfb50080 200020
sys memwl bfb50084 200080
sys memwl bfb50088 100080
sys memwl bfb500a0 8000078
sys memwl bfb500d0 10002F00
else
/bin/echo 0 > /proc/tc3162/fe_dyn_qdma_wan_txq_cngst
sys memwl bfb550a4 3c001700
fi

/userfs/bin/hw_nat -N 1
/userfs/bin/qdmamgr_lan set rxratelimit value 0 100000
/userfs/bin/qdmamgr_wan set rxratelimit value 0 100000

