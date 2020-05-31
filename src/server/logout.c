#include "uchat.h"

int mx_delete_message (t_server_info *info, t_socket_list *csl, json_object *js) {
    (void)csl;
    int message_id = json_object_get_int(json_object_object_get(js, "message_id"));
    char *command = malloc(1024);

    sprintf(command, "DELETE FROM msg_history where id = %d;", message_id);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        mx_send_json_to_all_in_room(info, js);
        mx_strdel(&command);
    }
    else {
        printf("fail\n");
    }
    return 1;
}

static int load_user_data(void *js, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    json_object *name = NULL;
    json_object *email = NULL;
    json_object *login = json_object_new_string(argv[2]);
    if (argv[4])
        name = json_object_new_string(argv[4]);
    else
        name = json_object_new_string("");
    if (argv[5])
        email = json_object_new_string(argv[5]);
    else
        email = json_object_new_string("");
    json_object *visual_n = json_object_new_int(atoi(argv[9]));
    json_object *audio_n = json_object_new_int(atoi(argv[10]));
    json_object *email_n = json_object_new_int(atoi(argv[11]));

    json_object_object_add((struct json_object *)js, "login", login);
    json_object_object_add((struct json_object *)js, "name", name);
    json_object_object_add((struct json_object *)js, "email", email);
    json_object_object_add((struct json_object *)js, "visual_n", visual_n);
    json_object_object_add((struct json_object *)js, "audio_n", audio_n);
    json_object_object_add((struct json_object *)js, "email_n", email_n);
    return 0;
}

int mx_leave_room (t_server_info *info, t_socket_list *csl, json_object *js) {
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
    char *command = malloc(1024);
    const char *json_string = NULL;

    sprintf(command, "DELETE FROM room_user where user_id='%d' and room_id='%d';", user_id, room_id);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        mx_send_json_to_all_in_room(info, js);
        json_string = json_object_to_json_string(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
        mx_strdel(&command);
    }
    else {
        printf("fail\n");
    }
    return 1;
}

int get_room_id(void *js, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;

    if (argv[0]) {
        struct json_object *t = json_object_new_int(atoi(argv[0]));
        json_object_object_add((struct json_object*) js, "room_id", t);
        return 0;
    }
    return 1;
}

int get_logins(void *name, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    char **str = (char  **)name;

    if (argv[0]) {
        *str = mx_strjoin(*str, argv[0]);
        return 0;
    }
    return 1;
}

void get_name(t_server_info *info, char **name, int first_id, int second_id) {
    char *command = malloc(1024);

    sprintf(command, "SELECT login FROM users WHERE id='%d' OR id='%d';", first_id, second_id);
    sqlite3_exec(info->db, command, get_logins, name, NULL);
    mx_strdel(&command);
}

