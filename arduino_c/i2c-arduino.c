#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define ADDRESS 0x04

static const char *devName = "/dev/i2c-1";

struct message {
	struct message *next;
	int data;
};

typedef struct {
	pthread_mutex_t lock;
	pthread_cond_t more;
	struct message *newest;
	struct message *oldest;
} queue;
#define QUEUE_INITIALIZER { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NULL, NULL }

void enqueue(queue *q, const int m) {
	struct message *n;
	n->next = NULL;
	n->data = m;
	pthread_mutex_lock(&(q->lock));
	if(q->newest == NULL) {
		q->newest = n;
		q->oldest = n;
	} else {
		q->newest->next = n;
		q->newest = n;
	}
	pthread_cond_signal(&(q->more));
	pthread_mutex_unlock(&(q->lock));
}

int dequeue(queue *q) {
	pthread_mutex_lock(&(q->lock));
	while(q->newest == NULL) {
		pthread_cond_wait(&(q->more), &(q->lock));
	}
	int result = q->newest->data;
	q->newest = q->newest->next;
	if(q->newest == NULL) {
		q->oldest = NULL;
	}
	pthread_mutex_unlock(&(q->lock));
	return result;
}

int file;
queue q = QUEUE_INITIALIZER;
void worker_thread() {
	printf("Worker thread started\n");
	while(1) {
		int num = dequeue(&q);
		printf("Sending %d\n", num);
		if(write(file, &num, 1) == 1) {
			usleep(100000);
			char buf[1];
			if(read(file, buf, 1) == 1) {
				int temp = (int) buf[0];
				printf("Received %d\n", temp);
			}
		} else {
			fprintf(stderr, "I2C: Failed to send message\n");
		}
	}
}

int main(int argc, char** argv) {
	int num;
	printf("I2C: Connecting\n");
	if((file = open(devName, O_RDWR)) < 0) {
		fprintf(stderr, "I2C: Failed to access %s\n", devName);
		exit(1);
	}

	printf("I2C: Acquiring bus to 0x%02x\n", ADDRESS);
	if(ioctl(file, I2C_SLAVE, ADDRESS) < 0) {
		fprintf(stderr, "I2C: Failed to acquire bus access to slave 0x%x\n", ADDRESS);
		exit(1);
	}

	pthread_t thread;
	pthread_create(&thread, NULL, (void *) &worker_thread, NULL);

	printf("Please enter a number between 0 and 255: ");
	while(~scanf("%d", &num)) {
		if(num >= 0 && num <= 255) {
			enqueue(&q, num);
		} else {
			fprintf(stderr, "Not a valid number!\n");
		}
		printf("Please enter a number between 0 and 255: ");
	}

	pthread_cancel(thread);
	pthread_join(thread, NULL);

	return 0;
}
