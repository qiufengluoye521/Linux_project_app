# 1 在虚拟机上运行，或PC(linux)平台上运行，不能在嵌入式平台应用。

# 2 步骤

2.1 tar -xjf freetype-2.4.10.tar.bz2

2.2 mv freetype-2.4.10 freetype-2.4.10_pc

2.3 cd freetype-2.4.10_pc

2.4 ./configure

2.5 make

2.6 sudo make install

2.7 gcc  -o example1 example1.c -I /usr/local/include/freetype2 -lfreetype -lm

-I 指定包含目录，-l 包含文件 -lfreetype: freetype -lm：数学库

2.8 复制字体文件，C:/WINDOWS/Fonts/simsun.ttc 复制在linux

# 3 移植到嵌入式平台

3.1 tar xjf freetype-2.4.10.tar.bz2 

3.2 ./configure --host=arm-linux 指定交叉编译器

--> 查看交叉编译环境的目录 echo $PATH

/work/tools/gcc-3.4.5-glibc-2.3.6/bin

--> cd /work/tools/gcc-3.4.5-glibc-2.3.6/bin --> cd .. -->  find -name include

include的目录为：

/work/tools/gcc-3.4.5-glibc-2.3.6/arm-linux/include

lib 目录为：

/work/tools/gcc-3.4.5-glibc-2.3.6/arm-linux/lib

3.3 make

3.4 make DESTDIR=$PWD/tmp install 先安装到当前目录的tmp目录下

3.5 cp * /work/tools/gcc-3.4.5-glibc-2.3.6/arm-linux/include -rf

--> cd /usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/include

--> mv freetype2/freetype .

编译出来的头文件tmp/usr/local/include/*应该放入：
/work/tools/gcc-3.4.5-glibc-2.3.6/arm-linux/include下

3.6 sudo cp * /work/tools/gcc-3.4.5-glibc-2.3.6/arm-linux/lib -d -rf

--> cp  *so /work/nfs_root/fs_mini_mdev_new/lib -d

 一般复制到嵌入式平台的/lib或/usr/lib下，可在/etc/ld.so.conf中指定共享库的位置，尽量使用配置好的文件系统，试了多次jz2440带qt的yaffs系统，将.so文件放入/lib中可用。

编译出来的库文件tmp/usr/local/lib/* 应该放入：
/work/tools/gcc-3.4.5-glibc-2.3.6/arm-linux/lib

3.7 编译 arm-linux-gcc  -o example1 example1.c  -lfreetype -lm

程序和pc端一致，放在嵌入式端执行，应该在终端（串口）显示。

若出现：Invalid or incomplete multibyte or wide character 错误用下面指令编译

arm-linux-gcc -finput-charset=GBK -fexec-charset=GBK  -o example1 example1.c  -lfreetype -lm

3.8 放到嵌入式端执行 ./example1 simsun.ttc






