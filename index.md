Canon LBP800 driver for linux
============

Canon LBP 800 linux drivers.  
Tested on Ubuntu 10.10, 12.04, 13.04, 13.10.
Also should works with newer Ubuntu (14.04, 15.04, 15.10, 16.04, etc.).

Version 0.1.10

What is this
--------
This is driver for Canon LBP800 printer (with CAPT protocol via parallel port).

How to install on Ubuntu
--------

I write this manual after I spend a whole day to setup this printer on Ubuntu :)  
If you want to make bug report or contribution — please open an issue or PR in this repo.
Also, fill free to drop me a line to mail@aleksejleonov.com.

#### 0. Check BIOS settings

Your parallel port settings should be:
* Parallel port must be enabled.
* **Mode: EPP** — very important.

Also, I have next options and it works for me (but other options, like I/O 278 or IRQ 5 should also work fine):
* I/O Address: 378
* IRQ: 7

#### 1. Download and unpack driver sources

```bash
cd ~
mkdir lbp800 && cd lbp800
wget https://github.com/aleksejleonov/lbp800_linux/archive/0.1.10.tar.gz
tar zxf 0.1.10.tar.gz && cd lbp800_linux-0.1.10
```

#### 2. Install packages that will be needed to compile driver

```bash
sudo apt-get install build-essential
sudo apt-get install libcups2-dev
```

#### 3. Remove libsane-hpaio package

libsane-hpaio package can try to use parallel port (/dev/lp0) and this can lead to conflicts.

```bash
sudo apt-get purge libsane-hpaio
```

#### 4. Make & Install driver

```bash
cd ~/lbp800/lbp800_linux-0.1.10
make
sudo make install
```

#### 5. Check parallel port modules

You need to check that **lp**, **ppdev**, **parport_pc** modules are loaded and works.

```bash
lsmod | grep lp
################################
# You have get something like: #
################################
#
# lp                     13299  0
# parport                40753  3 lp,ppdev,parport_pc

lsmod | grep ppdev
################################
# You have get something like: #
################################
#
# ppdev                  12817  0
# parport                40753  3 lp,ppdev,parport_pc

lsmod | grep parport_pc
################################
# You have get something like: #
################################
#
# parport_pc             27504  1
# parport                40753  3 lp,ppdev,parport_pc
```

If some of this modules missed in output, you need to add this module to ```/etc/modules```.  
Example (if **parport_pc** module is missed):

```bash
lsmod | grep parport_pc
# You have no output -- module missed

sudo nano /etc/modules
#####################################################
# Add "parport_pc" as new string at the end of file #
#####################################################
```

#### 6. Add users to "lp" group

You should add all users that will print on LBP800 to **lp** linux group.

```bash
##
# Change username1, username2 to real account names
##
sudo usermod -a -G lp username1
sudo usermod -a -G lp username2
```

#### 7. Reboot

Don't forget to turn on printer before PC!

```bash
sudo reboot
```

#### 8. Check printer

Check that linux can see your printer:

```bash
sudo cat /proc/sys/dev/parport/parport*/autoprobe*
##
# You should get:
##
# CLASS:PRINTER;
# MODEL:LBP-800;
# MANUFACTURER:Canon;
# DESCRIPTION:Canon LBP-800;
# COMMAND SET:CAPT;

sudo /usr/lib/cups/backend/parallel
##
# You should get:
##
# direct parallel:/dev/lp0 "Canon LBP-800" "Canon LBP-800 LPT #1"
# "MFG:Canon;MDL:LBP-800;CMD:CAPT;VER:1.0;CLS:PRINTER;DES:Canon LBP-800" ""
```

#### 9. Add printer

Open System settings → Printers. 
If Canon LBP800 appears in list, add it.

Else, if you can't find Canon LBP800 in list:
* Choose "Add by URI" option and set URI: **parcapt:/dev/parport0** (parcapt:, not parallel:).
* When you will be asked about driver, find "Canon" in printers list, then choose LBP800 model. (Also, you can choose PPD file from ppd folder in ~/lbp800/lbp800_linux-0.1.10).

Try to print test page. 
Everything should works :)

Debugging
--------

If printer still doesn't works — try to check all steps.

Also you can obtain useful info from kernel log:
```bash
dmesg | grep lp
dmesg | grep parport
```

If you have ubuntu with Apparmor (it can blocks cupsd) you should make next:
```bash
sudo aa-complain cupsd
```

Other printers
--------
If you have Canon printer with USB (like LBP810, LBP1120, etc.) try to install official Canon CAPT Drivers for linux.

* Detailed instructions for Ubuntu 10.04, 11.10, 12.04: [https://help.ubuntu.com/community/CanonCaptDrv190](https://help.ubuntu.com/community/CanonCaptDrv190).
* Last version of Canon CAPT Drivers from official site (as on September 2013, ver. 2.6): [http://support-au.canon.com.au/contents/AU/EN/0100459602.html](http://support-au.canon.com.au/contents/AU/EN/0100459602.html).
* Automated script to install Canon CAPT Drivers ver. 2.20 [https://github.com/raducotescu/CanonCAPTdriver](https://github.com/raducotescu/CanonCAPTdriver).


Thanks
--------
* **Teythoon** ([https://github.com/teythoon](https://github.com/teythoon)) for contributing to this repo.
* **Iamlolive**, **Fabux** and other people from [http://doc.ubuntu-fr.org/imprimante_canon_lbp800](http://doc.ubuntu-fr.org/imprimante_canon_lbp800). They write great wiki (in french :), and give link to 0.1.8.b0 version of this driver.
* **Sidorov Anonymous** (find this name in old Readme file for 0.1.8.b0 version, canon_lbp800driver_0.1.8.b0_linux 20101119; Some page checks are improved).
* **Massimo Del Fedele** (<max@veneto.com>), who actually created driver for LBP800. Also he is owner of [http://www.veneto.com/lbp800/lbp800.html](http://www.veneto.com/lbp800/lbp800.html) for 0.1.2 version. (Now this site isn't available).
* **Rildo Pragana** ([http://pragana.net](http://pragana.net)), for his driver for another GDI winprinter (Samsung ML-85G).
* **Nicolas Boichat** (<nicolas@boichat.ch>), for his driver for canon LBP-810 USB GDI printer.
