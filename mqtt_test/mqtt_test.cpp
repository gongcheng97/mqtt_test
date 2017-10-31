// mqtt_test.cpp : �������̨Ӧ�ó������ڵ㡣
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

	//lib���ʼ��
	mosquitto_lib_init();
	//�����ͻ���ʵ��
	mosclient = mosquitto_new(itoa(rand(), (char*)session_id, 10), true, NULL);
	mosquitto_connect_callback_set(mosclient, mqtt_cmd_connect_callback);
	mosquitto_disconnect_callback_set(mosclient, mqtt_cmd_disconnect_callback);
	mosquitto_tls_set(mosclient, "ca.cer", NULL, NULL, NULL, NULL);  //����ϵԴ��ۺ缼��֧�ֻ��CA֤���ļ�
	mosquitto_message_retry_set(mosclient, 3);
	mosquitto_message_callback_set(mosclient, message_callback);
	mosquitto_username_pw_set(mosclient, "develop", "666666");

	if (mosquitto_connect(mosclient, "59.108.232.149", 8883, 60))
	{
		printf("Unable to connect mqtt server, exit\n");
	}
	else
	{
		mosquitto_subscribe(mosclient, NULL, "#", 0);
		mosquitto_loop_forever(mosclient, 30000, 1000);
	}
	mosquitto_destroy(mosclient);
	mosquitto_lib_cleanup();
	return 0;
}

