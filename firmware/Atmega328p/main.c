
//Código para el funcionamiento Aquaguard con el sensor de turbidez, pH y temperatura y la comunicación UART by César Alejandro Tolentino Mendoza
#define F_CPU 8000000UL  // Configuración de la frecuencia de reloj a 8 MHz
#include <avr/io.h>
#include <util/delay.h>
#include <string.h> // Para funciones de manipulación de cadenas

// Declaración de funciones para el manejo del LCD
void escribe(int accion, int dato);
void iniciaLCD(void);
void gotoXY(int fila, int col);

// Declaración de funciones para la comunicación UART
void UART_init(uint16_t ubrr_value);
void UART_send_char(char data);
void UART_send_string(const char *str);

// Declaración de funciones para el ADC
void ADC_init(void);
uint16_t ADC_read(uint8_t ch);

// Definiciones y declaraciones para el sensor DS18B20
#define DS18B20_PIN PD2
#define DS18B20_PORT PORTD
#define DS18B20_DDR DDRD
#define DS18B20_PIN_REG PIND

uint8_t DS18B20_reset(void);
void DS18B20_write_bit(uint8_t bit);
uint8_t DS18B20_read_bit(void);
void DS18B20_write_byte(uint8_t byte);
uint8_t DS18B20_read_byte(void);
float DS18B20_get_temp(void);

// Declaración de funciones auxiliares
void escribeNumero(float num, uint8_t enteros, uint8_t decimales);
void UART_send_float(float num, uint8_t enteros, uint8_t decimales);
void float_to_string(float num, char *str, uint8_t enteros, uint8_t decimales);
int32_t pow_int(int32_t base, int32_t exponent);
void int_to_str(int32_t num, char *str);

int main(void) {
    // Variables para almacenar las lecturas de los sensores
    uint16_t adc_turbidez = 0;
    uint16_t adc_ph = 0;
    float ntu = 0.0;
    float phValue = 0.0;
    float temperature = 0.0;

    // Inicialización de periféricos
    iniciaLCD();
    UART_init(103); // Configuración de UART a ~9600 bps con F_CPU de 8MHz
    ADC_init();

    while (1) {
        // Lectura del sensor de turbidez en el canal ADC2
        adc_turbidez = ADC_read(2);
        float voltage_turbidez = adc_turbidez * (5.0 / 1023.0);
        ntu = (voltage_turbidez - 3.56) * 1000.0 / -3.56;
        if (ntu < 0.0) ntu = 0.0;
        if (ntu > 1000.0) ntu = 1000.0;

        // Lectura del sensor de pH en el canal ADC3
        adc_ph = ADC_read(3);
        float phVoltage = adc_ph * (5.0 / 1023.0);

        // Calibración del sensor de pH
        const float pH9_voltage = 2.935;
        const float pH7_voltage = 3.200;
        const float pH4_voltage = 3.660;
        float slopeLow = (4.0 - 7.0) / (pH4_voltage - pH7_voltage);
        float interceptLow = 7.0 - (slopeLow * pH7_voltage);
        float slopeHigh = (9.0 - 7.0) / (pH9_voltage - pH7_voltage);
        float interceptHigh = 7.0 - (slopeHigh * pH7_voltage);

        if (phVoltage >= pH7_voltage) {
            phValue = (slopeHigh * phVoltage) + interceptHigh;
        } else {
            phValue = (slopeLow * phVoltage) + interceptLow;
        }

        // Lectura del sensor de temperatura DS18B20
        temperature = DS18B20_get_temp();

        // Actualización de la pantalla LCD
        escribe(1, 0x01); // Comando para limpiar pantalla
        _delay_ms(2);

        // Visualización de NTU
        gotoXY(1, 0);
        escribe(2, 'N'); escribe(2, 'T'); escribe(2, 'U'); escribe(2, ':');
        escribeNumero(ntu, 3, 1);

        // Visualización de pH
        gotoXY(1, 8);
        escribe(2, 'p'); escribe(2, 'H'); escribe(2, ':');
        escribeNumero(phValue, 2, 2);

        // Visualización de Temperatura
        gotoXY(2, 0);
        escribe(2, 'T'); escribe(2, 'e'); escribe(2, 'm'); escribe(2, 'p'); escribe(2, ':');
        if (temperature != -999.0) {
            escribeNumero(temperature, 2, 1);
            escribe(2, 'C');
        } else {
            escribe(2, 'E'); escribe(2, 'r'); escribe(2, 'r'); escribe(2, 'o'); escribe(2, 'r');
        }

        // Envío de datos a través de UART
        UART_send_string("NTU: ");
        UART_send_float(ntu, 4, 1);
        UART_send_string(", pH: ");
        UART_send_float(phValue, 2, 2);
        UART_send_string(", Temp: ");
        if (temperature != -999.0) {
            UART_send_float(temperature, 3, 1);
            UART_send_string("C\n");
        } else {
            UART_send_string("Error\n");
        }

        _delay_ms(1000); // Espera antes de la siguiente lectura
    }
    return 0;
}

