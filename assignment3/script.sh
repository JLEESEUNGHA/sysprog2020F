# set ip_forward
sudo sysctl -w net.ipv4.ip_forward=1

# change directory to the right folder (if necessary)
#cd ~/spa1/assignment3

# remove old files
rm spa3.mod.*

# make and insert module
make
sudo insmod spa3.ko

echo "module inserted.\n"

# add firewall rules
sudo chmod 777 /proc/group32/add

echo "I 1111" > /proc/group32/add
echo "O 2222" > /proc/group32/add
echo "F 3333" > /proc/group32/add
echo "P 4444" > /proc/group32/add

echo "F 5555" > /proc/group32/add
echo "P 5555" > /proc/group32/add

echo "firewall rules initialised.\n"

# check firewall rules
cat /proc/group32/show

echo "firewall rules shown.\n"

# delete a firewall rule
sudo chmod 777 /proc/group32/del
echo "3" > /proc/group32/del

echo "firewall rule deleted.\n"

# with the server running, test the firewall
../assignment2/client 5 1111 2222 3333 4444 5555

echo "sending message to server...\n"

# save dmesg log
dmesg > dmesg_log

echo "log created."
