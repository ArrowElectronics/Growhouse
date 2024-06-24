Collect into one folder following file:
1) QSaharaServer.exe which is taken from the MB binaries (ChipCode)
2) fh_loader.exe which is taken from the MB binaries (ChipCode)
3) UFS_provisioning.bat
4) provision_toshiba.xml
5) prog_ufs_firehose_8996_lite.elf which is taken from the MB binaries (ChipCode)


Steps for UFS prowisioning:
1) Enter device into the EDL mode using dip switches
3) Power cycle
2) Open the folder from the CMDL and run script in the following way:
     UFS_provisioning.bat <COM number>

	For example:	
		UFS_provisioning.bat COM2
		
