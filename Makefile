obj-m +=bbq-proc.o

KDIR=/usr/src/linux-4.4

all:
	$(MAKE) -C $(KDIR)
	#SUBDIR=$(PWD) modules

clean:
	echo "No cleaning performed."
	#rm -rf *.o *.ko *.mod.* *.symbers *.order