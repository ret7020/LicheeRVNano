cd freertos/cvitek/
rm -r install build
./build_cv181x.sh
cd /workspace
build_fsbl
scp /workspace/install/soc_sg2002_licheervnano_sd/fip.bin root@192.168.1.21:/boot/fip.bin

