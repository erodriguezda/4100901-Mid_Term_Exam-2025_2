#include "room_control.h"

#include "gpio.h"    // Para controlar LEDs
#include "systick.h" // Para obtener ticks y manejar tiempos
#include "uart.h"    // Para enviar mensajes
#include "tim.h"     // Para controlar el PWM

// Estados de la sala
typedef enum {
    ROOM_IDLE,
    ROOM_OCCUPIED
} room_state_t;

// Variable de estado global
room_state_t current_state = ROOM_IDLE;
static uint32_t led_on_time = 0;
static uint8_t current_duty = PWM_INITIAL_DUTY;

static uint8_t prev_brightness = 0;      
static uint32_t b1_press_time = 0;       
static uint8_t b1_active = 0;            



void room_control_app_init(void)
{
   
    uart_send_string("Controlador de Sala v2.0\r\n");
    uart_send_string("- Lámpara: 20%\r\n");
    uart_send_string("- Puerta: Cerrada\r\n");

    // Inicializar PWM al duty cycle inicial (estado IDLE -> LED apagado)
    tim3_ch1_pwm_set_duty_cycle(PWM_INITIAL_DUTY);
    
}

void room_control_on_button_press(void)
{
    if (current_state == ROOM_IDLE) {
        current_state = ROOM_OCCUPIED;
        tim3_ch1_pwm_set_duty_cycle(100);  // PWM al 100%
        led_on_time = systick_get_ms();
        uart_send_string("Sala ocupada\r\n");
    } else {
        current_state = ROOM_IDLE;
        tim3_ch1_pwm_set_duty_cycle(0);  // PWM al 0%
        uart_send_string("Sala vacía\r\n");
    }
}

void room_control_on_uart_receive(char received_char)
{
    switch (received_char) {
        case 'h':
        case 'H':
            tim3_ch1_pwm_set_duty_cycle(100);
            uart_send_string("PWM: 100%\r\n");
            break;
        case 'l':
        case 'L':
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("PWM: 0%\r\n");
            break;
        case 'O':
        case 'o':
            current_state = ROOM_OCCUPIED;
            tim3_ch1_pwm_set_duty_cycle(100);
            led_on_time = systick_get_ms();
            uart_send_string("Sala ocupada\r\n");
            break;
        case 'I':
        case 'i':
            current_state = ROOM_IDLE;
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("Sala vacía\r\n");
            break;
        case '1':
            tim3_ch1_pwm_set_duty_cycle(10);
            current_duty = 10;
            uart_send_string("PWM: 10%\r\n");
            break;
        case '2':
            tim3_ch1_pwm_set_duty_cycle(20);
            current_duty = 20;
            uart_send_string("PWM: 20%\r\n");
            break;
        case '3':
            tim3_ch1_pwm_set_duty_cycle(30);
            current_duty = 30;
            uart_send_string("PWM: 30%\r\n");
            break;
        case '4':
            tim3_ch1_pwm_set_duty_cycle(40);
            current_duty = 40;
            uart_send_string("PWM: 40%\r\n");
            break;
        case '5':
            tim3_ch1_pwm_set_duty_cycle(50);
            current_duty = 50;
            uart_send_string("PWM: 50%\r\n");
            break;
        case 's':
        case 'S': {
            uart_send_string("Estado actual:\r\n");
            char buffer[40];
            sprintf(buffer, " - Lámpara: %u%%\r\n", get_current_duty());
            uart_send_string(buffer);
            if (current_state == ROOM_OCCUPIED)
                uart_send_string(" - Puerta: Abierta\r\n");
            else
                uart_send_string(" - Puerta: Cerrada\r\n");
            break;
}


        case '?':
            uart_send_string("\r\nComandos disponibles:\r\n");
            uart_send_string(" '1'-'5': Ajustar brillo lámpara (10%, 20%, 30%, 40%, 50%)\r\n");
            uart_send_string(" '0'   : Apagar lámpara\r\n");
            uart_send_string(" 'o'   : Abrir puerta (ocupar sala)\r\n");
            uart_send_string(" 'c'   : Cerrar puerta (vaciar sala)\r\n");
            uart_send_string(" 's'   : Estado del sistema\r\n");
            uart_send_string(" '?'   : Ayuda\r\n");
            break;


        

        default:
            uart_send_string("Comando desconocido: ");
            uart_send(received_char);
            uart_send_string("\r\n");
            break;
    }
}

void room_control_update(void)
{
    {
    
    if (gpio_read_pin(B1_GPIO_Port, B1_Pin) == 0 && b1_active == 0)  
    {
        prev_brightness = current_brightness;   
        set_brightness(100);                    
        b1_press_time = systick_get_ms();       
        b1_active = 1;                        
    }

    
    if (b1_active)
    {
        if (systick_get_ms() - b1_press_time >= 10000)   
        {
            set_brightness(prev_brightness);    
            b1_active = 0;                     
        }
    }

    
}
    if (current_state == ROOM_OCCUPIED) {
        if (systick_get_ms() - led_on_time >= LED_TIMEOUT_MS) {
            current_state = ROOM_IDLE;
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("Timeout: Sala vacía\r\n");
        }
    }
}

static uint8_t get_current_duty(void) {
    return current_duty;
}
