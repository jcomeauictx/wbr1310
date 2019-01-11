#define NODE_PATH   "/var/proc/web/"

int get_session_id(struct request *r);
int get_sessiongrp(struct request *r);
int check_session(struct request *r);