int mx_direct_message (t_server_info *info, t_socket_list *csl, json_object *js) {
    int first_id = json_object_get_int(json_object_object_get(js, "first_id"));
    int second_id = json_object_get_int(json_object_object_get(js, "second_id"));
    char *command = malloc(1024);
    char *name = mx_strnew(1);
    const char *json_string = NULL;

    get_name(info, &name, first_id, second_id);
    sprintf(command, "SELECT * FROM rooms INNER JOIN direct_rooms USING(id) \
        WHERE access=3 AND ((first_id='%d' AND second_id='%d') OR (first_id='%d' AND second_id='%d'));",
        first_id, second_id, second_id, first_id);
    sqlite3_exec(info->db, command, get_room_id, js, NULL);
    if (json_object_get_int(json_object_object_get(js, "room_id")) != 0) {
        json_object_object_add(js, "exist", json_object_new_int(1));
        json_string = json_object_to_json_string(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
        mx_strdel(&command);
    }
    else {
        char *command2 = malloc(1024);
        sprintf(command2, "INSERT INTO rooms (name, access) VALUES ('%s', '3'); \
        SELECT last_insert_rowid();" , name);
        if (sqlite3_exec(info->db, command2, get_room_id, js, NULL) == SQLITE_OK) {
            int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
            char *command3 = malloc(1024);
            sprintf(command3, "INSERT INTO room_user (room_id, user_id, role) VALUES ('%d', '%d', '0'); \
                    INSERT INTO room_user (room_id, user_id, role) VALUES ('%d', '%d', '0'); \
                    INSERT INTO direct_rooms (id, first_id, second_id) VALUES ('%d', '%d', '%d');" , 
                    room_id, first_id, room_id, second_id, room_id, first_id, second_id);
            if (sqlite3_exec(info->db, command3, NULL, NULL, NULL) == SQLITE_OK) {
                json_object *room_data = json_object_new_object();;
                json_object_object_add(room_data, "room_id", json_object_new_int(room_id));
                json_object_object_add(room_data, "name", json_object_new_string(name));
                json_object *messages = json_object_new_array();
                json_object_object_add(room_data, "access", json_object_new_int(3));
                json_object_object_add(room_data, "messages", messages);
                json_object_object_add(js, "room_data", room_data);
                json_object_object_add(js, "exist", json_object_new_int(0));
                mx_send_json_to_all_in_room(info, js);
            }
            else
                printf("fail2\n");
        }
        else
            printf("fail1\n");
    }
    return 1;
}


int mx_create_room_server (t_server_info *info, t_socket_list *csl, json_object *js) {
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int access = json_object_get_int(json_object_object_get(js, "acces"));
    const char *name = json_object_get_string(json_object_object_get(js, "name"));
    const char *json_string = NULL;
    json_object *room_data = json_object_object_get(js, "room_data");
    char *command = malloc(1024);

    sprintf(command, "INSERT INTO rooms (name, access) VALUES ('%s', '%d'); \
        SELECT last_insert_rowid();" , name, access);
    if (sqlite3_exec(info->db, command, get_room_id, room_data, NULL) == SQLITE_OK) {
        int room_id = json_object_get_int(json_object_object_get(room_data, "room_id"));
        json_object_object_add(js, "room_id", json_object_new_int(room_id));
        char *command1 = mx_strnew(1024);

        sprintf(command1, "INSERT INTO room_user (user_id, room_id, role) VALUES ('%d', '%d', '%d');", user_id, room_id ,1);
        if (sqlite3_exec(info->db, command1, NULL, NULL, 0) == SQLITE_OK) {
            json_string = json_object_to_json_string(js);
            mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
            mx_strdel(&command1);
            mx_strdel(&command);
        }
        else {
            printf("fail2\n");
        }
    }
    else {
        printf("fail\n");
    }
    return 1;
}

int mx_join_room (t_server_info *info, t_socket_list *csl, json_object *js) {
    (void)csl;
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
    char *command = malloc(1024);
    mx_print_json_object(js, "mx_process_input_from_server");

    sprintf(command, "INSERT INTO room_user (user_id, room_id, role) VALUES ('%d', '%d', '%d');", user_id, room_id, 0);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        json_object *room_data = json_object_object_get(js, "room_data");
        json_object *messages = json_object_new_array();
        char *command1 = mx_strnew(1024);

        json_object_object_add(room_data, "messages", messages);
        sprintf(command1, "SELECT *  FROM msg_history, users \
                where room_id = %d and users.id = msg_history.user_id order by msg_history.id desc limit 5;", room_id);
        if (sqlite3_exec(info->db, command1, mx_get_rooms_data, messages, 0) == SQLITE_OK) {
            mx_strdel(&command1);
            mx_send_json_to_all_in_room(info, js);
            mx_strdel(&command);
        }
        else {
            printf("fail2\n");
        }
    }
    else {
        printf("fail\n");
    }
    return 1;
}

static int search_rooms(void *array, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        json_object *room = json_object_new_object();
        json_object *id = json_object_new_int(atoi(argv[0]));
        json_object *name = json_object_new_string(argv[1]);
        json_object *acces = json_object_new_int(atoi(argv[2]));

        json_object_object_add(room, "id", id);
        json_object_object_add(room, "name", name);
        json_object_object_add(room, "acces", acces);
        json_object_array_add((struct json_object *)array, room);
    }
    return 0;
}

static int search_users(void *array, int argc, char **argv, char **col_name) {
    (void)argc;
    (void)col_name;
    if (argv[0]) {
        json_object *user = json_object_new_object();
        json_object *id = json_object_new_int(atoi(argv[0]));
        json_object *login = json_object_new_string(argv[2]);
        // json_object *email = json_object_new_string(argv[5]);
        // json_object *name = json_object_new_string(argv[4]);

        json_object_object_add(user, "id", id);
        // json_object_object_add(user, "name", name);
        json_object_object_add(user, "login", login);
        // json_object_object_add(user, "email", email);
        json_object_array_add((struct json_object *)array, user);
    }
    return 0;
}

