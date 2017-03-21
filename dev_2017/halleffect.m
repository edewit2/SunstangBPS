%% Hall Effect Signal Conditioning
%

% initialize variables
I = -30:0.1:92;
vref = 10;
voff = 4;
G = 1147/900;
S = 0.032;

vadc = @(I) (vref/2 + I.*S - voff).*G;

vadc(-15)
vadc(0)
vadc(30)
vadc(92)

figure(1)
plot(vadc(I),I);
L = polyfit(vadc(I),I,1)
fit = polyval(L,I);

sprintf('I = %0.4f*V + (%0.4f)',L(1),L(2))