// Implementación de funciones para el manejo del LCD

void escribe(int accion, int dato) {
    int activa;
    int regresa;
    if (accion == 1) {
        activa = 0xF0;  // RS=0, E=0 para comandos
        regresa = 0xF1; // Restablece E a 1
    } else {
        activa = 0xF2;  // RS=1, E=0 para datos
        regresa = 0xF3; // Restablece E a 1
    }
    PORTC = regresa;
    PORTB = dato;
    PORTC = activa;
    _delay_us(40);
    PORTC = regresa;
    _delay_us(40);
}

void iniciaLCD(void) {
    DDRB = 0xFF;     // Configura PORTB como salida
    DDRC = 0x03;     // Configura PORTC0 y PORTC1 como salida
    _delay_ms(50);   // Espera para la inicialización del LCD

    escribe(1, 0x38); // Configura el modo del LCD
    _delay_us(39);

    escribe(1, 0x0C); // Enciende el display sin cursor
    _delay_us(39);

    escribe(1, 0x06); // Configura el modo de entrada
    _delay_us(39);

    escribe(1, 0x01); // Limpia la pantalla
    _delay_ms(2);
}

void gotoXY(int fila, int col) {
    int coordenada;
    if (fila == 1) {
        coordenada = 0x80 + col;
    } else {
        coordenada = 0xC0 + col;
    }
    escribe(1, coordenada);
}

// Implementación de funciones para la comunicación UART

void UART_init(uint16_t ubrr_value) {
    UCSR0A |= (1 << U2X0); // Habilita doble velocidad
    UBRR0H = (ubrr_value >> 8);
    UBRR0L = ubrr_value;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Configura frame: 8 datos, 1 stop
    UCSR0B = (1 << TXEN0); // Habilita transmisión
}

void UART_send_char(char data) {
    while (!(UCSR0A & (1 << UDRE0))); // Espera hasta que el buffer esté vacío
    UDR0 = data; // Envía el carácter
}

void UART_send_string(const char *str) {
    while (*str) {
        UART_send_char(*str++);
    }
}

// Implementación de funciones para el ADC

void ADC_init(void) {
    ADMUX = (1 << REFS0); // Referencia AVcc
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Habilita ADC y prescaler de 64
}

uint16_t ADC_read(uint8_t ch) {
    ch &= 0b00000111; // Selecciona el canal ADC
    ADMUX = (ADMUX & 0xF8) | ch;
    ADCSRA |= (1 << ADSC); // Inicia conversión
    while (ADCSRA & (1 << ADSC)); // Espera a que finalice
    return ADC;
}

// Implementación de funciones para el sensor DS18B20

uint8_t DS18B20_reset(void) {
    uint8_t i;
    DS18B20_DDR |= (1 << DS18B20_PIN); // Configura el pin como salida
    DS18B20_PORT &= ~(1 << DS18B20_PIN); // Pone el pin en bajo
    _delay_us(480);

    DS18B20_DDR &= ~(1 << DS18B20_PIN); // Configura el pin como entrada
    _delay_us(60);

    i = (DS18B20_PIN_REG & (1 << DS18B20_PIN)); // Lee el estado del pin
    _delay_us(420);

    return i;
}

void DS18B20_write_bit(uint8_t bit) {
    DS18B20_DDR |= (1 << DS18B20_PIN); // Configura el pin como salida
    DS18B20_PORT &= ~(1 << DS18B20_PIN); // Pone el pin en bajo
    _delay_us(1);

    if (bit)
        DS18B20_DDR &= ~(1 << DS18B20_PIN); // Libera el pin si el bit es 1
    else
        DS18B20_DDR |= (1 << DS18B20_PIN); // Mantiene el pin en bajo si el bit es 0

    _delay_us(60);
    DS18B20_DDR &= ~(1 << DS18B20_PIN); // Libera el pin
}

