% We measure the gravity and magnetic field vectors in the body frame,
% using an accelerometer and a magnetometer. Assuming that there is no
% additional acceleration or magnetic disturbance, this can give us
% the absolute orientation of the device.

% This algorithm attempts to fit a quaternion describing a rotation,
% which best maps the vectors from the body frame to known vectors in
% an inertial (earth) frame, using least squares optimization.

series_name = 'series1.txt';
[time, mag_fs, gyro_fs, accel_fs, bias_mag, mag, bias_gyro, gyro, bias_accel, accel] = load_values(series_name);
n = length(time);

g_i = bias_accel; % Gravity in inertial frame
b_i = bias_mag;   % Magnetic field in inertial frame.
                  % This should be measured in an initialization routine

% We define the error term as the square error between
% the inertial vectors and the mapped-to-inertial body
% vectors. The map is done by mapping a quaternion to
% the rotation matrix describing the orientation of the
% body relative to the intertial frame.

% See equation (6)
R = @(a, b, c, d) [d^2+a^2-b^2-c^2   2*(a*b-c*d)      2*(a*c+b*d)  ;
                     2*(a*b+c*d)   d^2+b^2-a^2-c^2    2*(b*c-a*d)  ;
                     2*(a*c-b*d)     2*(b*c+a*d)   d^2+c^2-b^2-a^2 ];
                 
M = @(q) [R(q(1), q(2), q(3), q(4)) zeros(3, 3) ; 
          zeros(3, 3) R(q(1), q(2), q(3), q(4)) ];

Dr1 = @(a, b, c, d, y) [ -[ a  b  c  0  0  0] * y ;
                         -[-b  a  d  0  0  0] * y ;
                         -[-c -d  a  0  0  0] * y ;
                         -[ d -c  b  0  0  0] * y ] * 2;
                 
Dr2 = @(a, b, c, d, y) [ -[ b -a -d  0  0  0] * y ;
                         -[ a  b  c  0  0  0] * y ;
                         -[ d -c  b  0  0  0] * y ;
                         -[ c  d -a  0  0  0] * y ] * 2;
                 
Dr3 = @(a, b, c, d, y) [ -[ c  d -a  0  0  0] * y ;
                         -[-d  c -b  0  0  0] * y ;
                         -[ a  b  c  0  0  0] * y ;
                         -[-b  a  d  0  0  0] * y ] * 2;
                
Dr4 = @(a, b, c, d, y) [ -[ 0  0  0  a  b  c] * y ;
                         -[ 0  0  0 -b  a  d] * y ;
                         -[ 0  0  0 -c -d  a] * y ;
                         -[ 0  0  0  d -c  b] * y ] * 2;

Dr5 = @(a, b, c, d, y) [ -[ 0  0  0  b -a -d] * y ;
                         -[ 0  0  0  a  b  c] * y ;
                         -[ 0  0  0  d -c  b] * y ;
                         -[ 0  0  0  c  d -a] * y ] * 2;
                
Dr6 = @(a, b, c, d, y) [ -[ 0  0  0  c  d -a] * y ;
                         -[ 0  0  0 -d  c -b] * y ;
                         -[ 0  0  0  a  b  c] * y ;
                         -[ 0  0  0 -b  a  d] * y ] * 2;
                
J = @(q, y) [Dr1(q(1), q(2), q(3), q(4), y)' ; 
             Dr2(q(1), q(2), q(3), q(4), y)' ;
             Dr3(q(1), q(2), q(3), q(4), y)' ; 
             Dr4(q(1), q(2), q(3), q(4), y)' ;
             Dr5(q(1), q(2), q(3), q(4), y)' ;
             Dr6(q(1), q(2), q(3), q(4), y)' ];

q = zeros(4, n);
for k=1:n
    y_i = [g_i b_i]';
    y_b = [accel(k, :) mag(k, :)]';
    
    m = 3; % Gauss-Newton iterations
    qk = [0 0 0 1]';
    for l=1:m
        rk = y_i - M(qk) * y_b;
        Jk = J(qk, y_b);
        pk = (Jk'*Jk) \ (-Jk'*rk);
        qk = qk + pk;
    end
    
    % Normalize
    len_q = sqrt(qk(1)^2+qk(2)^2+qk(3)^2+qk(4)^2);
    
    q(:, k) = qk / len_q;
    
end

% Quaternion components
a = q(1, :);
b = q(2, :);
c = q(3, :);
d = q(4, :);

% Euler angles
phi = atan2(2*(d.*a + b.*c), 1 - 2*(a.*a + b.*b));
theta = asin(2*(d.*b - c.*a));
psi = atan2(2*(d.*c + a.*b), 1 - 2*(b.*b + c.*c));

%% PLOT QUATERNION COMPONENTS
figure(1); clf(1);
subplot(4, 1, 1);
plot(time, q(1, :), 'LineWidth', 2, 'Color', [0 0 0]);

subplot(4, 1, 2);
plot(time, q(2, :), 'LineWidth', 2, 'Color', [0 0 0]);

subplot(4, 1, 3);
plot(time, q(3, :), 'LineWidth', 2, 'Color', [0 0 0]);

subplot(4, 1, 4);
plot(time, q(4, :), 'LineWidth', 2, 'Color', [0 0 0]);

%% PLOT EULER ANGLES
figure(1); clf(1);

% rotation about body-z
subplot(3, 1, 1);
plot(time, phi * 180 / pi, 'LineWidth', 2, 'Color', [0 0 0]);
xlabel('time (sec)');

% rotation about rotated body-y
subplot(3, 1, 2);
plot(time, theta * 180 / pi, 'LineWidth', 2, 'Color', [0 0 0]);
xlabel('time (sec)');

% rotation about rotated body-x
subplot(3, 1, 3);
plot(time, psi * 180 / pi, 'LineWidth', 2, 'Color', [0 0 0]);
xlabel('time (sec)');

%% ANIMATED FRAME

h = plot3([0 1], [0 0], [0 0]);
xlim([-2 2]);
ylim([-2 2]);
zlim([-2 2]);
box on;
grid on;
hold on;
for t=1:length(time)
    cx = cos(psi(t));
    sx = sin(psi(t));
    cb = cos(theta(t));
    sb = sin(theta(t));
    cg = cos(phi(t));
    sg = sin(phi(t));
    Rx = [1 0 0 ; 0 cx -sx ; 0 sx cx];
    Ry = [cb 0 sb ; 0 1 0 ; -sb 0 cb];
    Rz = [cg -sg 0 ; sg cg 0 ; 0 0 1];
    R = Rz * Ry * Rx;
    x_axis = R * [1 0 0]';
    y_axis = R * [0 1 0]';
    z_axis = R * [0 0 1]';
    xdata = [0 x_axis(1) 0 y_axis(1) 0 z_axis(1)];
    ydata = [0 x_axis(2) 0 y_axis(2) 0 z_axis(2)];
    zdata = [0 x_axis(3) 0 y_axis(3) 0 z_axis(3)];
    set(h, 'xdata', xdata, 'ydata', ydata, 'zdata', zdata);
    pause(0.01);
end
