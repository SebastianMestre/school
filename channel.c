#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

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
	// exclusion mutua para que no se pise val
	pthread_mutex_lock(&chan->m);

	chan->val = x;

	// notifico que envie un mensaje
	sem_post(&chan->s1);

	// espero a que se haya recibido el mensaje
	sem_wait(&chan->s2);

	pthread_mutex_unlock(&chan->m);
}

int channel_receive(struct channel* chan) {

	// espero a que se haya enviado un mensaje
	sem_wait(&chan->s1);

	int x = chan->val;

	// notifico que recibi un mensaje
	sem_post(&chan->s2);

	return x;
}
