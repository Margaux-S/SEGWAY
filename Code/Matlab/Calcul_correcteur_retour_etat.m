% TD Segway, calcul du correcteur retour d’état
% 
% Ion Hazyuk - février 2016

clc;
clear all;

%% Paramètres physiques

% avec user
% g=9.81;
% l = 0.7; 
% lp= 1;
% Rw= 0.127;
% M = 35;
% m = 1.5;
% Mp = 75;
% j_wheel = 0.017+0.00065+0.009; % Inertia of the wheel and its pinion : Tire/Hub/Gear
% j_gear_motor = 0.001;
% j_handlebar = 0.05;
% j_person = 20;
% kg = 64/11;
% 
% m_eq_w = 2*((j_wheel + kg * kg * j_gear_motor)/(Rw^2));
% Meq = M + Mp + m + m_eq_w;
% Jeq = j_handlebar+j_person+Mp*lp^2+m*l^2;
% 
% a=Meq*(m*g*l+g*Mp*lp)/(Jeq*Meq-(m*l+Mp*lp)^2);
% b=2*kg*(m*l+Mp*lp)/(Rw*(Jeq*Meq-(m*l+Mp*lp)^2));
% c=Meq*Mp*lp*g/(Jeq*Meq-(m*l+Mp*lp)^2);
% 
% A=[0 1; a 0];
% B=[0 0
%    -b c];
% C=[1 0
%    0 1];
% D=[0,0;0,0];
% 
% sys=ss(A,B,C,D);
% 
% Te=1/50;
% sys_d=c2d(sys,Te,'zoh')
% 
% rank(ctrb(A,B(:,1)));
% P=[-4,-4];
% K=acker(A,B(:,1),P)
% 
% C1=[1 0];
% f=-1/(C(1,:)*inv(A-B(:,1)*K)*B(:,1));

%sans user
g=9.81;
l = 1; 
lp= 1;
Rw= 0.127;
M = 35;
m = 1.5;
Mp = 75;
j_wheel = 0.017+0.00065+0.009; % Inertia of the wheel and its pinion : Tire/Hub/Gear
j_gear_motor = 0.001;
j_handlebar = 0.15;
j_person = 20;
kg = 64/11;

m_eq_w = 2*((j_wheel + kg * kg * j_gear_motor)/(Rw^2));
Meq = M + m + m_eq_w;
Jeq = j_handlebar+m*l^2;

a=Meq*m*g*l/(Jeq*Meq-(m*l)^2);
b=2*kg*m*l/(Rw*(Jeq*Meq-(m*l)^2));
c=0;

A=[0 1; a 0];
B=[0 0
   -b c];
C=[1 0
   0 1];
D=[0,0;0,0];

A=[0 1; a 0];
B=[0
   -b];
C=[1 0
   0 1];
D=[0;0];

sys=ss(A,B,C,D);

Te=1/50;
sys_d=c2d(sys,Te,'zoh')

rank(ctrb(A,B(:,1)));
P=[-4,-4];
K=acker(A,B(:,1),P)

C1=[1 0];
f=-1/(C(1,:)*inv(A-B(:,1)*K)*B(:,1));