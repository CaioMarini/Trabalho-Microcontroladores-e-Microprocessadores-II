// Counts for how long the machine has been running

#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

// Linhas do teclado ligadas na porta B
#define keypadDirectionRegisterR DDRB
#define keypadPortControlR PORTB
#define keypadPortValueR PINB

// Colunas do teclado ligadas na porta B
#define keypadDirectionRegisterC DDRC
#define keypadPortControlC PORTC
#define keypadPortValueC PINC

#define ctrl PORTD // Usando a porta D
#define en 2       // En (Enable) do lcd no pino 2
#define rw 1       // RW (Read/Write) no pino 1
#define rs 0       // RS (register) no pino 0

//Funções usadas
void lcd_command(unsigned char cmd);
void lcd_init(void);
void lcd_data(unsigned char data);
void lcdCommand(char);
void lcdData(char);
void lcd_print(unsigned char *str);
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_update_time(void);
void keypadScan(void);
int caracQuant = 0;


int main() {
	DDRD = 0xFF; // Colocar por D como Saida
	lcd_init();  // Iniciar o LCD
	_delay_ms(5);//Delay

	lcd_gotoxy(1, 1);         // Ir para posição 1,1 do LCD
	lcd_print("Digite"); // Mostra a mensagem
	lcd_gotoxy(1, 2);         // Va para a posição 1,2 do LCD

	// inicialização do teclado
	keypadDirectionRegisterR = (1<<0) | (1<<1) | (1<<2) | (1<<3);
	keypadDirectionRegisterC = (0<<0) | (0<<1) | (0<<2) | (0<<3);

	keypadPortControlR = (0<<0) | (0<<1) | (0<<2) | (0<<3);
	keypadPortControlC = (1<<0) | (1<<1) | (1<<2) | (1<<3);
	
	while (1) {
		keypadScan();
	};

	return 0;
}

void keypadScan(){
	
	// Variavel para armazenar a coluna
	uint8_t keyPressCodeC = keypadPortValueC;
	
	keypadDirectionRegisterC ^= (1<<0) | (1<<1) | (1<<2) | (1<<3);
	keypadDirectionRegisterR ^= (1<<0) | (1<<1) | (1<<2) | (1<<3);
	
	keypadPortControlC ^= (1<<0) | (1<<1) | (1<<2) | (1<<3);
	keypadPortControlR ^= (1<<0) | (1<<1) | (1<<2) | (1<<3);
	
	_delay_ms(5);
	
	// Variavel para armazenar a linha
	int temp = keypadPortValueR;
	uint8_t keyPressCodeR = temp << 4;
	
	// Contador para a linha e coluna
	uint8_t keyPressCode = keyPressCodeC | keyPressCodeR;
	uint8_t blinkDuration = 0;
	
	
	
	// Coluna 1
	
	
	if(keyPressCode == 0b11101110){
		lcd_print("1");
		caracQuant++;
		
	}
	if(keyPressCode == 0b11011110){
		lcd_print("4");
		caracQuant++;
	}
	if(keyPressCode == 0b10111110){
		lcd_print("7");
		caracQuant++;
	}
	if(keyPressCode == 0b01111110){
		lcd_print("*");
		caracQuant++;
	}
	
	// Coluna 2
	if(keyPressCode == 0b11101101){
		lcd_print("2");
		caracQuant++;
	}
	if(keyPressCode == 0b11011101){
		lcd_print("5");
		caracQuant++;
	}
	if(keyPressCode == 0b10111101){
		lcd_print("8");
		caracQuant++;
	}
	if(keyPressCode == 0b01111101){
		lcd_print("0");
		caracQuant++;
	}
	
	// Coluna 3
	if(keyPressCode == 0b11101011){
		lcd_print("3");
		caracQuant++;
	}
	if(keyPressCode == 0b11011011){
		lcd_print("6");
		caracQuant++;
	}
	if(keyPressCode == 0b10111011){
		lcd_print("9");;
		caracQuant++;
	}
	if(keyPressCode == 0b01111011){
		lcd_print("=");
		caracQuant++;
	}
	
	// Coluna 4
	if(keyPressCode == 0b11100111){
		lcd_print("A");
		caracQuant++;
	}
	if(keyPressCode == 0b11010111){
		lcd_print("B");
		caracQuant++;
	}
	if(keyPressCode == 0b10110111){
		lcd_print("C");
		caracQuant++;
	}
	if(keyPressCode == 0b01110111){
		lcd_print("D");
		caracQuant++;
	}
	//se caracQuant for >= 16 o diplsy é limpo e começa a digitar novamente
	if (caracQuant >= 16)
	{
		lcdCommand(0x01); // Limpa o LCD
		_delay_ms(1);
		lcdCommand(0x80); // —8 go to first line and –0 is for 0th position
		_delay_ms(1);
		lcd_print("Digite"); //Digita o texto
		lcd_gotoxy(1, 2);
		caracQuant = 0;
	}
}

