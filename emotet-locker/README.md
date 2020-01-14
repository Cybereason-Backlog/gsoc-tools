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
a56fe527f15db198b9fdb7cc0b64d345d024f9ba8b6ff91c8c573931b4d7d98e *emotet-locker.exe
2a6f570501782e653bab5365f26df1599b78c31bc24fd4996c8e4062b5b4008a *x64/emotet-locker.exe



