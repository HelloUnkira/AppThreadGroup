/*app_module_vbus.c:
 *ͨ��app_module_vbus.py����
 *�ο�app_module_vbus.json�е�ģʽ����Դ
 *��ͨ��������Ϊһ�����׵�����
 *���ͬ�߳��в�ͬ��ģ�������
 *����Ҫע��ʵ��ִ�л���
 */

#include "app_module_vbus.h"

/* APP_EVENT1 */
/* this is response set: */
extern void app_module_vbus_empty(app_module_vbus *event);
extern void app_module_vbus_empty(app_module_vbus *event);
/* APP_EVENT2 */
/* this is response set: */
extern void app_module_vbus_empty(app_module_vbus *event);

extern void app_module_vbus_respond(app_module_vbus *event)
{
	switch (event->command) {
	case APP_EVENT1:
		app_module_vbus_empty(event);
		app_module_vbus_empty(event);
		break;
	case APP_EVENT2:
		app_module_vbus_empty(event);
		break;
	}
}

void app_module_vbus_empty(app_module_vbus *event)
{
}
