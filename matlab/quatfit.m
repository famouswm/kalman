% We measure the gravity and magnetic field vectors in the body frame,
% using an accelerometer and a magnetometer. Assuming that there is no
% additional acceleration or magnetic disturbance, this can give us
% the absolute orientation of the device.

% This algorithm attempts to fit a quaternion describing a rotation,
% which best maps the vectors from the body frame to known vectors in
% an inertial (earth) frame, using least squares optimization.

g_i = [0 0 -9.81]; % Gravity in inertial frame
b_i = [1 0 0];     % Magnetic field in inertial frame.
                   % This should be measured in an initialization routine
               
g_b = [-0.1 0.05 -9.61]; % Gravity in body frame (measured)
b_b = [0.95 0.05 0];     % Magnetic field in body frame (measured)

y_i = [g_i b_i]';
y_b = [g_b b_b]'; 

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

n = 3;
q = [0 0 0 1]';
for i=1:n
    rk = y_i - M(q) * y_b;
    Jk = J(q, y_b);
    pk = (Jk'*Jk) \ (-Jk'*rk);
    q = q + pk;
end
q