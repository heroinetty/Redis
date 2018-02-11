## Redis配置文件

标签（空格分隔）：参考文档

---

在启动Redis服务器时，我们需要为其制定一个配置文件，缺省情况下配置文件在Redis的源码目录下，文件名为redis.conf。

redis配置文件被分成了几大区域，他们分别是：
1. 通用（general）
2. 快照（snapshotting）
3. 复制（replication）
4. 安全（security）
5. 限制（limits）
6. 追加模式（append only mode）
7. LUA脚本（lua scripting）
8. 慢日志（slow log）
9. 事件通知（event notification）

为了对Redis的系统实现一个直接的认识，我们首先来看一下Redis的配置文件中定义了那些主要参数以及这些参数的作用。

- daemonize no
    > 默认情况下，redis不是在后台运行的。如果需要在后台运行，把该项的值更改为yes；

- pidfile /var/run/redis.pid
    > 当Redis在后台运行的时候，Redis默认会把pid文件放在/var/run/redis.pid，你可以配置其他地址。当运行多个redis服务时，需要指定不同的pid文件和端口；

- port 6379
    > 指定redis运行的端口，默认是6379；

- bind 127.0.0.1
    > 指定redis只接收来自于该IP地址的请求，如果不进行设置，那么将处理所有请求。在生产环境中最好设置该项；

- loglevel debug
    > 指定日志记录级别，其中Redis总共支持四个级别：debug、verbose、notice、warning，默认为verbose。debug表示记录很多信息，用于开发和测试。verbose表示记录有用的信息，但不像debug会记录那么多。notice表示普通的verbose，常用语生产环境。warning表示只有非常重要或者严重的信息会记录到日志；

- logfile /var/log/redis/redis.log
    > 配置log文件地址，默认值为stdout。若后台默认会输出到/dev/null；

- databases 16
    > 可用数据库数，默认值为16，默认数据库为0。数据库范围在0-（database-1）之间；

- save
    > 保存数据到磁盘格式为save < seconds > < changes >，指出在多长时间内，有多少次更新操作，就将数据同步到数据文件rdb。相当于条件触发抓取快照，这个可以多个条件配合。save 900 1 就表示900秒内有1个key被改变就保存数据到磁盘；

- rdbcompression yes
    > 存储至本地数据时（持久化到rdb文件）是否压缩数据，默认为yes；

- dbfilename dump.rdb
    > 本地持久化数据库文件名，默认值为dump.rdb；

- dir ./
    > 工作目录，数据库镜像备份的文件放置的路径。这里的路径跟文件名要分开配置是因为redis在进行备份时，先会将当前数据库的状态写入到一个临时文件中，在备份完成时，再把该临时文件替换为上面所指定的文件。而这里的临时文件和上面所配置的备份文件都会放在这个指定的路径当中，AOF文件也会存放在这个目录下面。注意这里必须指定一个目录而不是文件；

- slaveof
    > 主从复制，设置改数据库为其他数据库的从数据库。设置当本机为slave服务时，设置master服务的IP地址及端口；

- masterauth
    > 当master服务设置了密码保护时（用requirepass制定的密码）slave服务连接master的密码；

- slave-serve-stale-data yes
    > 当从库同主机失去连接或者复制正在进行，从机库有两种运行方式：如果slave-serve-stale-data设置为yes(默认设置)，从库会继续响应客户端的请求。如果slave-serve-stale-data设置为no，除去INFO和SLAVOF命令之外的任何请求都会返回一个错误"SYNC with master in progress"；

- repl-ping-slave-period 10
    > 从库会按照一个时间间隔想主库发送PING，可以通过repl-ping-slave-period 设置这个时间间隔，默认是10秒；

- repl-timeout 60
    > 设置主库批量数据传输时间或者ping回复时间间隔，默认值为60秒，一定要确保repl-timeout大于repl-ping-slave-period；

- requirepass foobared
    > 设置客户端连接后进行任何其他指定项需要使用的密码，因为redis速度相当快，所以在一台比较好的服务器下，一个外部的用户可以在一秒钟进行150K次密码尝试，这意味着你需要指定非常强大的密码来防止暴力破解；

- rename-command CONFIG ""
    > 命令重命名，在一个共享环境下可以重命名相对危险的命令。比如把CONFIG重名为一个不容易猜测的字符：# rename-command CONFIG b840fc02d524045429941cc15f59e41cb7be6c52。如果想删除一个命令，直接把它重命名为一个空字符即可，rename-command CONFIG ""

