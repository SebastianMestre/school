#pragma once

struct rwlock;

struct rwlock* rwlock_create();
void rwlock_destroy(struct rwlock*);

void rwlock_unlock_reader(struct rwlock*);
void rwlock_unlock_writer(struct rwlock*);

void rwlock_lock_reader(struct rwlock*);
void rwlock_lock_writer(struct rwlock*);