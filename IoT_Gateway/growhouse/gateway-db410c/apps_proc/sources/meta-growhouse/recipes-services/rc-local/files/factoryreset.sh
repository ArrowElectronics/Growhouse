#!/bin/sh

# Restart ble advertiser
/bin/systemctl restart ble-advertise.service

# Restart ble-server
/bin/systemctl restart ble-server

# Remove database of selene
/bin/rm -rf /opt/selene/db/

# start selene service
/bin/systemctl restart selene

# Restart growhouse-server
/bin/systemctl restart growhouse-server

