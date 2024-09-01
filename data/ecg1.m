ecg = readtable("ecg1_author_has_rbbb_lmao.csv");

v = table2array(ecg(:,1) - mean(ecg(:,1))) * 3e-6 / 22;
n = length(v);
fs = 1007;
t = (1:n)/fs * 1000;

plot(t, v)
grid
axis([200 1700 -2e-3 1.5e-3])


%snr(v, fs)
