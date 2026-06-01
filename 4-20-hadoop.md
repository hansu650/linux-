# 这玩意可以写到心得里面蛮重要的讲了两次了
## 纯计网
## https=http+tls 
- ping不一定是ip因为服务器集群 
- 所以我们看对应网址的dns
- 的response的行号
- 找到真正解析的ip

- tls 1.2 baidu
- 点开tls 第一次握手 有random  还有 16个 suite让server选
- 第二个tls 握手 第二个 random  还有certifications证书 还有选择的那个16个里面的套件 
- 第三次 看不到随机数了

- tls 1.3 sina
- ECDH 椭圆曲线所以只需要两次就可以协商出来key
 

# hadoop
- 现在基本上都使用flink,因为hadoop 太慢了
- 单纯用来练习因为难配 flink 简单

- 1.x mapreduce 计算+资源
- 2.x mapreduce 计算 yarn 资源
- mapreduce已经被淘汰了，还是太慢了，spark

- yarn hdfs还是可以不变因为性能瓶颈主要是计算
2. yarn 主节点 resource manager 从节点 node manager 报告给主节点资源使用情况
- client给 主节点任务主节点切片，通过从节点资源情况选择从节点分配容器(类似虚拟机)  子任务都在容器里面运行

- ntfs eftc 集中式
3. hdfs 分布式
- 同一个文件分布在不同的node里面，这样可以并行读更快
- namenode 有个备份
# 至少有5个进程 检查的时候
1. 
2. 
3. 
4. 
5. 
- 2.6 最完整的一个版本
- 3.3.5 集群安装是真的有多个物理机器
- 所以我们是一个伪分布式
- 很多个机器都是自己

- 别忘了最后注销当前用户（点击屏幕右上角的齿轮，选择注销），返回登陆界面。在登陆界面中选择刚创建的 hadoop 用户进行登陆。
- 不用自己的用户登录
- 一般不用vim

## 配置ssh
- qq的远程协助 早期tailnet 现在是ssh
- 主控节点要能登陆到从节点上面去
- ps -aux | grep ssh
- 可以看到自带的ssh客户端
- sshd  守护进程 我们要装ssh-server端
- ssh localhost ssh客户端是要在别的电脑装但是我们是自己的电脑所以在自己的电脑里面装
- 管理员不用输密码，节点都是相互信任的所以都配置了无密码登录

- 只要是ssh 登录到服务器下一步一定要exit

# 用户名 ssh登录不退出
- 两个容易错的地方

## 安装java8
- 配置环境变量

- 和apache一样只需要下载不用安装，下载bin 二进制的不下source
- md5看你下的对不对，md5算一个摘要信息
- grep看看正常的md5码和正常的对不对

## 伪分布式配置
1. 配置4个文件 
- site.xml
- 缓存，端口号等配置 有兴趣的同学可以去看有没有别的配置
- hdfs-site.xml
- 数据做不做备份 不备份只存一次
- namenode用ip地址(伪分布式就用file的路径来区分就行了)

## 启动一个脚本来启动hadoop
- 还要把hadoop的环境变量配一下
- jps 至少有5个进程
- namenode
- secdorynamenode
- node

6-7个进程
hdfs+yarn
## yarn也要配好
- 也就两个配置文件
- 把资源管理从mapreduce改成yarn
- 启动
- start history server

## 给老师看jps就行
- localhost:8088/cluster
- 可以跑一下应用
- 里面自带实例


- 下面这个不用给老师看
- 一般跑wordcount实例

- 给用户展示的是一个整体，所以你创建了一个文件用户也不知道在哪里
- ls output


## 网络编程 到时候后面要来讲


- 编译内核编两次
- 先编一次啥都不改编一次


