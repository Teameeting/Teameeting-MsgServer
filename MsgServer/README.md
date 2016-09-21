#编译说明

###Linux
本项目只提供Linux编译方式<br>
<br>
1. build_shell中的run_build.sh为编译脚本<br>
<br>
cd build_shell<br>
./run_build.sh 即可编译整个项目<br>
<br>
msgserver为编译后文件目录<br>
在完成编译后，运行untar.sh解压文件msgserver.tar.bz2<br>
cd msgserver <br>
sudo ./install.sh -i 即可安装Teameeting项目的可执行文件到/usr/local/dync/msgserver/ <br>
sudo ./install.sh -j 即可安装直播项目的可执行文件到/usr/local/dync/msgserver/ <br>
<br>
运行./start.sh 即可启动程序<br>
./start.sh<br>
./start_ms.sh<br>
<br>
运行./stop.sh 即可结束程序<br>
./stop.sh<br>
./stop_ms.sh<br>


2. 分步编译，参考各个目录README<br>

---
