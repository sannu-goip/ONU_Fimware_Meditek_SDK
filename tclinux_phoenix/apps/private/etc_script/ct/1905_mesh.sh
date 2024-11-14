#!/bin/sh

start_controller()
{
brctl stp br0 on
brctl setbridgeprio br0 0
}

start_agent()
{
brctl stp br0 on
brctl setbridgeprio br0 32768
brctl setpathcost br0 apcli0 402
brctl setpathcost br0 apclii0 401
}

if [ $1 -eq 1 ]
then
start_agent
else
start_controller
fi