int mx_search_all (t_server_info *info, t_socket_list *csl, json_object *js) {
    const char *query = json_object_get_string(json_object_object_get(js, "query"));
    char *command = malloc(1024);
    char *command1 = malloc(1024);
    const char *json_string = NULL;
    json_object *array_rooms = json_object_new_array();
    json_object *array_users = json_object_new_array();

    json_object_object_add(js, "rooms", array_rooms);
    json_object_object_add(js, "users", array_users);
    if (strcmp(query, "All") == 0) {
        sprintf(command, "SELECT * FROM rooms;");
        sprintf(command1, "SELECT * FROM users;");
    }
    else {
        sprintf(command, "SELECT * FROM rooms WHERE name LIKE '%%%s%%' AND NOT access=3;", query);
        sprintf(command1, "SELECT * FROM users WHERE login LIKE '%%%s%%';", query);
    }
    if (sqlite3_exec(info->db, command, search_rooms, array_rooms, NULL) != SQLITE_OK) {
        printf("fail\n");
        return 0;
    }
    if (sqlite3_exec(info->db, command1, search_users, array_users, NULL) != SQLITE_OK) {
        printf("fail\n");
        return 0;
    }

    json_string = json_object_to_json_string(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
    mx_strdel(&command);
    return 1;
}

int mx_load_profile (t_server_info *info, t_socket_list *csl, json_object *js) {
    int id = json_object_get_int(json_object_object_get(js, "id"));
    char *command = malloc(1024);
    const char *json_string = NULL;

    sprintf(command, "SELECT * FROM users, user_notifications where users.id='%d' and user_notifications.user_id='%d';", id, id);
    if (sqlite3_exec(info->db, command, load_user_data, js, NULL) == SQLITE_OK) {
        json_string = json_object_to_json_string(js);
        mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
        mx_strdel(&command);
    }
    else {
        printf("fail\n");
    }
    return 1;
}

int mx_edit_profile (t_server_info *info, t_socket_list *csl, json_object *js) {
    int user_id = json_object_get_int(json_object_object_get(js, "user_id"));
    int add_info = json_object_get_int(json_object_object_get(js, "add_info"));
    char *command = malloc(1024);
    const char *json_string = NULL;

    if (!add_info) {
        const char *column = json_object_get_string(json_object_object_get(js, "column"));
        const char *data = json_object_get_string(json_object_object_get(js, "data"));
        sprintf(command, "UPDATE users SET %s='%s' where id='%d';", column, data, user_id);
    }
    else {
        int visual = json_object_get_int(json_object_object_get(js, "visual_n"));
        int audio = json_object_get_int(json_object_object_get(js, "audio_n"));
        int email = json_object_get_int(json_object_object_get(js, "email_n"));
        sprintf(command, "UPDATE user_notifications SET visual='%d', \
            audio='%d', email='%d' where user_id='%d';", visual, audio, email, user_id);
    }
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) 
        json_object_object_add(js, "confirmation", json_object_new_int(1));
    else 
        json_object_object_add(js, "confirmation", json_object_new_int(0));
    json_string = json_object_to_json_string(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
    mx_strdel(&command);
    return 1;
}


int mx_edit_message (t_server_info *info, t_socket_list *csl, json_object *js) {
    (void)csl;
    int message_id = json_object_get_int(json_object_object_get(js, "message_id"));
    const char *data = json_object_get_string(json_object_object_get(js, "data"));
    char *command = malloc(1024);

    sprintf(command, "UPDATE msg_history SET message='%s' where id='%d';", data, message_id);
    if (sqlite3_exec(info->db, command, NULL, NULL, NULL) == SQLITE_OK) {
        mx_send_json_to_all_in_room(info, js);
        mx_strdel(&command);
    }
    else {
        printf("fail\n");
    }
    return 1;
}


int mx_load_history (t_server_info *info, t_socket_list *csl, json_object *js) {
    const char *json_string = NULL;
    int room_id = json_object_get_int(json_object_object_get(js, "room_id"));
    int last_id = json_object_get_int(json_object_object_get(js, "last_id"));
    char *command = malloc(1024);
    json_object *messages = json_object_new_array();

    json_object_object_add(js, "messages", messages);
    sprintf(command, "SELECT *  FROM msg_history, users \
                where room_id = %d and msg_history.id < %d and users.id = msg_history.user_id \
                order by msg_history.id desc limit 15;", room_id, last_id);
    if (sqlite3_exec(info->db, command, mx_get_rooms_data, messages, NULL) == SQLITE_OK) {
        printf("succes\n");
    }
    else {
        printf("fail\n");
    }
    json_string = json_object_to_json_string(js);
    mx_save_send(&csl->mutex, csl->tls_socket, json_string, strlen(json_string));
    mx_strdel(&command);
    return 1;
}

int mx_logout (t_server_info *i, t_socket_list *csl, json_object *js) {
	(void)csl;
    //const char *json_string = NULL;
    char *command = malloc(1024);
    int id = json_object_get_int(json_object_object_get(js, "user_id"));

    sprintf(command, "UPDATE users SET socket='0' WHERE id='%s'", mx_itoa(id));
    if (sqlite3_exec(i->db, command, NULL, NULL, NULL) == SQLITE_OK) {
		printf("succes\n");
    }
    else {
    	printf("fail\n");
    }
    mx_strdel(&command);
	return 1;
}
