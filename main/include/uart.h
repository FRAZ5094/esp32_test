void uart_set_tx_pin(int gpio_num);
void uart_set_baud_rate(int rate);
void uart_send_byte(char c);
void uart_send_str(char *str);

void uart_get_byte(const void *byte);
