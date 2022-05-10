#pragma once

struct fair_rwlock;

struct fair_rwlock* fair_rwlock_create();
void fair_rwlock_destroy(struct fair_rwlock*);

void fair_rwlock_unlock_reader(struct fair_rwlock*);
void fair_rwlock_unlock_writer(struct fair_rwlock*);

void fair_rwlock_lock_reader(struct fair_rwlock*);
void fair_rwlock_lock_writer(struct fair_rwlock*);
