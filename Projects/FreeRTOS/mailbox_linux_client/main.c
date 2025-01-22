#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

enum SYSTEM_CMD_TYPE {
	CMDQU_SEND = 1,
	CMDQU_SEND_WAIT,
	CMDQU_SEND_WAKEUP,
};

enum IP_TYPE {
	IP_ISP = 0,
	IP_VCODEC,
	IP_VIP,
	IP_VI,
	IP_RGN,
	IP_AUDIO,
	IP_SYSTEM,
	IP_CAMERA,
	IP_LIMIT,
};




#define RTOS_CMDQU_DEV_NAME "/dev/cvi-rtos-cmdqu"
#define RTOS_CMDQU_SEND                         _IOW('r', CMDQU_SEND, unsigned long)
#define RTOS_CMDQU_SEND_WAIT                    _IOW('r', CMDQU_SEND_WAIT, unsigned long)
#define RTOS_CMDQU_SEND_WAKEUP                  _IOW('r', CMDQU_SEND_WAKEUP, unsigned long)

enum SYS_CMD_ID {
	SYS_CMD_INFO_TRANS = 0x50,
	SYS_CMD_INFO_LINUX_INIT_DONE,
	SYS_CMD_INFO_RTOS_INIT_DONE,
	SYS_CMD_INFO_STOP_ISR,
	SYS_CMD_INFO_STOP_ISR_DONE,
	SYS_CMD_INFO_LINUX,
	SYS_CMD_INFO_RTOS,
	SYS_CMD_SYNC_TIME,
	SYS_CMD_INFO_DUMP_MSG,
	SYS_CMD_INFO_DUMP_EN,
	SYS_CMD_INFO_DUMP_DIS,
	SYS_CMD_INFO_DUMP_JPG,
	SYS_CMD_INFO_TRACE_SNAPSHOT_START,
	SYS_CMD_INFO_TRACE_SNAPSHOT_STOP,
	SYS_CMD_INFO_TRACE_STREAM_START,
	SYS_CMD_INFO_TRACE_STREAM_STOP,
	SYS_CMD_INFO_LIMIT,
};


enum DUO_LED_STATUS {
	DUO_LED_ON	= 0x02,
	DUO_LED_OFF,
    DUO_LED_DONE,
};

struct valid_t {
	unsigned char linux_valid;
	unsigned char rtos_valid;
} __attribute__((packed));

typedef union resv_t {
	struct valid_t valid;
	unsigned short mstime; // 0 : noblock, -1 : block infinite
} resv_t;

typedef struct cmdqu_t cmdqu_t;
/* cmdqu size should be 8 bytes because of mailbox buffer size */
struct cmdqu_t {
	unsigned char ip_id;
	unsigned char cmd_id : 7;
	unsigned char block : 1;
	union resv_t resv;
	unsigned int  param_ptr;
} __attribute__((packed)) __attribute__((aligned(0x8)));

int main()
{
    int ret = 0;
    int fd = open(RTOS_CMDQU_DEV_NAME, O_RDWR);
    if(fd <= 0)
    {
        printf("open failed! fd = %d\n", fd);
        return 0;
    }

    struct cmdqu_t cmd = {0};
    cmd.ip_id = IP_SYSTEM;
    cmd.cmd_id = SYS_CMD_INFO_LINUX;
    // cmd.resv.mstime = 100;

    // cmd.param_ptr = DUO_LED_ON;
    ret = ioctl(fd , RTOS_CMDQU_SEND, &cmd);
    if(ret < 0)
    {
        printf("ioctl error!\n");
        close(fd);
    }
    sleep(1);

    close(fd);
    return 0;
}
