hold off;
x=[10, 100, 1000, 2000];
y1=[286.3333333,151.6666667,99.33333333,132];
y2=[228.6666667,192.6666667,168.3333333,132.6666667];
y3=[309.3333333,208,129.6666667,145.3333333];
hold on;
plot(x,y1);
plot(x,y2);
plot(x,y3);
title("AVG propagation");
xlabel("Budget");
ylabel("Mean score");
legend("Level 1","Level 2","Level 3");

