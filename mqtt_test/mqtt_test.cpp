// mqtt_test.cpp : 定义控制台应用程序的入口点。
//

#include "mosquitto.h"
#include "stdio.h"
#include "stdlib.h"

static void mqtt_cmd_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	if (result) {
		printf("Connect mqtt broker failed!\n");
	}
	printf("Connect mqtt broker succ!\n");
}

static void mqtt_cmd_disconnect_callback(struct mosquitto *mosq, void *obj, int rc)
{
	printf("cmd mqtt disconnected, try to reconnect");
	int result = mosquitto_reconnect(mosq);
	if (result != MOSQ_ERR_SUCCESS)
	{
		printf("Reconnect to mqtt broker failed, exit\n");
	}
	else
		printf("Reconnect to mqtt broker succeed\n");
}

void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	if (message->payloadlen) {
		printf("%s %s\n", message->topic, message->payload);
	}
	else {
		printf("%s (null)\n", message->topic);
	}
}

int main(int argc, char *argv[])
{
	mosquitto *mosclient;
	char *session_id[15];

	//lib库初始化
	mosquitto_lib_init();
	//创建客户端实例
	mosclient = mosquitto_new(itoa(rand(), (char*)session_id, 10), true, NULL);
	//设置连接回调函数
	mosquitto_connect_callback_set(mosclient, mqtt_cmd_connect_callback);
	//设置断开连接回调函数
	mosquitto_disconnect_callback_set(mosclient, mqtt_cmd_disconnect_callback);
	//获取CA认证
	mosquitto_tls_set(mosclient, "ca.cer", NULL, NULL, NULL, NULL);  //请联系源清慧虹技术支持获得CA证书文件
	//设置重连时间
	mosquitto_message_retry_set(mosclient, 3);
	//设置消息回调
	mosquitto_message_callback_set(mosclient, message_callback);
	//设置用户名密码
	mosquitto_username_pw_set(mosclient, "develop", "666666");

	//连接
	if (mosquitto_connect(mosclient, "59.108.232.149", 8883, 60))
	{
		printf("Unable to connect mqtt server, exit\n");
	}
	else
	{
		//订阅所有主题
		//mosquitto_subscribe(mosclient, NULL, "#", 0);
		//测试订阅特定主题
		mosquitto_subscribe(mosclient, NULL, "SR/up/FAB411172016617", 0);
		//mosquitto_subscribe(mosclient, NULL, "SMB/FAB411172016617", 0);
		//循环
		mosquitto_loop_forever(mosclient, 30000, 1000);
	}
	mosquitto_destroy(mosclient);
	mosquitto_lib_cleanup();
	return 0;
}

