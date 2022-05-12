
struct channel;

struct channel* create_channel();
void channel_send(struct channel* chan, int x);
int channel_receive(struct channel* chan);
