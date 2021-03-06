
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bits (Code protection off)
#pragma config BOREN = ON       // Brown-out Reset Enhable bit (BOR enabled)
#pragma config LVP = ON         // Low Voljtage In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code Protection off)
#include <xc.h>
#include<pic16f877a.h>
#define _XTAL_FREQ 4000000
#define Baud_rate 9600

unsigned short int data [30]={};
unsigned short int acc [10]={};
char tensionL,tensionH, tempH,tempL,currentH,currentL,s;
char check;

unsigned long somme,av,co;
volatile unsigned int short val ;
int rpt;

unsigned short int  c,value,temp,k,courant,tension;


void ADC_Init();



void ADC_Init()
{
  ADCON0 = 0x81;               //Turn ON ADC and Clock Selectiojjjn
  ADCON1 = 0xC0;               //All pins as Analog Input and setting Reference Voltages
}

unsigned int ADC_Read( unsigned int channel)
{
  if(channel > 7)              //Channel range is 0 ~ 7
    return 0;

  ADCON0 &= 0x85;              //Clearing channel selection bits
  ADCON0 |= channel<<3;
  ADCON1=0x80;
  //Setting channel selection bits
  
  __delay_ms(2);               //Actmaxuisition time to charge hold capacitor
  GO_nDONE = 1;                //Initializ1es A/D conversion
  while(GO_nDONE);             //Waiting for conversion to complete
  return ((ADRESH<<8)+ADRESL); //Return result
}




unsigned int ADC_Read1( unsigned int channel)
{
  if(channel > 7)              //Channel range is 0 ~ 7
    return 0;

  ADCON0 &= 0x85;              //Clearing channel selection bits
  ADCON0 |= channel<<3;
  ADCON1=0x81;
  //Setting channel selection bits
  
  __delay_ms(2);               //Actmaxuisition time to charge hold capacitor
  GO_nDONE = 1;                //Initializ1es A/D conversion
  while(GO_nDONE);             //Waiting for conversion to complete
  return ((ADRESH<<8)+ADRESL); //Return result
}
void Shift()       //function of shift&averge it's numeric filter (IIR)
{
    k=0;
    for(int i=0;i<29;i++){
        data[i]= data[i+1];
        
    }
    data[29]=value;
    
    
}
void averge(){
    av=0;
    temp=0;
    for(int i=0;i<30;i++){
        av = av+data[i];
    }
    temp=av/30;
    tempL=temp;
    
    tempH=temp>>8;
}

void Shi()
{
    k=0;
    for(int i=0;i<9;i++){
        acc[i]= acc[i+1];
        
    }
    acc[9]=val;
    
    
}
void aver(){
    co=0;
    temp=0;
    for(int i=0;i<10;i++){
        co = co+acc[i];
    }
    courant=co/10;
    
    currentL=courant;
    
    
    currentH=courant>>8;
   
}

void ADC1()
{
    
   somme=0;
   c=0;

   
   __delay_ms(1);
    for(int i=0; i<40;i++)
    {
         c=ADC_Read(s);
         __delay_ms(5);
        somme=somme+c;
   
        
    }
    value=somme/40;
    Shift();
    averge();
    somme=0;
}
void num() // current sensor ADC 
{
    __delay_ms(1);
        for(int i=0; i<20;i++)
        {
        
          
          c=ADC_Read1(s);
          __delay_ms(5);
          somme=somme+c;
    
         }
       
         val=somme/20;
    
    
}




