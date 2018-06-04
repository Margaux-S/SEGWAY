clear all;
close all;

%% Boucle courant

L = 0.00047 
R = 0.47     

E = 24       % Tension alim moteur

Kp=R
Ti=L/R
Ki=1/Ti
Fe=6700
Te=1/Fe
M_BO_s= tf ([1/R],[L/R 1])
BF=feedback(M_BO_s*Kp,1)

open_system('Boucle_courant.slx')

C_PI= tf ([Kp2*Ti Kp], [Ti 0])

BF2=feedback(M_BO_s*C_PI,1)

M_BO_z = c2d(M_BO_s,Te,'tustin')
C_PI_z= c2d(C_PI,Te,'tustin')
BF3=feedback(M_BO_z*C_PI_z,1)