#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

// This file implements synchronous channels.

struct channel {
	sem_t s1, s2;
	pthread_mutex_t m;
	int val;
};

struct channel* create_channel() {
	struct channel* chan = malloc(sizeof(*chan));
	return chan;
}

void channel_send(struct channel* chan, int x) {
	// I use a mutex to sequentialize the sending of messages
	pthread_mutex_lock(&chan->m);

	chan->val = x;

	// Notify that a message was sent
	sem_post(&chan->s1);

	// Wait until the message was received
	sem_wait(&chan->s2);

	pthread_mutex_unlock(&chan->m);
}

int channel_receive(struct channel* chan) {

	// Wait until a message was sent
	sem_wait(&chan->s1);

	int x = chan->val;

	// Notify that a message was received
	sem_post(&chan->s2);

	return x;
}