void ADC2() // fucntion of switiching between OP AMP1- OP AMP2
{
   somme=0;
   c=0;
   num();       
         if((val < 0x2b8) && (val >  0xcd))    // IF MEASURED VALUE LESS THAN 3.4V  and greater than 0.6v
          {
            check=0;  
           }
         else
           {
             
           
   
            s=5;              // read from pin PORTA.A5
            num();            //ADC 
            if((val < 0x2b8) && (val >  0xcd))    // IF MEASURED VALUE LESS THAN 3.4V  and greater than 0.6v
              {
               check=1;  
               }
               else        // IF MEASURED VALUE GREATER (means the OP AMP saturated)
               {
                 s=6;     // read from pin PORTA.A6
           
                 num();       
                 if((val < 0x2b8) && (val >  0xcd))// IF MEASURED VALUE LESS THAN 3.4V  and greater than 0.6v
                    {
                    check=2;  
                    }
                 else
                 {
                    s=7;
                    num();             
                        check=3;         
            }   
         }
    }
   
    
   Shi();
                                   
    aver();
    somme=0;
   
}


void ADC3(){
   somme=0;
   c=0;
  
    
     __delay_ms(8);
    for(int i=0; i<40;i++)
    {
        c=ADC_Read(s);
        __delay_ms(8);
        somme=somme+c;
    
    }
    tension=somme/40;
  tensionL=tension;
 
    tensionH=tension>>8;
   
            somme=0;
    
}

void Initialize_UART(void)
{

    TRISC6 = 0; // TX Pin set as output
    TRISC7 = 1; // RX Pin set as input
    //________I/O pins set __________//
    
    /**Initialize SPBRG register for required int rpt,m;

    baud rate and set BRGH for fast baud_rate**/
    SPBRG = ((_XTAL_FREQ/16)/Baud_rate) - 1;
    BRGH  = 1;  // for high baud_rate
  
    SYNC  = 0;    // Asynchronous
    SPEN  = 1;    // Enable serial port pins
   

    TXEN  = 1;    // enable transmission
    CREN  = 1;    // enable reception

    
 
    TX9   = 0;    
    RX9   = 0;   
       
}

//_____
void UART_send_char(char bt)  
{
   
    TXREG = bt; 
}

void main (void ){
    TRISB=0x00;
    TRISD=0x00;
    TRISA=0xff;
    TRISE=0xff;
    TRISC=0x00;
    Initialize_UART();    //Initialize UART module   
    ADC_Init();
        __delay_ms(600);
    PORTD=0x00;
    PORTB=0x00;
    while(1)
    {

        for(rpt=0;rpt<5;rpt++)
        {  
            __delay_ms(50);
            
            
           
           
          s=0;
          ADC3();
          __delay_ms(10);
          UART_send_char('s');
          __delay_ms(10);
          UART_send_char('k');
          
         __delay_ms(10);
         UART_send_char(tensionL);
       
          
       
          __delay_ms(10);
          UART_send_char('k');
          __delay_ms(10);
          UART_send_char('s');
          
         __delay_ms(10);
         UART_send_char(tensionH);
       
          
          __delay_ms(10);
           s=1;
            
            ADC1();
            __delay_ms(10);
             
             UART_send_char('p');
           __delay_ms(10);
             UART_send_char('m');
            __delay_ms(10);
          UART_send_char(tempL);
        
         __delay_ms(10);
          
           UART_send_char('m');
           __delay_ms(10);
             UART_send_char('p');
            __delay_ms(10);
          UART_send_char(tempH);
          
          __delay_ms(10);
          
           s=4;
          ADC2();
          UART_send_char('l');
          __delay_ms(10);
          UART_send_char('i');
          __delay_ms(10);
          UART_send_char(check);
          __delay_ms(10);
          UART_send_char('f');
          __delay_ms(10);
          UART_send_char('h');
        
         __delay_ms(10);
         UART_send_char(currentL);
         
          __delay_ms(10);
         __delay_ms(10);
          UART_send_char('h');
          __delay_ms(10);
          UART_send_char('f');
          
         __delay_ms(10);
         UART_send_char(currentH);
         
          __delay_ms(10);
        }
     
        if(temp>0x333)
        {
            RB0=1;
            
        }
        else 
        {
            
        if(tension<649)
            RB0=0;
        }
        
    }       
    
        
       
        

}
