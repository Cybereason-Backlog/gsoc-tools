# about
This tool grabs the mutex that emotet 1st stage binary gains in advance and prevents further activities by it.
this just reduces the risk of infection on the windows devices.

# how to install
copy binary bin\emotet-locker.exe (x86) or bin\x64\emotet-locker.exe to somehwere on the machine and configure as runs only once when device is booted.

* Distribution and configuration via GPO is possible for enterprise usage.
* Configuring taskscheduler is recommended on personal user if single PC is shared with multiple users
* Source code is also avilable, you can build it if necessary.

# hash of binary
79c2c400dd5b2653f1d7d11a8c3978c276a467c2be657802ac5138c426e1cb39  bin/emotet-locker.exe
156259759450fcc4011b4023b6065352d3fab745ed6021ffb54bb8d063cb7eeb  bin/x64/emotet-locker.exe

# history
Jan 24, 2020 - Fixed bug of mutex name generation in some cases and added another mutex rule.</br>
Thanks @gorimpthon (https://twitter.com/gorimpthon/) for your feedback</br>
Jan 14, 2020 - Revised License text and removes dependencies of VC runtime</br>
Jan 4, 2020 - first import</br>


