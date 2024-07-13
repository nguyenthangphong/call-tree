#ifndef CALL_TREE_TYPE_H
#define CALL_TREE_TYPE_H

enum ct_mode_t {
    MODE_ERROR, BUILD, RUN, CLEAN
};

enum ct_flag_t {
    FLAG_ERROR, FSTACK_USAGE, FDUMP_RTL_EXPAND
};

enum ct_status_t {
    STATUS_ERROR = -2, STATUS_FAIL, STATUS_OK
};

#endif // CALL_TREE_TYPE_H