// Função para avançar a posição no display
void lcd_gotoxy(unsigned char x, unsigned char y) {
	unsigned char firstCharAdr[] = {0x80, 0xC0, 0x94, 0xD4};
	lcdCommand(firstCharAdr[y - 1] + x - 1);
	_delay_ms(0.1);
}

void lcd_init(void) {
	lcdCommand(0x02); // inicializa o LCD no modo 4 bits
	_delay_ms(1);
	lcdCommand(0x28); // inicializa o LCD em 2 linhas, 5X7 na matriz de 4 bits
	_delay_ms(1);
	lcdCommand(0x01); // Limpa o LCD
	_delay_ms(1);
	lcdCommand(0x0E); // Liga o curso
	_delay_ms(1);
	lcdCommand(0x80); // Inicia na posição inicial
	_delay_ms(1);
	return;
}

void lcdCommand(char cmd_value) {
	char cmd_value1;
	cmd_value1 = cmd_value & 0xF0;//Mascara os 4 bits inferiores porque PD4 a PD7 são usadas
	lcd_command(cmd_value1);//Envia o texto para o LCD
	cmd_value1 = ((cmd_value << 4) & 0xF0); // Desloca 4 bits e mascara
	lcd_command(cmd_value1);// Envia  o texto para o LCD
}

void lcdData(char data_value) {
	char data_value1;
	data_value1 = data_value & 0xF0;//Mascara os 4 bits porque PD4 a PD7 são usadas
	lcd_data(data_value1);
	data_value1 = ((data_value << 4) & 0xF0); //Desloca 4 bits e mascara
	lcd_data(data_value1); // Envia para o LCD
}

void lcd_command(unsigned char cmd) {
	ctrl = cmd;
	ctrl &= ~(1 << rs); // RS = 0, para comando
	ctrl &= ~(1 << rw); // RW = 0, para escrever no LCD
	ctrl |= (1 << en);  // EN = 1, passndo de alto para baixo
	_delay_ms(1);
	ctrl &= ~(1 << en); // EN = 0, passando de alto para baixo
	_delay_ms(40);
	return;
}

void lcd_data(unsigned char data) {
	ctrl = data;
	ctrl |= (1 << rs);  // RS = 1, dados
	ctrl &= ~(1 << rw); // RW = 0, escrever
	ctrl |= (1 << en);  // EN = 1, passando de alto para baixo
	_delay_ms(1);
	ctrl &= ~(1 << en); // EN = 0, passando de alto para baixo
	_delay_ms(40);
	return;
}

void lcd_print(
unsigned char *str) { // armazena o endereço da string Str
	int i = 0;
	while (str[i] !=
	'\0') {     // fica em loop até encontrar o fim da string
		lcdData(str[i]); // envia para o LCD byte a byte
		i++;
	}
	return;
}