uint8_t DS18B20_read_bit(void) {
    uint8_t bit = 0;
    DS18B20_DDR |= (1 << DS18B20_PIN); // Configura el pin como salida
    DS18B20_PORT &= ~(1 << DS18B20_PIN); // Pone el pin en bajo
    _delay_us(1);

    DS18B20_DDR &= ~(1 << DS18B20_PIN); // Configura el pin como entrada
    _delay_us(14);

    if (DS18B20_PIN_REG & (1 << DS18B20_PIN))
        bit = 1;

    _delay_us(45);
    return bit;
}

void DS18B20_write_byte(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        DS18B20_write_bit(byte & 0x01);
        byte >>= 1;
    }
}

uint8_t DS18B20_read_byte(void) {
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        byte >>= 1;
        if (DS18B20_read_bit())
            byte |= 0x80;
    }
    return byte;
}

float DS18B20_get_temp(void) {
    uint8_t temp_lsb, temp_msb;
    int16_t temp;
    float temperature;

    if (DS18B20_reset())
        return -999.0; // Error en la comunicación

    DS18B20_write_byte(0xCC); // Comando Skip ROM
    DS18B20_write_byte(0x44); // Comando de conversión de temperatura
    _delay_ms(750); // Espera a que finalice la conversión

    if (DS18B20_reset())
        return -999.0; // Error en la comunicación

    DS18B20_write_byte(0xCC); // Comando Skip ROM
    DS18B20_write_byte(0xBE); // Comando de lectura del scratchpad

    temp_lsb = DS18B20_read_byte();
    temp_msb = DS18B20_read_byte();

    temp = (temp_msb << 8) | temp_lsb;
    temperature = temp * 0.0625; // Conversión a grados Celsius

    return temperature;
}

// Implementación de funciones auxiliares

void escribeNumero(float num, uint8_t enteros, uint8_t decimales) {
    int32_t valorEntero = (int32_t)num;
    int32_t valorDecimal = (int32_t)((num - valorEntero) * pow_int(10, decimales) + 0.5);

    if (num < 0) {
        escribe(2, '-');
        valorEntero = -valorEntero;
        valorDecimal = -valorDecimal;
    }

    for (int i = enteros - 1; i >= 0; i--) {
        int32_t divisor = pow_int(10, i);
        int8_t digito = (valorEntero / divisor) % 10;
        escribe(2, '0' + digito);
    }

    escribe(2, '.');

    for (int i = decimales - 1; i >= 0; i--) {
        int32_t divisor = pow_int(10, i);
        int8_t digito = (valorDecimal / divisor) % 10;
        escribe(2, '0' + digito);
    }
}

void UART_send_float(float num, uint8_t enteros, uint8_t decimales) {
    char buffer[20];
    float_to_string(num, buffer, enteros, decimales);
    UART_send_string(buffer);
}

void float_to_string(float num, char *str, uint8_t enteros, uint8_t decimales) {
    int32_t valorEntero = (int32_t)num;
    float parteDecimal = num - (float)valorEntero;
    int32_t valorDecimal = (int32_t)(parteDecimal * pow_int(10, decimales) + 0.5);

    if (num < 0) {
        *str++ = '-';
        valorEntero = -valorEntero;
        valorDecimal = -valorDecimal;
    }

    char bufferEntero[12];
    int_to_str(valorEntero, bufferEntero);
    strcpy(str, bufferEntero);
    str += strlen(bufferEntero);

    *str++ = '.';

    char bufferDecimal[12];
    int_to_str(valorDecimal, bufferDecimal);
    int decimalLength = strlen(bufferDecimal);
    for (int i = 0; i < decimales - decimalLength; i++) {
        *str++ = '0';
    }
    strcpy(str, bufferDecimal);
    str += strlen(bufferDecimal);

    *str = '\0';
}

void int_to_str(int32_t num, char *str) {
    int i = 0;
    int isNegative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    while (num != 0) {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    if (isNegative)
        str[i++] = '-';

    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int32_t pow_int(int32_t base, int32_t exponent) {
    int32_t result = 1;
    for (int32_t i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}
