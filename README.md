# Redis之高性能服务器存储应用

## Redis简介
Redis是一个开源的key-value存储系统。与Memcached类似，Redis将大部分数据存储在内存中，支持的数据类型包括：字符串、哈希表、链表、集合、有序集合及基于这些数据结构类型的相关操作。Redis使用C语言开发，在大多数像Linux、BSD和Solaris等POSIX系统上无需任何外部依赖就可以使用。Redis支持的客服端语言也非常丰富，常用的计算机语言如C、C#、C++、Object-C、PHP、Python、Java、Perl、Lua、Erlang等均有可用的服务端来访问Redis服务器。当前Redis的应用非常广泛，国内像新浪、淘宝，国外像Flickr、GitHub等均在使用Redis的缓存服务。

一句话简介：Redis基于内存操作，读写速度很快，100000读写/秒，可以作为内存型缓存服务器，提供持久化存储方案，可以作为结构不复杂的数据库使用。

## 为什么要缓存
5400转的笔记本硬盘：50-90MB/s

7200转的台式机硬盘：90-190MB/s

固态硬盘读写速度可以达到500MB/s

内存DDRIII1333的速度读写速度大概在8G/s

其他频率的条子速度根据大小会有很大的浮动

## Redis使用场景
1.取最新N个数据的操作
    
    比如典型的取你网站的最新文章，我们可以将最新5000条评论的ID放在Redis的List集合中，并将超出集合部分从数据库获取。
    
2.排行榜应用，取TOP N 操作

    这个需求与上面的不同之处在于，前面操作以时间为权重，这个是一某个条件为权重，比如按顶的次数排序，
    这时候就需要我们的sorted set出马了，将你要排序的值设置成sorted set的socred，
    将具体的数据设置成对应的Value，每次只需要执行一条ZADD命令即可。
    
3.需要精准设定过期时间的应用

    比如你可以把上面说到的sorted set的score值设置成过期时间的时间戳，那么就可以简单地通过过期时间排序，
    定时清除过期数据了，不仅是清除Redis中过期数据，你完全可以把Redis里这个过期时间当成是对数据库中数据的索引，
    用Redis来找出那些数据需要过期删除，然后再精准地从数据库中删除相应的记录。

4.计时器应用

    Redis的命令都是原子性的，你可以轻松地利用INCR，DECR命令来创建计数器系统。

5.uniq操作，获取某段时间所有数据排重值

    这个使用Redis的set数据结构最适合了，只需要不断地将数据注set中扔就行了，set意为集合，所以会自动排重。

6.Pub/Sub构建实时消息系统

    Redis的Pub/Sub系统可以构建实时的消息系统，比如很多用Pub/Sub构建的实时聊天系统的例子。

7.构建队列系统

    使用list可以构建队列系统，使用sorted set 甚至可以构建有优先级的队列系统。

8.缓存

    最常用，性能优于Memcached（被libevent拖慢），数据结构更多样化。

应用举例：  
当Redis只是当做cache和MySQL同步使用时：
- 读：读redis -> 没有，读mysql -> 把mysql数据写回redis
- 写：写mysql -> 成功，写redis

## Linux下安装Redis
到Redis官网下载稳定版3.0.6

    http://redis.io/download

或者直接终端下输入

    wget http://download.redis.io/releases/redis-3.0.6.tar.gz
    tar zxvf redis-3.0.6.tar.gz
    cd redis-3.0.6
    make
    sudo make install

## Redis基本操作
终端运行服务器：[root@VM_88_201_centos ~]# redis-server  

指定配置文件运行服务器：[root@VM_88_201_centos redis-3.0.6]# redis-server [./redis.conf]  

开启客户端：[root@VM_88_201_centos redis-3.0.6]# redis-cli  
127.0.0.1:6379>  

设置/获取key：  
127.0.0.1:6379> set name itcast  
127.0.0.1:6379> get name  

删除key：127.0.0.1:6379> del name  

判断key是否存在：127.0.0.1:6379>exists name  

停止服务：终端CTRL + C 客户端里shutdown  

rdb持久化存储：save--阻塞服务，不建议使用  

bgsave --子进程异步备份数据

## Redis数据类型
- String——字符串
- Hash——字典
- List——列表
- Set——集合
- Sorted Set——有序集合
- Pub/Sub——订阅
- Transactions——事务

### String——字符串
String数据结构是简单的key-value类型，value不仅可以是String，也可以是数字。使用String类型，可以完全实现目前Memcached的功能，并且效率更高。还可以享受Redis的定时持久化（可以选择RDB模式或者AOF模式）。  

string类型是二进制安全的。意思是redis的string可以包含任何数据，比如jpg图片或者序列化的对象。从内部实现来看其实string可以看作byte数组，最大上限是1G字节，下面string类型的定义：

    struct sdshdr
    {
      long len;
      long free;
      char buf[];
    };

