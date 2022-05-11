#pragma once

struct wlock;

struct wlock* wlock_create();
void wlock_destroy(struct wlock*);

void wlock_lock_reader(struct wlock*);
void wlock_lock_writer(struct wlock*);

void wlock_unlock_reader(struct wlock*);
void wlock_unlock_writer(struct wlock*);