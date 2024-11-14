#!/bin/sh

hw_nat -!
hw_nat -U 15 15 15 15

echo 8192 > /proc/net/skbmgr_driver_max_skb
echo 3072 > /proc/net/skbmgr_limit

echo 0 > /proc/tc3162/softqdma_ratelimit
echo 0 0 > /proc/tc3162/soft_qdma
