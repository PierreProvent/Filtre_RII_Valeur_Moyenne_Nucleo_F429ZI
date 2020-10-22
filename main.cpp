#include "mbed.h"

#define fech 40000 // Fréquence d'échantillonnage souhaitée

Serial pc(USBTX, USBRX);

/* Entrées sorties */
AnalogIn entree(PF_10); // A5 connecteur Arduino
PwmOut monpwm(PD_14); // D10 Arduino

/* Timer et Ticker */
Ticker read_sample_ticker ;

/* Interruptions */
InterruptIn button(USER_BUTTON);

/* Variables globales */
float in = 0 ;
float moy = 0 ;
int pwmperiode = 500 ; //  us
int pwmpulsewidth = 250 ; //  us

/* Programmes d'interruption */
void pressed()
{
    pwmpulsewidth +=  0.1*pwmperiode ;
    if (pwmpulsewidth > pwmperiode ) pwmpulsewidth = 0 ;
    monpwm.pulsewidth_us(pwmpulsewidth);
}

void read_sample() {
    in = 3.3f*entree.read(); // Conversion en volts
    moy = 0.0001f*in + 0.9999f*moy ;
    /* Calcul de la valeur moyenne avec un filtre récursif d'ordre 1 */
    /* y(n) = (1 + a)*x(n) - a*y(n-1) */
    /* a = -0.9999 pour un lissage fort */
}

int main()
{
    button.rise(&pressed);
    pc.printf("\033[2J"); // Sequence escape pour effacer la console
    pc.printf("\033[0;0H"); // Curseur en 0 ; 0
    pc.printf("Mesure de valeur moyenne d'un signal analogique injecte sur la broche PF_10(A5)\n");
    pc.printf("Calcul par filtre numerique passe-bas recursif RII premier ordre\n"); 
    pc.printf("Signal test interne disponible : PWM sur broche PD_14 (D10)\n"); 
    pc.printf("Entrer la periode du signal PWM : 500 us < T < 50 000 us : ");
    pc.scanf("%d",&pwmperiode) ; pc.printf("\n");
    pwmpulsewidth = pwmperiode/2 ;
    monpwm.period_us(pwmperiode);
    monpwm.pulsewidth_us(pwmpulsewidth);
    pc.printf("Relier le signal analogique externe ou le signal test interne sur PF_10(A5)\n");
    pc.printf("Bouton USER pour modifier le rapport cyclique du signal test si necessaire\n");
    pc.printf("fech = %d Hz\n",fech);
    read_sample_ticker.attach_us(&read_sample,1000000/fech); 
    while (1) {
        pc.printf("\nFrequence signal test PWM = %d Hz Rapport cyclique = %4d %%\n",
        1000000/pwmperiode,(100*pwmpulsewidth)/pwmperiode);
        pc.printf("Valeur moyenne du signal sur PF_10(A5) = %10.3f volts\n",moy) ;
        pc.printf("\033[3A"); // Sequence escape qui remonte le curseur de 3 lignes sur la console
        wait(0.2) ; // pause affichage
    }
}