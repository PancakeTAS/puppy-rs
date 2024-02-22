#define NEKOSBEST_IMPL
#include "api.h"

int fetch_endpoints(void (*register_callback)(command_data*)) {
    // fetch all endpoints
    nekos_endpoint_list endpoints;
    nekos_status status = nekos_endpoints(&endpoints);
    if (status != NEKOS_OK) {
        log_fatal("Failed to fetch endpoints");
        return 1;
    }

    // create commands
    for (size_t i = 0; i < endpoints.len; i++) {
        command_data command = {
            .endpoint = endpoints.endpoints[i],
            .command_description = NULL,
            .command_name = NULL,
            .response_message = NULL,
            .response_message_user = NULL
        };

        // register command
        register_callback(&command);
    }

    return 0;
}
