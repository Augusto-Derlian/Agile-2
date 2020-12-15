// Usar o PI para controlar o Duty cycle do PWM.

// Debug LCD
//sbit LCD_RS at RD5_bit;
//sbit LCD_EN at RD4_bit;
//sbit LCD_D4 at RD0_bit;
//sbit LCD_D5 at RD1_bit;
//sbit LCD_D6 at RD2_bit;
//sbit LCD_D7 at RD3_bit;

//sbit LCD_RS_Direction at TRISD5_bit;
//sbit LCD_EN_Direction at TRISD4_bit;
//sbit LCD_D4_Direction at TRISD0_bit;
//sbit LCD_D5_Direction at TRISD1_bit;
//sbit LCD_D6_Direction at TRISD2_bit;
//sbit LCD_D7_Direction at TRISD3_bit;


#define K1 0.5172f
#define K2 0.4391f


float duty_cycle;
float ref = 3.5f;
float u=0, erro, leitura;
float u_ant =0, erro_ant;
char Debug[5];
long counter =0;

void interrupt(){                       // Interrupção para garantir o TS em 2ms
                                        // 0.5us * 16 * 250 = 2 ms para
                                        // atualizar o duty_cycle
     if(T0IF_bit){
        T0IF_bit = 0x00;
        counter =1;
        TMR0 = 0x06;    // reinicia o contador para garantir os 250 contagens

     } // end IF
} // end interrupt


void main(){
   // configurar o parâmetros do PIC
   ADCON0 = 0b00010001; // Seleciona RA5 com entrada analógica
   ANSEL  = 0b00010000; // Configura AN4 pin como entrada analógica
   C1ON_bit = 0;                      // Disable comparators
   C2ON_bit = 0;

   // Configurações da interrupção para o controle

   INTCON = 0b11100000;     // Habilita Intrrupção geral/por periféricos/
                            // Estouro TMR0
   OPTION_REG = 0b10000011; // Define o presacle para 1:8, desabilita resistores
                            // de pull-up

   TMR0 = 0x06; // Inicia o Timer0 em 06 para garantir Ts.

   TRISC = 0x00;  // Configura o PORTC como saída
   PORTC = 0x00;  // Inicializa os pinos em nível lógico baixo
   //TRISD = 0x00;  // idem para Debug
   //PORTD = 0x00;  // idem para Debug

   // Configuração do PWM

   PWM1_init(10000); // configura o PWM pra um frequência de 10kHz
   duty_cycle = 127; // condição inicial do PWM em 50%
   PWM1_Start();
   PWM1_Set_Duty(duty_cycle);

   // configuração do LDC
   //Lcd_Init();
   //Lcd_Cmd(_LCD_CLEAR);
   //Lcd_Cmd(_LCD_CURSOR_OFF);

   while(1){
        leitura = ADC_Read(4);
        leitura = (leitura*5.0f/1024.0f);  // 0.0488 = (5/1024)
        //FloatToStr(leitura,Debug);
        //Lcd_Out(1,1,Debug);
        erro = ref - leitura;
        //FloatToStr(erro,Debug);
        //Lcd_Out(1,1,Debug);
        if(counter == 1){
                   u = K1*erro - K2*erro_ant + u_ant;
                   PORTC.RC0 = !PORTC.RC0;
                   if(u > 255)
                   {
                        duty_cycle = 255;
                   }else{
                   if( u<0){
                       duty_cycle = 0;
                   }else{
                         duty_cycle = u;
                   }
        }
        u_ant = u;
        //FloatToStr(u,Debug);
        //Lcd_Out(2,1,Debug);
        erro_ant = erro;
        counter = 0;

        }// end atualização de duty cycle
   PWM1_Set_Duty(duty_cycle);
   }// end loop principal

}// end main