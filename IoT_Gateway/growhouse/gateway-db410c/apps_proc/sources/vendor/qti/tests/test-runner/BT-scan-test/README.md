# BT-scan-test README

## Description:
    - The main Dragonboard will act as the DUT and the another one will act as a helper to the tests. The DUT scans for the HELPER MAC address. The test is successful if the HELPER MAC is found.

## Requirements:
    - Two Dragonboards 410c: DUT (Device Under Testing) and the HELPER.
    - "test-definitions/automated/util/BT-tests-util/BT-tests-config.sh" file updated with the IP of the Dragonboards.
    - Two dragonboards and the host linux (if the tests are running remotely) must be connected on the same ethernet network for the SSH communication. 