len 是buf数组的长度。  

free是数组中剩余可用字节数，由此可以理解为什么string类型是二进制安全的了，因为它本质上就是个byte数组，当然你可以包含任何数据了。  

buf 是个char 数组用于存贮实际的字符串内容。  

另外string类型可以被部分命令按int处理，比如只用string类型，redis就可以被看作加上持久化特征的Memcached。当然redis对string类型的操作比Memcached还是多很多的，具体操作方法如下：

#### 命令示例：
set --设置key对应的值为string类型的value。

    > set name itcast

setnx --设置key对应的值为string类型的value。如果已经存在，值不变，返回0，nx是not exist的意思。

    > get name
    "itcast"
    > setnx name itcast_new
    (integer) 0
    > get name
    "itcast"

setex--设置key对应的值为string类型的value，并指定此键值对应的有效值。

    > setex color 10 red
    > get color
    "red"
    10秒后...
    > get color
    (nil)

setrange -- 设置指定可以的value的子字符串

    > set email xwp@itcast.cn
    > get email
    "xwp@itcast.cn"
    > setrange email 4 gmail.com
    > get email
    "xwp@gmail.com"
    其中的4是指下标为4（包含4）的字符开始替换

mset -- 一次设置多个key的值，成功返回OK表示所有的值都设置了，失败返回0表示没有任何值被设置。

    > mset key1 python key2 c++
    OK

msetnx -- 依次设置多个值，成功返回OK表示所有的值都设置了，失败返回0表示没有任何值被设置。

    > get key1
    "python"

get -- 获取key对应的string的值，如果key不存在返回nil

    > get name
    "itcast"
    > getset name itcast_new
    "itcast"
    > get name
    "itcast_new"

getrange -- 获取指定看key的value值的子字符串。

    > getrange name 0 4
    "itcas"

mget -- 一次获取多个key的值，如果对应的key不存在，则对应返回nil。

    > mget key1 key2 key3
    1) "python"
    2) "c++"
    3) (nil)

incr -- 对key的值做加加操作

    > set age 20
    > incr age
    (integer) 21

incrby -- 同incr类似，加指定值，key不存在时候会设置key，并认为原来的value是0

    > incrby age 5
    (integer) 5

decr -- 对应的值做的是减减操作，decr一个不存在key，则设置key为-1  

decrby --同的decr减指定值

append--给指定key的字符串值追加value，返回新字符串值的长度。例如我们向name的值追加一个"redis"字符串：

    > append name redis
    > get name

### Hash——字典
在Memcached中，我们经常将一些结构化的信息打包成hashmap，在客户端序列化后存储为一个字符串的值（一般是JSON格式），比如用户的昵称、年龄、性别、积分等。这时候在需要修改其中某一项时，通常需要将字符串（JSON）取出来，然后进行反序列化，修改某一项的值，再序列化成字符串（JSON）存储回去。简单修改一个属性就干这么多事情，消耗必定是很大的，也不适用于一些可能并发操作的场合（比如两个并发的操作都需要修改积分）。而Redis的Hash结构可以便你像在数据库中Update一个属性一样只修改某一项属性值。

#### 命令示例：
hset -- 设置hash field为指定值，如果key不存在，则先创建。

    > hset myhash field1 Hello
    (integer) 1

hsetnx -- 设置hash field为指定值，如果key不存在，则先创建，如果field已经存在，返回0， nx是not exist的意思。

    > hsetnx myhash field "Hello"
    (integer) 1
    > hsetnx myhash field "Hello"
    (integer) 0
第一次执行是成功的，但第二次执行相同的命令失败，原因是field已经存在了。

hmset -- 同时设置hash的多个field。

    > hmset myhash field1 Hello field2 World
    OK

hget -- 获取指定的hash field。

    > hget myhash field1
    "Hello"
    > hget myhash field2
    "World"
    > hget myhash field3
    (nil)
    由于数据库没有field3，所以取到的是一个空值nil。

hmget--获取全部指定的hash field。

    > hmget myhash field1 field2 field3
    1) "Hello"
    2) "World"
    3) (nil)

hincrby -- 指定的hash field加上给定值

    > hset myhash field3 20
    (integer) 1
    > hget myhash field3
    "20"
    > hincrby myhash field3 -8
    (integer) 12
    > hget myhash field3
    "12"

hexists -- 测试指定field是否存在

    > hexists myhash field1
    (integer) 1
    > hexists myhash field9
    (integer) 0
    通过上例可以说明field1存在，但field9是不存在的。

hlen -- 返回指定hash的field数量

    > hlen myhash
    (integer) 4

hkeys -- 返回hash的所有field。

    > hkeys myhash
    1) "field2"
    2) "field"
    3) "field3"
    说明这个hash中有3个field。

hvals -- 返回hash的所有value。

    > hvals myhash
    1) "World"
    2) "Hello"
    3) "12"
    所有这个hash中有3个field。

