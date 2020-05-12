#include "uchat.h"

void mx_send_json_to_all_in_room(t_server_info *info, json_object *json_obj) {
    int *sockets_online = mx_get_users_sock_in_room(&info,
        json_object_get_int(json_object_object_get(json_obj, "room_id")));
    t_socket_list *tls_list = NULL;
    const char *json_string = json_object_to_json_string(json_obj);

    for (int i = 0; i < info->wdb->i; i++) {
        tls_list = mx_find_socket_elem(info->socket_list, sockets_online[i]);
        if (tls_list) {
            mx_save_send(&tls_list->mutex, tls_list->tls_socket, json_string, strlen(json_string));
        }
    }
}

int mx_run_function_type(t_server_info *info, t_socket_list *csl) {
    int return_value = -1;
    int type = json_object_get_int(json_object_object_get(csl->obj, "type"));

    if (type == MX_MSG_TYPE)
        return_value = mx_process_message_in_server(info, csl->obj);
    else if (type == MX_FILE_SEND_TYPE)
        return_value = mx_save_file_in_server(info, csl);
    else if (type == MX_FILE_DOWNLOAD_TYPE)
        return_value = mx_send_file_from_server(info, csl);
    else if (type == MX_AUTH_TYPE)
    	return_value = mx_authorization(info, csl, csl->obj);
    else if (type == MX_REG_TYPE)
        return_value = mx_registration(info, csl, csl->obj);
    else if (type == MX_LOGOUT_TYPE)
        return_value = mx_logout(info, csl, csl->obj);
    else if (type == MX_LOAD_MORE_TYPE)
        return_value = mx_load_history(info, csl, csl->obj);
    return return_value;
}
