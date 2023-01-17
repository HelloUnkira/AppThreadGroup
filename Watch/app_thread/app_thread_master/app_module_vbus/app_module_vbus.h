#ifndef APP_MODULE_VBUS_H
#define APP_MODULE_VBUS_H

/*app_module_vbus.h:
 *ͨ��app_module_vbus.py����
 *�ο�app_module_vbus.json�е�ģʽ����Դ
 *��ͨ��������Ϊһ�����׵�����
 *���ͬ�߳��в�ͬ��ģ�������
 *����Ҫע��ʵ��ִ�л���
 */

#include <stdint.h>

typedef struct {
	/* ���ݽ���ȡ����˫��Լ���׳� */
	uint32_t command;   /* ��Ӧ��ָ�� */
	uint32_t size;      /* ����Ԫ���� */
	void    *data;      /* ����Ԫ */
} app_module_vbus;

extern void app_module_vbus_respond(app_module_vbus *event);

#define APP_EVENT1		0x0001
#define APP_EVENT2		0x0002

#endif
