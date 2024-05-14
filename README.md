How to use:

1. Get verdor ID and device ID
```
# lspci -nn -s 0000:00:19.0
```

2. Allocate a new id to the pei driver and bind the device to it.  For example:
```
# echo "8086 10f5" > /sys/bus/pci/drivers/pei-drv/new_id
# echo -n 0000:00:19.0 > /sys/bus/pci/drivers/pei-drv/bind
# ls -l /sys/bus/pci/devices/0000:00:19.0/driver
 .../0000:00:19.0/driver -> ../../../bus/pci/drivers/pei-drv
```

3. Unbind the pei driver.
```
echo -n 0000:00:19.0 > /sys/bus/pci/drivers/pei-drv/unbind
```