- maxclients 128
    > 设置同一时间最大客户端连接数，默认无限制，Redis可以同时打开客户端相当于Redis进程可以打开的最大文件描述符数。如果设置maxclients 0，表示不作限制。当客户端连接数到达限制时，Redis会关闭新的连接并向客户端；

- maxmemory
    > 指定Redis最大内存限制。Redis在启动时会把数据加载到内存中，达到最大内存后，Redis会先尝试清楚已到期或即将到期的Key，Redis同时也会移除空的list对象。当此方法处理后，仍然到达最大内存设置，将无法再进行写入操作，但仍然可以进行读取操作。注意：Redis新的vm机制，会把Key存放内存，Value会存放在swap区；

- maxmemory-policy volatile-lru
    > 当内存达到最大值的时候Redis会选择删除那些数据呢？有五种方式可供选择：  
    > volatile-lru代表利用LRU算法移除设置过期时间的key（LRU：最近使用Least Recently Used），  
    > allkeys-lru代表利用LRU算法移除任何key，  
    > volarile-random代表移除设置过过期时间的随机key，  
    > allkeys-random代表移除一个随机的key，  
    > volatile-ttl代表移除即将过期的key（minor TTL），  
    > noeviction代表不移除任何key，只是返回一个写错误。  
    > 
    > 注意：对于上面的策略，如果没有合适的key可以移除，写的时候Redis会返回一个错误；

- appendonly no
    > 默认情况下，redis会在后台异步的把数据库镜像备份到磁盘，但是该备份是非常耗时的，而且备份也不能很频繁。如果发生诸如拉闸限电、拔插头等情况，那么将造成比较大范围的数据丢失，所以redis提供了另外一种更加搞笑的数据库备份及灾难恢复方式。开启append only 模式之后，redis会把所接收到的每一次写操作请求都追加到appendonly.aof文件中。当redis重新启动时，会从该文件恢复出之前的状态，但是这样会造成appendonly.aof文件过大，所以redis还支持了BGREWRITEAOF指令对appendonly.aof 进行重新整理，你可以同时开启asynchronous dumps 和 AOF；

- appendfilename appendonly.aof
    > AOF文件名称，默认为"appendonly.aof"；

- appendfsync everysec
    > Redis支持三种同步AOF文件的策略：no代表不进行同步，系统去操作，always代表每次有写操作都进行同步，everysec代表对写操作进行累计，每秒同步一次，默认是"everysec"，按照速度和安全折中这是最好的；

- slowlog-log-slower-than 10000
    > 记录超过特定执行时间的命令。执行时间不包括I/O计算，比如连接客户端，返回结果等，只是命令执行时间。可以通过两个参数设置slow log；一个是告诉Redis执行超过多少时间被记录的参数slowlog-log-slower-than（微妙），另一个是slow log的长度。当一个新命令被记录的时候最早的命令将从队列中移除，下面的时间以微妙为单位，因此1000000代表一分钟，注意制定一个负数将关闭慢日志，而设置为0将强制每个命令都会记录；

- hash-max-zipmap-entries 512 && hash-max-zipmap-value 64
    > 当hash中包含超过指定元素个数并且最大的元素没有超过临界时，hash将以一种特殊的编码方式（大大减少内存使用）来存储，这里可以设置这两个临界值。Redis Hash对应Value内部实际就是一个HashMap，实际这里会有2中不同实现。这个Hash的成员比较少时Redis为了节省内存会采用类似一维数组的方式来紧凑存储，而不会采用真正的HashMap结构，对应的value redis Object的encoding的zipmap。当成员数量增大时会自动转成真正的HashMap，此时encoding为ht；

- list-max-ziplist-entries 512
    > list数据类型多少节点以下会采用去指针的紧凑存储格式；

- list-max-ziplist-value 64
    > 数据类型节点值大小小于多少字节会采用紧凑存储格式；

- set-max-intset-entries 512
    > set数据类型内部数据如果全部是数值型，且包含多少节点以下会采用紧凑格式存储；

- zset-max-ziplist-entries 128
    > zsort 数据类型多少节点以下会采用去指针的紧凑存储格式；

- zset-max-ziplist-value 64
    > zsort 数据类型节点值大小小于多少字节会采用紧凑存储格式；

- activerehashing yes
    > Redis 将在每100毫秒时使用1毫秒的CPU时间来对redis的hash表进行重新hash，可以降低内存的使用。当你的使用场景中，有非常严格的实时性需要，不能够接受Redis时不时的对请求有2毫秒的延迟的话，把这项配置为no。如果没有这么严格的实时性要求，可以设置为yes，以便能够尽可能快的释放内存；
