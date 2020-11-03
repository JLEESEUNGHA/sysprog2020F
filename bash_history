vi .bash_history
sudo apt-get install -y ncurses-dev libssl-dev
git
wget https://kernel.org/pub/linux/kernel/v4.x/linux-4.4.tar.gz
ls
mv linux-4.4.tar.gz /usr/src/
sudo mv linux-4.4.tar.gz /usr/src/
tar -zxvf /usr/src/linux-4.4.tar.gz 
mv linux-4.4/ /usr/src/
sudo mv linux-4.4/ /usr/src/
ls
cd /usr/src
cd linux-4.4
ls -a
cp /boot/config-4.15.0-112-generic .config
make menuconfig
cd ..
make bzImage -j 6 && make modules -j 6
cd linux-4.4/
make bzImage -j 6 && make modules -j 6
make bzImage -j 6 | tee make_bzImage.log && make modules -j 6 | tee make_modules.log
ls
sudo make modules_install -j 6 | tee make_modules_install.log && sudo make install | tee make_install.log
ls
exit
vi .bash_history
cp .bash_history bash_history_install.log
sudo reboot








vi .bash_history 
cp .bash_history bash_history_log2
wget http://iozone.org/src/current/iozone3_414.tar
tar -svf iozone3_414.tar 
tar -xvf iozone3_414.tar 
ls
cd iozone3_414/src/current/
sudo make linux-ia64
ls -a iozone
sudo apt install f2fs-tools
dd if=/dev/zero of=./diskfile bs=1024 count=200000
mkfs.f2fs ./diskfile 
mount | tee mount_org
vi mount_org 
losetup /dev/loop0 diskfile 
sudo losetup /dev/loop0 diskfile
ls
mkdir dir_f2fs
mount /dev/loop0 dir_f2fs/
sudo mount /dev/loop0 dir_f2fs/
ls dir_f2fs/
mount | tee mount_aft_f2fs
cat /proc/diskstats | less
exit
vi .bash_history 
cp .bash_history bash_history_post_f2fs_creation
cd /usr/src
ls
cd linux-4.4/block/
cp blk-core.c blk-core.c_old
vi blk-core.c
sudo apt install vim
vi blk-core.c
cd ..
make bzImage -j 6 | tee make_bzImage.log2 && make modules -j 6 | tee make_modules.log2
vi block/blk-core.c
make bzImage -j 6 2>&1 | tee make_bzImage.log2
make modules -j 6 | tee make_modules.log2
sudo make modules_install -j 6 | tee make_modules_install.log2 && sudo make install | tee make_install.log2
sudo reboot





dmseg
mount
mount /dev/loop0 dir_f2fs/
sudo mount /dev/loop0 dir_f2fs/
losetup /dev/loop0 ./diskfile 
sudo losetup /dev/loop0 ./diskfile 
sudo mount /dev/loop0 dir_f2fs/
./iozone -i 0 -f /dev/loop0
exit




cd /usr/src/linux-4.4/
vi block/blk-core.c
cp block/blk-core.c block/blk-core.c_less_old
vi block/blk-core.c
make bzImage -j 6 2>&1 | tee make_bzImage.log2
vi block/blk-core.c
make bzImage -j 6 2>&1 | tee make_bzImage.log2
make modules -j 6 | tee make_modules.log2
sudo make modules_install -j 6 | tee make_modules_install.log2 && sudo make install | tee make_install.log2
reboot





dmesg
sudo ./iozone -i 0 -f /dev/loop0
sudo losetup /dev/loop0 ./diskfile 
sudo mount /dev/loop0 dir_f2fs/
losetup /dev/loop0 ./diskfile 
sudo mount -t f2fs /dev/loop0 dir_f2fs/
sudo losetup /dev/loop0 ./diskfile 
exit







cd /usr/src/linux-4.4
vi block/blk-core.c
cd ~
vi .bash_history 
vi block/blk-core.c
cd /usr/src/linux-4.4
vi block/blk-core.c
make bzImage -j 6 2>&1 | tee make_bzImage.log2 && make modules -j 6 | tee make_modules.log2 && sudo make modules_install -j 6 | tee make_modules_install.log2 && sudo make install | tee make_install.log2
vi block/blk-core.c
make bzImage -j 6 2>&1 | tee make_bzImage.log2 && make modules -j 6 | tee make_modules.log2 && sudo make modules_install -j 6 | tee make_modules_install.log2 && sudo make install | tee make_install.log2
reboot








vi .bash_history 
mkdir imp
mount /dev/sdb /imp
sudo mkfs.f2fs /dev/sdb1
sudo mkfs.ext4 /dev/sdb2
mount /dev/sdb imp
it init
sudo apt install git
git init
cp /usr/src/linux-4.4/block/blk-core.* .
ls
git commit
git config --global user.email "jleesh@korea.ac.kr"
git config --globa user.name "JLEESEUNGHA"
git commit
git add .
git commit
git config --global core.editor "vim" 
git remote origin https://github.com/JLEESEUNGHA/sysprog2020F.git
git remote add origin https://github.com/JLEESEUNGHA/sysprog2020F.git
git branch -M main
git push -u origin main
cd ..
cd spa1/






make
sudo insmod bbq-proc.ko 
dmesg
cat /proc/BBQ/BBQ | less
sudo cat /proc/BBQ/BBQ | less
ls
vi bbq-proc.c
make
sudo insmod bbq-proc.ko
sudo rmmod bbq-proc.ko
sudo insmod bbq-proc.ko
sudo cat /proc/BBQ/BBQ | less
exit
sudo shutdown now
sudo reboot








uname -r
cd spa1/
vi bbq-proc.c
make
insmod bbq-proc.ko
sudo insmod bbq-proc.ko
dmesg -l 6 > dmesg_log_disk_test
vi dmesg_log_disk_test 
vi i_write_log_f2fs 
mount /dev/sdb1 dir_f2fs/
sudo mount /dev/sdb1 dir_f2fs/
sudo mount /dev/sdb2 dir_ext4/
sudo ./iozone -i 0 -s 500000B -f dir_f2fs/tmp && sudo cat /proc/BBQ/BBQ > i_write_log_f2fs
sudo ./iozone -i 0 -s 500000B -f dir_ext4/tmp && sudo cat /proc/BBQ/BBQ > i_write_log_ext4
dmesg -l 6 > dmesg_log_disk_test
vi dmesg_log_disk_test 
vi i_write_log_f2fs 
vi i_write_log_ext4 
sudo ./iozone -i 0 -s 500000B -f dir_ext4/tmp && sudo cat /proc/BBQ/BBQ > i_write_log_ext42
sudo ./iozone -i 0 -s 500000B -f dir_f2fs/tmp && sudo cat /proc/BBQ/BBQ > i_write_log2
vi i_write_log2
dmesg > dmesg_full_log
vi dmesg_full_log 
sudo ./iozone -i 0 -s 500000B -f dir_ext4/tmp && sudo cat /proc/BBQ/BBQ > i_write_log_ext42
dmesg > dmesg_full_log
vi dmesg_full_log 
ls
vi i_write_log2
ls
cp i_write_log2 ./spa1/
cp dmesg_full_log ./spa1/
vi spa1/
cd spa1/
ls
git status
vi include/bbq.h 
ls
git add include/* i_write_log2 dmesg_full_log 
git status
git commit
git push
sudo shutdown now
