How to compile and insmod module:
1. Get the source code.
```
# git clone https://github.com/gjinhui/pei-drv.git
```
- Or downloading from the github, and then unzip.


2. Start to compile. Go to the source directory first and then make.
```
# make
```

3. insmod module.
```
# insmod pei_drv.ko
```
- Check:
```
# lsmod |grep pei_drv
 pei_drv                16384  0
```



How to use:

1. Get verdor ID and device ID. (PEI card vendor ID and device ID: 8086:0d4b)
```
# lspci -nn |grep 0000:b0:00.0
 b0:00.0 Class 1180: Device 8086:0d4b (rev 05)

```

2. Allocate a new id to the pei driver and bind the device to it.  For example:
```
# echo "8086 0d4b" > /sys/bus/pci/drivers/pei-drv/new_id

# ls -l /sys/bus/pci/devices/0000:0b:00.0/driver
 .../0000:0b:00.0/driver -> ../../../bus/pci/drivers/pei-drv
```

3. Unbind the pei driver.
```
# echo -n 0000:0b:00.0 > /sys/bus/pci/drivers/pei-drv/unbind
```
