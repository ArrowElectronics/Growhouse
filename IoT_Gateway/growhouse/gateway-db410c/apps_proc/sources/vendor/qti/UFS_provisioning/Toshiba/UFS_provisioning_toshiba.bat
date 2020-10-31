
set com=%1
QSaharaServer.exe -p  \\.\%com% -s 13:prog_ufs_firehose_8996_lite.elf -b .\
Timeout 2
fh_loader.exe --sendxml=provision_toshiba.xml --search_path=.\ --skipstorageinit --memoryname=ufs --port=\\.\%com%
Timeout 2