# include <stdio.h>
# include <stdlib.h>
# include <stddef.h>
# include <stdarg.h>
# include <string.h>
# include <assert.h>
# include <hiredis/hiredis.h>

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
