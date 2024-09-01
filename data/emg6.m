emg = readtable("emg6.csv");
fs = 1035.17;
n = 998;
t = (1:n) / fs * 1000;

tiledlayout(6,1, 'Padding', 'none', 'TileSpacing', 'compact'); 

for i=1:6
    nexttile
    v = (table2array(emg(1:end-2, i)) - 2^23) * 3e-6;
    v = v - mean(v);
    f = lowpass(v, 300, fs);

    hold on
    plot(t, v, 'k.', 'MarkerSize', 2);
    plot(t, f, 'b-', 'LineWidth', 1.5);
    text(75, 0.4e-3, sprintf('%d', i-1), 'FontSize', 20, 'VerticalAlignment','baseline');
    hold off

    axis([50 400 -2e-3 2e-3])
    if i < 6
        xticklabels([])
    end
    grid;
end

xlabel('t [ms]')
