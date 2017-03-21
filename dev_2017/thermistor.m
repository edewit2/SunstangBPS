%% NTC Thermistor Curve Fitting
%

% thermistor data
data = [111300	-30
105770	-29
100544	-28
95605	-27
90935	-26
86518	-25
82339	-24
78385	-23
74642	-22
71097	-21
67740	-20
64572	-19
61569	-18
58720	-17
56018	-16
53453	-15
51019	-14
48708	-13
46514	-12
44430	-11
42450	-10
40568	-9
38780	-8
37080	-7
35463	-6
33925	-5
32462	-4
31069	-3
29744	-2
28482	-1
27280	0
26135	1
25045	2
24005	3
23014	4
22070	5
21169	6
20309	7
19489	8
18707	9
17960	10
17246	11
16564	12
15912	13
15290	14
14695	15
14127	16
13583	17
13063	18
12566	19
12091	20
11635	21
11199	22
10782	23
10383	24
10000	25
9633	26
9282	27
8945	28
8622	29
8313	30
8016	31
7731	32
7458	33
7195	34
6944	35
6702	36
6470	37
6247	38
6033	39
5828	40
5630	41
5440	42
5257	43
5082	44
4913	45
4750	46
4594	47
4444	48
4299	49
4160	50
4025	51
3896	52
3772	53
3652	54
3537	55
3426	56
3319	57
3216	58
3117	59
3021	60
2928	61
2839	62
2753	63
2670	64
2590	65
2512	66
2438	67
2366	68
2296	69
2229	70
2164	71
2101	72
2040	73
1981	74
1925	75
1870	76
1817	77
1766	78
1716	79
1668	80
1622	81
1577	82
1533	83
1491	84
1451	85
1411	86
1373	87
1336	88
1300	89
1266	90
];

clc;
data_sel = data(16:106,:);      % select working range
R = data_sel(:,1);              % extract resistances
T = data_sel(:,2);              % extract temperatures
clf;

% polynomial fit
O = 5;                          % order of polynomial (if O>5 badly conditioned)
[P]= polyfit(T,R,O);
R_fit = polyval(P,T);
error_R = abs(R_fit-R);
R_fit = @(T) 0;
for i = 1:O+1
   R_fit = @(T) R_fit(T) + P(i)*T.^(O-i+1);
end
figure(1)
subplot(2,1,1)
plot(T,R_fit(T),'.',T,R);
title('resistance vs temperature curve');
legend('fit','data')
subplot(2,1,2)
plot(T,error_R);
title('resistance fit error vs temperature')

%% Thermistor Signal Conditioning
%

% resistive divider transfer function
Rs = 0;
R1 = 1.2e4 + Rs;
Vdd = 2.5;
vadc = @(T) Vdd./(R1+R_fit(T)).*R_fit(T);

L = polyfit(T,vadc(T),1);
v_fit = polyval(L,T);

error_V = (v_fit-vadc(T));

figure(2)
subplot(2,1,1)
plot(T,vadc(T),'.',T,v_fit);
title('vout vs temperature')
legend('actual',sprintf('V = %0.4f + (%0.4f)*T',L(2),L(1)));

subplot(2,1,2)
plot(T,error_V);
title('voltage fit error vs temperature');

sprintf('T = (V - %0.4f)/%0.4f',L(2),L(1))
