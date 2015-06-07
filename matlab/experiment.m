data = importdata('series1.txt');
accel_fs = 2;
gyro_fs = 250;
bitrange = 32768;

time = data(:, 1);
mag_x = data(:, 2);
mag_y = data(:, 3);
mag_z = data(:, 4);

gyro_x = data(:, 5) * gyro_fs / bitrange;
gyro_y = data(:, 6) * gyro_fs / bitrange;
gyro_z = data(:, 7) * gyro_fs / bitrange;

accel_x = data(:, 8) * accel_fs / bitrange;
accel_y = data(:, 9) * accel_fs / bitrange;
accel_z = data(:, 10) * accel_fs / bitrange;

%% ACCELEROMETER ALL
hold all;
box on;
grid on;
plot(time, accel_x, 'LineWidth', 2);
plot(time, accel_y, 'LineWidth', 2);
plot(time, accel_z, 'LineWidth', 2);
xlabel('time (sec)');
ylabel('a (9.81m/s^2)');
xlim([0 8]);
ylim([-accel_fs accel_fs]);

%% GYRO ALL
hold all;
box on;
grid on;
plot(time, gyro_x, 'LineWidth', 2);
plot(time, gyro_y, 'LineWidth', 2);
plot(time, gyro_z, 'LineWidth', 2);
xlabel('time (sec)');
ylabel('w (deg / sec)');
legend('x', 'y', 'z');
xlim([0 8]);
ylim([-gyro_fs gyro_fs]);

%% ALL

subplot(2, 3, 1);
plot(time, accel_x, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('a_x [9.81m/s^2]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-accel_fs accel_fs]);

subplot(2, 3, 2);
plot(time, accel_y, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('a_y [9.81m/s^2]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-accel_fs accel_fs]);

subplot(2, 3, 3);
plot(time, accel_z, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('a_z [9.81m/s^2]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-accel_fs accel_fs]);

subplot(2, 3, 4);
plot(time, gyro_x, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('g_x [degrees/s]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-gyro_fs gyro_fs]);

subplot(2, 3, 5);
plot(time, gyro_y, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('g_y [degrees/s]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-gyro_fs gyro_fs]);

subplot(2, 3, 6);
plot(time, gyro_z, 'Color', [0 0 0], 'LineWidth', 2);
% ylabel('g_z [degrees/s]');
% xlabel('time (sec)');
grid on;
xlim([0 8]);
ylim([-gyro_fs gyro_fs]);