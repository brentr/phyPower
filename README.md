This Linux kernel module creates the write only file

/sys/phyPower/command

accepting command strings of the form:

  +phyName or -phyName
  
to turn the Platform phy named "phyName" on or off, respectively.

Developed to provide a means to cycle USB VBUS power when devices misbehave.

(Specific use case is for devices that become unresponsive when their firmware hangs or crashes)

Example:

  # echo -usb >/sys/phyPower/command

Turns Rock Pi-S boards USB host VBUS board off
