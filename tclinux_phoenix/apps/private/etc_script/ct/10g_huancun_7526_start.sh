#!/bin/sh

hw_nat -!
hw_nat -U 999 999 999 999

echo 9096 > /proc/net/skbmgr_driver_max_skb
echo 8072 > /proc/net/skbmgr_limit

echo 100000 > /proc/tc3162/softqdma_ratelimit
echo 0x20 1 > /proc/tc3162/soft_qdma
