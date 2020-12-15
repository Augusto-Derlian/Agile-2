// Usar o PI para controlar o Duty cycle do PWM.

#define K1 0.5172f // Constante de controle proporcional
#define K2 0.4391f // Constante de controle integral


float duty_cycle;
float ref = 3.5f; // Tensao de referencia da carga
float u=0, erro, leitura;
float u_ant = 0, erro_ant;
long conta_quantidade_de_interrupcoes = 0;

void interrupt(){                       // Interrup��o para garantir o TS em 2ms
                                        // 0.5us * 16 * 250 = 2 ms para
                                        // atualizar o duty_cycle
     if(T0IF_bit){
        T0IF_bit = 0x00;
        conta_quantidade_de_interrupcoes = 1;
        TMR0 = 0x06;    // reinicia o contador para garantir os 250 contagens

     } // end IF
} // end interrupt


void main(){
   // configurar o par�metros do PIC
   ADCON0 = 0b00010001; // Seleciona RA5 com entrada anal�gica
   ANSEL  = 0b00010000; // Configura AN4 pin como entrada anal�gica
   C1ON_bit = 0;                      // Disable comparators
   C2ON_bit = 0;

   // Configura��es da interrup��o para o controle

   INTCON = 0b11100000;     // Habilita Intrrup��o geral/por perif�ricos/
                            // Estouro TMR0
   OPTION_REG = 0b10000011; // Define o presacle para 1:8, desabilita resistores
                            // de pull-up

   TMR0 = 0x06; // Inicia o Timer0 em 06 para garantir Ts.

   TRISC = 0x00;  // Configura o PORTC como sa�da
   PORTC = 0x00;  // Inicializa os pinos em n�vel l�gico baixo
   //TRISD = 0x00;  // idem para Debug
   //PORTD = 0x00;  // idem para Debug

   // Configura��o do PWM

   PWM1_init(10000); // configura o PWM pra um frequ�ncia de 10kHz
   duty_cycle = 127; // condi��o inicial do PWM em 50%
   PWM1_Start();
   PWM1_Set_Duty(duty_cycle);

   // configura��o do LDC
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
        if(conta_quantidade_de_interrupcoes == 1){
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
        conta_quantidade_de_interrupcoes = 0;

        }// end atualiza��o de duty cycle
   PWM1_Set_Duty(duty_cycle);
   }// end loop principal

}// end main