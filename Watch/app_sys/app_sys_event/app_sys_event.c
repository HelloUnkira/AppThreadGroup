/*app_sys_event.c:
 *ͨ��app_sys_event.py����
 *�ο�app_sys_event.json�е�ģʽ����Դ
 *��ͨ��������Ϊһ�����׵�����
 *���ͬ�߳��в�ͬ��ģ�������
 *����Ҫע��ʵ��ִ�л���
 */

#include "app_sys_event.h"

/* APP_EVENT1 */
/* this is response set: */
extern void app_sys_event_empty(app_sys_event *event);
extern void app_sys_event_empty(app_sys_event *event);
/* APP_EVENT2 */
/* this is response set: */
extern void app_sys_event_empty(app_sys_event *event);

extern void app_sys_event_respond(app_sys_event *event)
{
	switch (event->command) {
	case APP_EVENT1:
		app_sys_event_empty(event);
		app_sys_event_empty(event);
		break;
	case APP_EVENT2:
		app_sys_event_empty(event);
		break;
	}
}
void app_sys_event_empty(app_sys_event *event)
{
}

