% Tension mesurée sur la sortie 5V de l’Arduino nano : 4.67 V
% Uin = 4.67;
% Tension calculée pour trouver la même tension que mesurée par le voltmètre
Uin = 4.8;

% Résistance
R = 10;

% format long e
format short
cd '~/Documents/programmation/ouilogique-Arduino/decharge-batterie/'
filename = 'data_sosbatterie_1.csv';      % 3.8 h, 1593 mAh
filename = 'data_sosbatterie_2.csv';      % 3.8 h, 1672 mAh
filename = 'data_high_creek_1.csv';       % 3.7 h, 1786 mAh
filename = 'data_samsung_1.csv';          % 3.1 h, 1472 mAh
filename = 'data_powerbank_didel_1.csv';  % 1.9 h,  893 mAh


M = csvread( filename );
x = M( : , 1 );
x = x - x( 1 );
y = M( : , 2 );
y = y / 1024 * Uin;

plot( x, y, 'r-', 'linewidth', 5 );
axis( [ x( 1 ) x( length( x ) )  0 6 ] )
title( 'DÉCHARGE D’UNE BATTERIE 18650', 'FontSize', 20, 'Position', [ 0.5  0.96 ] )
xlabel( 'TEMPS (s)', 'FontSize', 20 )
ylabel( 'TENSION (V)', 'FontSize', 20 )

Gt1 = find( y > 1 );
Y = y( Gt1 );

Ulast = Y( length( Y ) )
Umoy = mean( Y )
Imoy = Umoy / R
dT = x( length( Gt1 ) ) - x( 1 );
dT = dT / 3600
mA = Umoy / R * 1000
mAh = mA * dT

mAh_trapz = trapz( x/3600, y/R*1000 )