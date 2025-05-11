#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <zenoh-pico.h>

int main(void) {
    const char *keyexpr = "demo/example/zenoh-pico-pub";
    const char *value = "Publish from LicheeRV Nano!";
    const int publish_count = 10;

    z_owned_config_t config;
    z_config_default(&config);

    z_owned_session_t session;
    if (z_open(&session, z_move(config), NULL) < 0) {
        printf("Failed to open Zenoh session.\n");
        return -1;
    }

    if (zp_start_read_task(z_loan_mut(session), NULL) < 0 ||
        zp_start_lease_task(z_loan_mut(session), NULL) < 0) {
        printf("Failed to start Zenoh tasks.\n");
        z_drop(z_move(session));
        return -1;
    }

    z_owned_publisher_t publisher;
    z_view_keyexpr_t ke;
    if (z_view_keyexpr_from_str(&ke, keyexpr) < 0) {
        printf("Invalid key expression: %s\n", keyexpr);
        z_drop(z_move(session));
        return -1;
    }
    if (z_declare_publisher(z_loan(session), &publisher, z_loan(ke), NULL) < 0) {
        printf("Failed to declare publisher.\n");
        z_drop(z_move(session));
        return -1;
    }

    for (int i = 0; i < publish_count; ++i) {
        char buf[128];
        snprintf(buf, sizeof(buf), "[%d] %s", i, value);
        printf("Publishing: %s\n", buf);

        z_owned_bytes_t payload;
        z_bytes_copy_from_str(&payload, buf);
        z_publisher_put(z_loan(publisher), z_move(payload), NULL);

        z_sleep_s(1);
    }

    z_drop(z_move(publisher));
    z_drop(z_move(session));

    return 0;
}
