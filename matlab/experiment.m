data = importdata('series2.txt');
mag_fs = 0.73; % mG / lsb
accel_fs = 2;  % g / lsb
gyro_fs = 250; % dps / lsb
mag_range = 2048;
accel_range = 32768;
gyro_range = 32768;

samples = length(data) - 1;
time = data(2:samples, 1);

bias_mag_x = data(1, 2) * mag_fs / mag_range;
bias_mag_y = data(1, 3) * mag_fs / mag_range;
bias_mag_z = data(1, 4) * mag_fs / mag_range;
mag_x = data(2:samples, 2) * mag_fs / mag_range;
mag_y = data(2:samples, 3) * mag_fs / mag_range;
mag_z = data(2:samples, 4) * mag_fs / mag_range;

bias_gyro_x = data(1, 5) * gyro_fs / gyro_range;
bias_gyro_y = data(1, 6) * gyro_fs / gyro_range;
bias_gyro_z = data(1, 7) * gyro_fs / gyro_range;
gyro_x = data(2:samples, 5) * gyro_fs / gyro_range;
gyro_y = data(2:samples, 6) * gyro_fs / gyro_range;
gyro_z = data(2:samples, 7) * gyro_fs / gyro_range;

bias_accel_x = data(1, 8) * accel_fs / accel_range;
bias_accel_y = data(1, 9) * accel_fs / accel_range;
bias_accel_z = data(1, 10) * accel_fs / accel_range;
accel_x = data(2:samples, 8) * accel_fs / accel_range;
accel_y = data(2:samples, 9) * accel_fs / accel_range;
accel_z = data(2:samples, 10) * accel_fs / accel_range;

%% MAGNETOMETER ALL
hold all;
box on;
grid on;
plot(time, mag_x, 'LineWidth', 2);
plot(time, mag_y, 'LineWidth', 2);
plot(time, mag_z, 'LineWidth', 2);
xlabel('time (sec)');
ylabel('b (mG)');
xlim([0 8]);
ylim([-mag_fs mag_fs]);

%% ACCELEROMETER ALL
hold all;
box on;
grid on;
plot(time, accel_x - bias_accel_x, 'LineWidth', 2);
plot(time, accel_y - bias_accel_y, 'LineWidth', 2);
plot(time, accel_z - bias_accel_z, 'LineWidth', 2);
xlabel('time (sec)');
ylabel('a (9.81m/s^2)');
xlim([0 8]);
ylim([-accel_fs accel_fs]);

%% GYRO ALL
hold all;
box on;
grid on;
plot(time, gyro_x - bias_gyro_x, 'LineWidth', 2);
plot(time, gyro_y - bias_gyro_y, 'LineWidth', 2);
plot(time, gyro_z - bias_gyro_z, 'LineWidth', 2);
xlabel('time (sec)');
ylabel('w (deg / sec)');
legend('x', 'y', 'z');
xlim([0 8]);
ylim([-gyro_fs gyro_fs]);

%% ALL

subplot(3, 3, 1);
plot(time, accel_x - bias_accel_x, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('a_x [mG]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-accel_fs accel_fs]);

subplot(3, 3, 2);
plot(time, accel_y - bias_accel_y, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('a_y [mG]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-accel_fs accel_fs]);

subplot(3, 3, 3);
plot(time, accel_z - bias_accel_z, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('a_z [mG]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-accel_fs accel_fs]);

subplot(3, 3, 4);
plot(time, gyro_x - bias_gyro_x, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('g_x [degrees/s]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-gyro_fs gyro_fs]);

subplot(3, 3, 5);
plot(time, gyro_y - bias_gyro_y, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('g_y [degrees/s]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-gyro_fs gyro_fs]);

subplot(3, 3, 6);
plot(time, gyro_z - bias_gyro_z, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('g_z [degrees/s]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-gyro_fs gyro_fs]);

subplot(3, 3, 7);
plot(time, mag_x, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('b_x [mG]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-mag_fs mag_fs]);

subplot(3, 3, 8);
plot(time, mag_y, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('b_y [mG]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-mag_fs mag_fs]);

subplot(3, 3, 9);
plot(time, mag_z, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('b_z [mG]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-mag_fs mag_fs]);