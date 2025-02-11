#pragma once

struct rlock;

struct rlock* rlock_create();
void rlock_destroy(struct rlock*);

void rlock_lock_reader(struct rlock*);
void rlock_lock_writer(struct rlock*);

void rlock_unlock_reader(struct rlock*);
void rlock_unlock_writer(struct rlock*);