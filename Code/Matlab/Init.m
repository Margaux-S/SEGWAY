%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This .m file loads all numerical values for the Segway in the workspace
% to be used later on in the Simulink model
% Make sure to run it before running any Simulink model!
% 
% Work property of : Bonnaud B. - Fleury JB. - Moraine A.
% Date created : 01/11/14
% Last modified : 20/12/14
% By : Moraine A.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all; close all; clc;

%% Physical parameters of the Segway

% Gravity
g = 9.81; % Gravity [m/sec²]

% Dimensions
l = 0.7; % Distance between the pivot O and the center of gravity of the handlebar [m]
lp = 1; %Distance between the pivot O and the center of gravity of the person [m]
Rw = 0.254/2; %Radius of the wheel [m]

% Masses
M = 35; % Total mass of the lower part of the Seway [Kg]
m = 2; % Mass of the handbar [Kg]
Mp = 80; % Mass of the person [Kg]

% Inertia
Jw = 6.5e-4+0.9e-3+17e-3 ; % Inertia of a wheel and its pinion [Kg.m²]
Jm = 4.64e-3 ; % Inertia of a motor with its transmission shaft and pinion [Kg.m²]
Jh = 1e-1 ; % Inertia of the handlebar [Kg.m²]
Jp = 18 ; % Inertia of the person [Kg.m²]

% Transmission
Kg = 70/11; % Gearing ratio of the reducer (>1 for a reducer) [1]

% DC Motor
Km = 36/(2500/60*2*pi); % Torque constant of the DC motor [Nm/A]
R = 1; % Electrical resistance of the winding of the motor [Ohm]
L = 1e-3; % Inductance of the motor's winding [H]
Vbatt=36; % Voltage of the battery [V]
tau_rise=10*60; % Thermal time constant of the DC motor (rising temperature) (in s)
tau_decrease=30*60; % Thermal time constant of the DC motor (decreasing temperature) (in s)
I_nom=17.8; % Nominal current of the DC motor [A]
I_max=30; % Maximal current in the DC motor [A]

eta_pwc = 1;

% Rolling resistance coefficient
Crr=1e-2; % Rolling resistance coecient [1]

% Aerodynamical constants
Cx=1.1; % Drag coefficient of the person [1]
S=.8; % Area of the cross-section of the person [m²]
rho=1.2; % Density of the air [Kg/m²]

%% Initial conditions

z_0=0; % Initial position of the Segway
z_dot_0=0; % Initial speed of the Segway
theta_0=.0; % Initial angle of the Segway
theta_dot_0=0; % Initial angular speed of the Segway

%% Model parameters

Te = 2e-2; % Sampling period
Ti = 5e-3; % Sampling period for current loop

%% Controller parameters

K_P_theta=427;
K_I_theta=57;
K_D_theta=87;
K_I=100;
K_w=4*K_I;

theta_lim=.26; % Max angle at which the Segway comes to a full stop

%% Filter parameters

Ki_f = .0000897449867742515;
xi =2;

%% Calculated values

% Equivalent masses and inertia
Meq_w = 2*(Jw + Kg^2*Jm)/(Rw^2);
Meq = M + m + Mp + Meq_w;
Jeq = Jh + Jp + Mp*lp^2 + m*l^2;

% State-Space for input U and output theta
A=[2*Jeq*Km^2*Kg^2/(R*Rw^2*((m*l+Mp*lp)^2-Jeq*Meq)) 0 (m*l+Mp*lp)^2*g/((m*l+Mp*lp)^2-Jeq*Meq)
    -2*(m*l+Mp*lp)*Km^2*Kg^2/(R*Rw^2*((m*l+Mp*lp)^2-Jeq*Meq)) 0 -(m*l+Mp*lp)*Meq*g/((m*l+Mp*lp)^2-Jeq*Meq)
    0 1 0];
B=[-2*Jeq*Km*Kg/(R*Rw*((m*l+Mp*lp)^2-Jeq*Meq))
    2*(m*l+Mp*lp)*Km*Kg/(R*Rw*((m*l+Mp*lp)^2-Jeq*Meq))
    0];
C=[0 0 1];
D=0;

SYS = ss(A,B,C,D);

% State-Space for input Cm and output theta

A_2=[0 -(m*l+Mp*lp)*Meq*g/((m*l+Mp*lp)^2-Jeq*Meq)
    1 0];
B_2=[2*(m*l+Mp*lp)/(Rw*((m*l+Mp*lp)^2-Jeq*Meq))
    0];
C_2=[0 1];

D_2=0;

SYS_2=ss(A_2,B_2,C_2,D_2);

% State-Space for input U and output I
A_3=[2*Jeq*Km^2*Kg^2/(R*Rw^2*((m*l+Mp*lp)^2-Jeq*Meq))];
B_3=[-2*Jeq*Km*Kg/(R*Rw*((m*l+Mp*lp)^2-Jeq*Meq))];
C_3=[-Km*Kg/(Rw*R)];
D_3=1/R;

SYS_3 = ss(A_3,B_3,C_3,D_3);
