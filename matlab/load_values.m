
% Each sensor is ordered x, y, z and is in the following units:
% bias_mag, mag: Gauss
% bias_gyro, gyro: degrees / second
% bias_accel, accel: 9.81 * meters / second^2
function [time, mag_fs, gyro_fs, accel_fs, bias_mag, mag, bias_gyro, gyro, bias_accel, accel] = load_values(series_name)

data = importdata(series_name);

% TODO: These may change from experiment to experiment
% and should be stored in the series data
mag_range = 2048;
accel_range = 32768;
gyro_range = 32768;

% Full range scale
mag_fs = 1.88; % Gauss (2048/1090)
accel_fs = 2;  % Multiples of g
gyro_fs = 250; % Degrees / second

samples = length(data) - 1;
time = data(2:samples, 1);

bias_mag = data(2:4) * mag_fs / mag_range;
mag = data(2:samples, 2:4) * mag_fs / mag_range;

bias_gyro = data(1, 5:7) * gyro_fs / gyro_range;
gyro = data(2:samples, 5:7) * gyro_fs / gyro_range;

bias_accel = data(1, 8:10) * accel_fs / accel_range;
accel = data(2:samples, 8:10) * accel_fs / accel_range;

end