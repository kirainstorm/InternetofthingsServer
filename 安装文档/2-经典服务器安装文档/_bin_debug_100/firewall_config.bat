cd %~dp0
netsh advfirewall firewall add rule name="X XService TCP Port" dir=in action=allow protocol=TCP localport=1935,6499-6505,6599-6605
netsh advfirewall firewall add rule name="X Sqlservr TCP Port" dir=in action=allow protocol=TCP localport=1433
netsh advfirewall firewall add rule name="X Sqlservr UDP Port" dir=in action=allow protocol=UDP localport=1434
netsh advfirewall firewall add rule name="X Sqlservr Program" dir=in program="%ProgramFiles%\Microsoft SQL Server\MSSQL12.SQLEXPRESS\MSSQL\Binn\sqlservr.exe" action=allow
netsh advfirewall firewall add rule name="X XService XInternetRedirection" dir=in program="%cd%\XInternetRedirection.exe" action=allow
netsh advfirewall firewall add rule name="X XService XInternetSignaling" dir=in program="%cd%\XInternetSignaling.exe" action=allow
netsh advfirewall firewall add rule name="X XService XInternetStream" dir=in program="%cd%\XInternetStream.exe" action=allow
netsh advfirewall firewall add rule name="X XService XInternetStorageWriter" dir=in program="%cd%\XInternetStorageWriter.exe" action=allow
netsh advfirewall firewall add rule name="X XService XInternetStorageReader" dir=in program="%cd%\XInternetStorageReader.exe" action=allow
netsh advfirewall firewall add rule name="X XService XRtmpTransmisProc" dir=in program="%cd%\XRtmpTransmisProc.exe" action=allow
pause