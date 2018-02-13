## Redis之C接口
+ 网址：
    > https://github.com/redis/hiredis

+ 下载：
    > https://github.com/redis/hiredis.git

+ 安装：
    > cd hiredis
    > make
    > sudo make install
    > 安装输出
    > mkdir -p /usr/local/include/hiredis /usr/local/include/hiredis/adapters /usr/local/lib  
    > cp -a hiredis.h async.h read.h sds.h /usr/local/include/hiredis  
    > cp -a adapters/*.h /usr/local/include/hiredis/adapters  
    > cp -a libhiredis.so /usr/local/lib/libhiredis.so.0.13  
    > cd /usr/local/lib && ln -sf libhiredis.so.0.13 libhiredis.so  
    > cp -a libhiredis.a /usr/local/lib  
    > mkdir -p /usr/local/lib/pkgconfig  
    > cp -a hiredis.pc /usr/local/lib/pkgconfig  
    > 
    > 默认库安装在"/usr/local/lib/"，设置加载库路径
    > vim ~/.bashrc
    > 最后一行写入
    > export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/
    > 重启终端

+ 设置共享库环境:
> 函数原型：redisContext *redisConnect(const char *ip, int port)

说明：该函数用来连接redis数据库，参数为数据库的ip地址和端口，一般redis数据库的端口为6379 该函数返回一个结构体redisContext。
> 函数原型: void * redisCommand(redisContext *c, const char * format, ...);

说明：该函数执行命令，就如sql数据库中的SQL语句一样，只是执行的是redis数据库中的操作命令，第一参数为连接数据库时返回的RedisContext，剩下的参数为变参，就如C标准函数print函数一样的变参。返回值为void *，一般强制转换成为redisReply类型的进行进一步的处理。
> 函数原型：void freeReplyObjec(void * repaly);

说明：释放redisCommand执行后返回的redisReply所占用的内存
> 函数原型：void redisFree(redisContext * c);

说明：释放redisConnect()所产生的连接。

+ 测试用例：
```c
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <hiredis/hiredis.h>

void doTest()
{
	// redis 默认监听端口为6379 可以在配置文件中修改
	redisContext * c = redisConnect("localhost", 6379);
	if (c -> err)
	{
		redisFree(c);
		printf("Connect to redisServer failure\n");
		return ;
	}
	printf("Connect to redisServer  success\n");

	const char * command1 = "set key1 itcast_1";
	redisReply * r = (redisReply *)redisCommand(c, command1);

	if ( NULL == r)
	{
		printf("Execute command1 failure\n");
		redisFree(c);
		return ;
	}

	if (! (r -> type == REDIS_REPLY_STATUS && strcasecmp(r -> str, "OK") == 0))
	{
		printf("Failed to execute command [%s]\n", command1);
		freeReplyObject(r);
		redisFree(c);
		return ;
	}
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command1);

	const char * command2 = "strlen key1";
	r = (redisReply *)redisCommand(c, command2);

	if ( NULL == r)
	{
		printf("Execute command2 failure\n");
		redisFree(c);
		return ;
	}

	if (r -> type != REDIS_REPLY_INTEGER)
	{
		printf("Failed to execute command [%s]\n", command2);
		freeReplyObject(r);
		redisFree(c);
		return ;
	}
	int length = r -> integer;
	freeReplyObject(r);
	printf("The length of 'key1' is %d.\n", length);
	printf("Succeed to execute command[%s]\n", command2);

	const char * command3 = "get key1";
	r = (redisReply *)redisCommand(c, command3);

	if ( NULL == r)
	{
		printf("Execute command3 failure\n");
		redisFree(c);
		return ;
	}

	if (r -> type != REDIS_REPLY_STRING)
	{
		printf("Failed to execute command [%s]\n", command3);
		freeReplyObject(r);
		redisFree(c);
		return ;
	}
	printf("The value of 'key1' is %s\n", r -> str);
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command3);

	const char * command4 = "get key2";
	r = (redisReply *)redisCommand(c, command4);

	if ( NULL == r)
	{
		printf("Execute command4 failure\n");
		redisFree(c);
		return ;
	}

	if (r -> type != REDIS_REPLY_NIL)
	{
		printf("Failed to execute command [%s]\n", command4);
		freeReplyObject(r);
		redisFree(c);
		return ;
	}
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command4);

	redisFree(c);
}

int main()
{
	doTest();
	return 0;
}
```

+ 编译
    > gcc hiredis.c -lhiredis -o hiredis

+ 运行
    > ./hiredis

## Redis之C++接口
Redis官方引入的C++库众多，但官方没有重点推荐使用哪个：
> https://redis.io/clients#c--

除此之外可以使用Redis官方提供的C语言实现的客户端hiredis，进行封装定制
```cpp
// redis.h

#ifndef _REDIS_H_
#define _REDIS_H_

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>

#include <hiredis/hiredis.h>

class Redis
{
public:
	Redis(){}
	~Redis()
	{
		this -> _connect = NULL;
		this -> _reply = NULL;
	}

	bool connect(std :: string host, int port)
	{
		this -> _connect = redisConnect(host.c_str(), port);
		if(this -> _connect != NULL && this -> _connect -> err)
		{
			printf("connect error: %s\n", this -> _connect -> errstr);
			return 0;
		}
		return 1;
	}
	std :: string get(std ::  string key)
	{
		this -> _reply = (redisReply *)redisCommand(this -> _connect, "GET %s", key.c_str());
		std :: string str = this -> _reply -> str;
		freeReplyObject(this -> _reply);
		return str;
	}
	void set(std :: string key, std :: string value)
	{
		redisCommand(this -> _connect, "SET %s %s", key.c_str(), value.c_str());
	}

private:
	redisContext *_connect;
	redisReply * _reply;
};

#endif
```
+ 测试用例
```cpp
// redis.cpp

# include "redis.h"

int main()
{
	Redis * r = new Redis();

	if (!r -> connect("127.0.0.1", 6379))
	{
		printf("connect error!\n");
		return 0;
	}
	r -> set("name", "itcast cpp");
	printf("Get the name is %s\n", r -> get("name").c_str());

	delete r;

	return 0;
}
```

+ Makefile编译：
```makefile
# Makefile

redis: redis.cpp redis.h
    g++ redis.cpp -o redis -L/usr/local/lib/ -lhiredis

clean:
    rm -rf redis
```

## Redis之python接口
+ 参考：
    > http://redis.io/clients#python
    > 推荐redis-py
    
+ 安装：
    > sudo pip install redis

Parser 可以控制如何解析redis相应的内容。redis-py包含两个Parser类，PythonParser 和HiredisParser。默认，如果已经安装了hiredis模块，redis-py会使用HiredisParser，否则会使用PythonParser。

HiredisParser是C编写的，由redis核心团队维护，性能要比PythonParser提高10倍以上，所以推荐使用。

+ 安装方法：
    > sudo pip install hiredis
    
+ 命令行使用：
```python
启动python或ipython
>>> import redis
>>> r = redis.StrictRedis(host = 'localhost', port = 6379, db = 0)
>>> r.set('foo', 'bar')
>>> True
>>> r.get('foo')
>>> 'bar'
```

+ 测试代码：
```python
import redis

class Database:
    def __init__(self):
        self.host = 'localhost'
        self.port = 6379
    def write(self, website, city, year, month, day, deal_number):
        try:
            key = '_'.join([website, city, str(year), str(month), str(day)])
            val = deal_number
            r = redis.StrictRedis(host = self.host, port = self.port)
            r.set(key, val)
            return 
        except Exception as exception:
            print(exception)
    def read(self, website, city, year, month, day):
        try:
            key = '_'.join([website, city, str(year), str(month), str(day)])
            r = redis.StrictRedis(host = self.host, port = self.port)
            value = r.get(key)
            return value
        except Exception as exception:
            print(exception)
if __name__ == "__main__":
    db = Database()
    db.write('itcastcpp', 'beijing', 2016, 1, 22, 8000)
    print(db.read('itcastcpp', 'beijing', 2016, 1, 22))
```
