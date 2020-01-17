# about
This tool grabs the mutex that emotet 1st stage binary gains in advance and prevents further activities by it.
this just reduces the risk of infection on the windows devices.

# how to install
copy binary bin\emotet-locker.exe (x86) or bin\x64\emotet-locker.exe to somehwere on the machine and configure as runs only once when device is booted.

* Distribution and configuration via GPO is possible for enterprise usage.
* Configuring taskscheduler is recommended on personal user if single PC is shared with multiple users
* Source code is also avilable, you can build it if necessary.

# hash of binary
a56fe527f15db198b9fdb7cc0b64d345d024f9ba8b6ff91c8c573931b4d7d98e *emotet-locker.exe
2a6f570501782e653bab5365f26df1599b78c31bc24fd4996c8e4062b5b4008a *x64/emotet-locker.exe



