This Linux kernel module creates the file /sys/phyPower
accepting command strings of the form:


  +phyName or -phyName

  
to turn the Platform phy named "phyName" on or off, respectively.

Developed to provide a means to cycle USB VBUS power to completely reset it when devices misbehave.

(Specific use case is for devices that become unresponsive when their firmware hangs or crashes)
