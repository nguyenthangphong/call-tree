#ifndef CALL_TREE_TYPE_H
#define CALL_TREE_TYPE_H

#define REGEX_GET_FUNCTION_MAIN              "^;; Function (?P<mangle>.*)\\s+\\((?P<function>\\S+)(,.*)?\\).*$"
#define REGEX_GET_FUNCTION_CALLED            "^.*\\(call.*\"(?P<target>.*)\".*$"
#define REGEX_GET_FUNCTION_SYMBOL_REF        "^.*\\(symbol_ref.*\"(?P<target>.*)\".*$"
#define REGEX_GET_FUNCTION_EXCLUDE           "R_OSAL|memcpy"

enum ct_mode_t {
    MODE_ERROR, BUILD, RUN
};

enum ct_flag_t {
    FLAG_ERROR, FSTACK_USAGE, FDUMP_RTL_EXPAND
};

enum ct_status_t {
    STATUS_ERROR = -1, STATUS_OK
};

typedef struct {
    QList<QString> calls;
    QMap<QString, bool> refs;
} ct_function_data_t;

#endif // CALL_TREE_TYPE_H
