fs = 1038.33;
n = 5000;
t = (1:n) / fs * 1000;

ecg3 = readtable("ecg3.csv");
ecg3 = table2array(ecg3(:, [3, 1, 2]));

tiledlayout(3,1, 'Padding', 'none', 'TileSpacing', 'compact');

for i=1:3
    nexttile
    v = (ecg3(:, i) - 2^23) * 3e-6;
    v = v - mean(v);
    %v = bandpass(v, [5, 50], fs);
    f = lowpass(v, 10, fs);
    hold on
    plot(t, v, 'k.', 'MarkerSize', 2);
    plot(t, f, 'b-', 'LineWidth', 1.5);
    text(100, 0.4e-3, repmat('I', 1, i), 'FontSize', 32, 'VerticalAlignment','baseline');
    hold off
    axis([0 4800 -1e-3 1e-3])
    if i < 3
        xticklabels([])
    end
    grid;
end

xlabel('t [ms]')

%%

v = (ecg3(:, 2) - 2^23) * 3e-6;
v = v - mean(v);
f = lowpass(v, 10, fs);

snr(f, fs)
