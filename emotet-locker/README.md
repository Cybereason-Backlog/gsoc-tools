# about
This tool grabs the system resource (example for mutex, for the prevenion of multiple execution) in advance and
prevents emotet 1st stage executable binary from running.
this just reduces the risk of infection on the windows devices.

# how to install
copy binary bin\emotet-locker.exe (x86) or bin\x64\emotet-locker.exe to somehwere on the machine and configure as runs only once when device is booted.

* distribution and configuration via GPO is possible for enterprise usage.
* configuring taskscheduler is recommended on personal user if single PC is shared with multiple users
Source code is also avilable, you can build it if necessary.

# hash of binary
33e3453875097943f8745ddd690c9153a8b769dc788ca63d0d57db021bccac18  bin/x64/emotet-locker.exe
1cd1d9a84c77ab7d1666066c04ef124d4f9442981c4e2109c7e7cf797666a17a  bin/emotet-locker.exe



