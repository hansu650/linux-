# 域名
# 安全的网页(https)
- 不用重做，一个名字就行

1. cd到这个文件夹里面看一下/lib/modules/`uname -r`/kernel/fs
- ls

- 所有支持的文件系统(模块)
- ``斜引号，看版本号的

- fs就是文件系统


1. cat /etc/fstab
uuid因为多插盘名字会变 /挂在根目录 ext4 权限 

vfat 引导扇区

2. Related Commands
-    su;  useradd(记得要去设置密码不然无法登录);  adduser(更友好，但是是封装了的不是shell命令);   passwd;  usermod(创建用户的属性)

4. Environment variables:   PATH SHELL PWD  HOME

- env
- 系统中间所有定义的环境变量
- echo $SHELL
- 默认bashshell
- echo $PWD
- echo $HOME

- which ls
- user/bin 在环境变量里面
- 

- echo $PATH  (echo %PATH%)






- export PATH=“$PATH: /home/***”
-         所以path不能掉    原来的值 : 附加新的值
- path是原来的所有的环境变量位置，把别的同学看一下就行



- /etc/profile    /etc/environment  ~/.bashrc
-                                  ~ 家目录 .开头 隐藏文件 

- source /etc/environment
-  source  使后面的配置文件生效，这样就不用重启了


5. IDE:  vscode;  eclipse; anaconda

6. GNU tools
- GCC, gdb, make (Makefile), git (version control)
- 纯命令行编译

----
# apache配置
7. Apache configuration
- nginx(更简单)
- 那种框架无所谓只要是Linux上面运行的

ufw enable
ufw status
sudo ufw allow 80/tcp

sudo ufw allow 443/tcp

# 网页(html/css/js)  +  写配置文件  
( 1) Configuration File
  /etc/apache2:  Apache configuration directory. All Apache configuration files reside here.
  - 配置文件都在这里
 /var/www/myweb.com/html/index.html:  actual Web content (by default only contains the default Apache see page) provided by the/var/WWW/directory. This can be changed by modifying the Apache configuration file.

- /var/www/qt.com/html/index.html
www.qt.com

/etc/apache2 / apache2.conf: the main configuration file. This can be modified to make changes to the Apache global configuration. This file is responsible for loading many other files in the configuration directory.

/etc/apache2/ports.conf: This file specifies the port that Apache will listen on. By default, when the SSL-enabled module is enabled, Apache listens on port 80 and on port 443.
- ip:端口号，表明网络进程 http 80 https 443
- 先配80 再配443
- 这两个文件我们不会改的

## 虚拟主机
- 每个都有一个文件

/etc/apache2/sites-available/
- 最后配置文件写在这里 
/etc/apache2/sites-enabled/  
- copy到这里



