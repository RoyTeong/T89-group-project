//Include necessary libraries
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

// Server open on port 4242
#define TCP_PORT 4242
#define DEBUG_printf printf
#define BUF_SIZE 2048


// Datatype for TCP server data
typedef struct TCP_SERVER_T_ {
    struct tcp_pcb *server_pcb;
    struct tcp_pcb *client_pcb;
    uint8_t buffer_recv[BUF_SIZE];
    uint16_t recv_len;  // Add recv_len member
} TCP_SERVER_T;

// Close the TCP server connection
static err_t tcp_server_close(void *arg) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (state->client_pcb != NULL) {
        tcp_arg(state->client_pcb, NULL);
        tcp_poll(state->client_pcb, NULL, 0);
        tcp_sent(state->client_pcb, NULL);
        tcp_recv(state->client_pcb, NULL);
        tcp_err(state->client_pcb, NULL);
        tcp_close(state->client_pcb);
        state->client_pcb = NULL;
    }
    if (state->server_pcb) {
        tcp_arg(state->server_pcb, NULL);
        tcp_close(state->server_pcb);
        state->server_pcb = NULL;
    }
    return ERR_OK;
}

// Data coming in from TCP client
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (!p) {
        return ERR_OK;
    }
    if (p->tot_len > 0) {
        // Buffer receives incoming data
        const uint16_t buffer_left = BUF_SIZE - state->recv_len;
        state->recv_len += pbuf_copy_partial(p, state->buffer_recv + state->recv_len,
                                            p->tot_len > buffer_left ? buffer_left : p->tot_len, 0);
        tcp_recved(tpcb, p->tot_len);

        // Output buffer when received
        // Print char data
        for (int i = 0; i < state->recv_len; i++) {
            DEBUG_printf("%c", state->buffer_recv[i]);
        }
        DEBUG_printf("\n");

        // Send ack back to client. Client will output ack_msg.
        const char* ack_msg = "Message received!";
        tcp_write(tpcb, ack_msg, strlen(ack_msg), 1);
    }
    pbuf_free(p);

    // Check for whole buffer
    if (state->recv_len == BUF_SIZE) {
        // Clear buffer
        memset(state->buffer_recv, 0, BUF_SIZE);
        state->recv_len = 0;
        // Close connection
        tcp_server_close(arg);
    }
    return ERR_OK;
}

// Handle TCP server error
static void tcp_server_err(void *arg, err_t err) {
    // Use this if any TCP errors

    // if (err != ERR_ABRT) {
    //     DEBUG_printf("tcp_client_err_fn %d\n", err);
    // }
}

// Handle TCP server connections
static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
    
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (err != ERR_OK || client_pcb == NULL) {
        DEBUG_printf("Failure in accept\n");
        return ERR_VAL;
    }
    DEBUG_printf("Client connected\n");

    state->client_pcb = client_pcb;
    tcp_arg(client_pcb, state);
    tcp_sent(client_pcb, NULL);

    //TCP client data reception and errors callback
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

// Open the TCP server connection
static bool tcp_server_open(void *arg) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    DEBUG_printf("Starting server at %s on port %u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), TCP_PORT);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb) {
        DEBUG_printf("failed to create pcb\n");
        return false;
    }

    err_t err = tcp_bind(pcb, NULL, TCP_PORT);
    if (err) {
        DEBUG_printf("failed to bind to port %u\n", TCP_PORT);
        return false;
    }

    state->server_pcb = tcp_listen(pcb);
    if (!state->server_pcb) {
        DEBUG_printf("failed to listen\n");
        if (pcb) {
            tcp_close(pcb);
        }
        return false;
    }

    tcp_arg(state->server_pcb, state);

    // Callback for TCP server connections
    tcp_accept(state->server_pcb, tcp_server_accept);

    return true;
}

static TCP_SERVER_T* create_tcp_server(void) {
    TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));

    if (!state) {
        DEBUG_printf("failed to allocate state\n");
        return NULL;
    }
    
    // Open the TCP server connection
    if (!tcp_server_open(state)) {
        tcp_server_close(state);
        cyw43_arch_deinit();
        return NULL;
    }
    
    return state;

}

int wifi_init()
{

    if (cyw43_arch_init()) {
        DEBUG_printf("failed to initialise\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms("Gabriel", "ifconfig", CYW43_AUTH_WPA2_MIXED_PSK, 30000)) {
        DEBUG_printf("failed to connect.\n");
        return 1;
    }

    return 0;

}

int main() {
    stdio_init_all();

    // Initialize the Wi-Fi driver
    if (wifi_init()) {
        cyw43_arch_deinit();
        return 1;
    }

    // Initialize the TCP server and open the connection
    TCP_SERVER_T *state = create_tcp_server();
    if (!state) {
        cyw43_arch_deinit();
        return 1;
    }

    while (1) {
        // The main loop
        cyw43_arch_poll(); // Poll for Wi-Fi driver or lwIP work
        sleep_ms(1000);
    }

    return 0;
}
