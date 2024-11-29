#!/bin/sh

PROFILE_CFG=/userfs/profile.cfg
if [ -f $PROFILE_CFG ] ; then
    chmod 777 $PROFILE_CFG
    . $PROFILE_CFG
fi

if [ "$TCSUPPORT_CPU_EN7580" != "" ];then
/bin/echo 1 > /proc/tc3162/fe_dyn_qdma_wan_txq_cngst
sys memwl bfb570a4 3c000333
sys memwl bfb550a4 3c000e00

sys memwl bfb50080 800080
sys memwl bfb50084 800080
sys memwl bfb50088 800080
sys memwl bfb500a0 1a300078
sys memwl bfb500d0 10002330
else
/bin/echo 1 > /proc/tc3162/fe_dyn_qdma_wan_txq_cngst
sys memwl bfb550a4 3c000333
fi

/userfs/bin/hw_nat -N 30
/userfs/bin/qdmamgr_lan set rxratelimit value 0 6000
/userfs/bin/qdmamgr_wan set rxratelimit value 0 6000