hgetall -- 获取某个hash中全部的field及value。

    > hgetall myhash
    1) "field2"
    2) "World"
    3) "field"
    4) "Hello"
    5) "field3"
    6) "12"  

### List——列表
List说白了就是链表（Redis使用双端链表实现的List），相信学过数据结构的人都应该能理解其结构。使用List结构，我们可以轻松地实现最新消息排行等功能（如新浪微博的TimeLiness）。List的另一个应用就是消息队列，可以使用List的PUSH操作，将任务存在List中，然后工作线程再用POP操作将任务取出进行运行执行。Redis还提供了操作List中某一段元素的API，你可以直接查询，删除List中的某一段元素。

应用场景：1、微博TimeLine 2、消息队列

### Set——集合
Set就是一个集合，集合的概念就是一堆不重复值得组合。利用Redis提供的Set数据结构，可以存储一些集合性的数据。比如在微博应用中，可以将一个用户所有的关注人存在一个集合。因为Redis非常人性化的集合提供了求交集、并集、差集等操作，那么就可以非常方便的实现如共同关注、共同爱好、二度好友等功能，对上面的所有集合操作，你还可以使用不同的命令选择将结果返回给客户端还是存集到一个新的集合中。

应用场景：

    1.共同好友、二度好友
    2.利用唯一性，可以统计访问网站的所有独立IP
    3.好友推荐的时候，根据tag求交集，大于某个threshold就可以推荐

### Sorted Set——有序集合
和Sets相比，Sorted Sets是将Set中的元素增加了一个权重参数score，使得集合中的元素能够按score进行有序排列，比如一个存储全班同学成绩的Sorted Set,其集合value可以是同学的学号，而score就可以是其考试得分，这样在数据插入集合的时候，就已经进行了天然的排序。另外还可以用Sorted Sets 来做带权重的队列，比如普通消息的score 为1 ，重要消息的score为2，必然工作线程可以选择按score的倒叙来获取工作任务。让重要的任务先执行。

应用场景：

    1.带权重的元素，比如一个游戏的用户得分排行榜
    2.比较复杂的数据结构，一般用到的场景不算太多

### 订阅-发布系统
Pub/Sub从字面上理解就是发布（Publish）与订阅（Subscribe），在Redis中，你可以设定对某一个key值进行消息发布及消息订阅，当一个key值上进行了消息发布后，所有订阅它的客户端都会收到相应的消息。这一功能最明显的用法就是用作实时消息系统，比如普通的即时聊天，群聊等功能。

### 事务——Transactions
谁说NoSQL都不支持事务，虽然Redis的Transactions 提供的并不是严格的ACID的事务（比如一串用EXEC提供执行的命令，在执行中服务宕机，那么会有一部分命令执行了，剩下的没执行），但是这个Transactions 还是提供了基本的命令打包执行的功能（在服务器上不出问题的情况下，可以保证一连串的命令式顺序在一起执行的，中间有会有其它客户端命令插进来执行）。  

Redis还提供了一个Watch功能，你可以对一个key进行Watch，然后再执行Transactions,在这过程中，如果这个Watched的值进行了修改，那么这个Transactions会发现并拒绝执行。

## 实战Redis服务器部署
## 总结与建议
### 总结
1. Redis使用最佳方式是全部数据in-memory
2. Redis更多场景是作为Memcached的替代来使用。
3. 当需要除key/value之外的更多数据结构支持时，使用Redis更适合。
4. 当存储的数据不能被剔除时，使用Redis更合适。

### 建议
1. 批量处理：
redis在处理数据时，最好是要进行批量处理，将一次处理1条数据改为多条，性能成倍提高。测试的目的就是要弄清楚批量和非批量处理之间的差别，性能差异非常大，所以在开发过程中尽量批量处理，即每次发送多条数据，以抵消网络速度影响。
2. 网络：
redis在处理时受网络影响非常大，所以，部署最好能在本机部署，如果本机部署redis，能获取10-20倍的性能。集群情况下，网络硬件、网速要求一定要高。
3. 内存：
如果没有足够内存，linux可能将redis一部分数据放到交换分区，导致读取速度非常慢导致超时。所以一定要预留足够多的内存供redis使用。
4. 少用get/set多用hashset
作为一个key/value 存在，很多开发者自然的使用set/get 方法来使用Redis，实际上这并不是最优化的使用方法。尤其是在未启用VM情况下，Redis全部数据需要放入内存，节约内存尤为重要。

假如一个key-value单元需要最小占用512字节，即使志存一个字节也占用了512字。这时候就有一个设计模式，可以把key复用， 几个key-value放入一个key中，value再作为一个set存入，这样同样512字节就会存放10-100倍的容量。

这就是为了节约内存，建议使用hashset而不是set/get的方法来使用Redis

## 命令速查
> http://doc.redisfans.com/
