series_name = 'series1.txt';
[time, mag_fs, gyro_fs, accel_fs, bias_mag, mag, bias_gyro, gyro, bias_accel, accel] = load_values(series_name);
n = time(length(time));

mag_x = mag(:, 1);
mag_y = mag(:, 2);
mag_z = mag(:, 3);
bias_mag_x = bias_mag(:, 1);
bias_mag_y = bias_mag(:, 2);
bias_mag_z = bias_mag(:, 3);

accel_x = accel(:, 1);
accel_y = accel(:, 2);
accel_z = accel(:, 3);
bias_accel_x = bias_accel(:, 1);
bias_accel_y = bias_accel(:, 2);
bias_accel_z = bias_accel(:, 3);

gyro_x = gyro(:, 1);
gyro_y = gyro(:, 2);
gyro_z = gyro(:, 3);
bias_gyro_x = bias_gyro(:, 1);
bias_gyro_y = bias_gyro(:, 2);
bias_gyro_z = bias_gyro(:, 3);

%% MAGNETOMETER ALL
figure(1); clf(1);
hold all;
box on;
grid on;
plot(time, mag_x, 'LineWidth', 2);
plot(time, mag_y, 'LineWidth', 2);
plot(time, mag_z, 'LineWidth', 2);
xlabel('time (sec)');
ylabel('b (Gauss)');
xlim([0 n]);
ylim([-mag_fs mag_fs]);

%% ACCELEROMETER ALL
figure(1); clf(1);
hold all;
box on;
grid on;
plot(time, accel_x, 'LineWidth', 2);
plot(time, accel_y, 'LineWidth', 2);
plot(time, accel_z, 'LineWidth', 2);
xlabel('time (sec)');
ylabel('a (9.81m/s^2)');
xlim([0 n]);
ylim([-accel_fs accel_fs]);

%% GYRO ALL
figure(1); clf(1);
hold all;
box on;
grid on;
plot(time, gyro_x - bias_gyro_x, 'LineWidth', 2);
plot(time, gyro_y - bias_gyro_y, 'LineWidth', 2);
plot(time, gyro_z - bias_gyro_z, 'LineWidth', 2);
xlabel('time (sec)');
ylabel('w (deg / sec)');
legend('x', 'y', 'z');
xlim([0 n]);
ylim([-gyro_fs gyro_fs]);

%% ALL
figure(1); clf(1);
subplot(3, 3, 1);
plot(time, accel_x, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('a_x [Gauss]');
% xlabel('time (sec)');
grid on;
xlim([0 n]);
ylim([-accel_fs accel_fs]);

subplot(3, 3, 2);
plot(time, accel_y, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('a_y [Gauss]');
% xlabel('time (sec)');
grid on;
xlim([0 n]);
ylim([-accel_fs accel_fs]);

subplot(3, 3, 3);
plot(time, accel_z, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('a_z [Gauss]');
% xlabel('time (sec)');
grid on;
xlim([0 n]);
ylim([-accel_fs accel_fs]);

subplot(3, 3, 4);
plot(time, gyro_x - bias_gyro_x, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('g_x [degrees/s]');
% xlabel('time (sec)');
grid on;
xlim([0 n]);
ylim([-gyro_fs gyro_fs]);

subplot(3, 3, 5);
plot(time, gyro_y - bias_gyro_y, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('g_y [degrees/s]');
% xlabel('time (sec)');
grid on;
xlim([0 n]);
ylim([-gyro_fs gyro_fs]);

subplot(3, 3, 6);
plot(time, gyro_z - bias_gyro_z, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('g_z [degrees/s]');
% xlabel('time (sec)');
grid on;
xlim([0 n]);
ylim([-gyro_fs gyro_fs]);

subplot(3, 3, 7);
plot(time, mag_x, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('b_x [mG]');
% xlabel('time (sec)');
grid on;
xlim([0 n]);
ylim([-mag_fs mag_fs]);

subplot(3, 3, 8);
plot(time, mag_y, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('b_y [mG]');
% xlabel('time (sec)');
grid on;
xlim([0 n]);
ylim([-mag_fs mag_fs]);

subplot(3, 3, 9);
plot(time, mag_z, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('b_z [mG]');
% xlabel('time (sec)');
grid on;
xlim([0 n]);
ylim([-mag_fs mag_fs]);