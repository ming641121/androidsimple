#!/system/bin/sh
# setprop qemu.hw.mainkeys 1 => Hide soft buttons
echo "$(getprop persist.sys.mainkeys)"
if [ $(getprop persist.sys.mainkeys) != "0" ];then
	setprop qemu.hw.mainkeys 1
else
	setprop qemu.hw.mainkeys 0
fi
