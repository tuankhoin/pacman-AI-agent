hold off;
x=[10, 100, 1000, 2000];
y1=[240.3333333,266.3333333,319.3333333,248];
y2=[303.3333333,315.6666667,387.3333333,367];
y3=[358.3333333,383.6666667,440.6666667,411];
hold on;
plot(x,y1);
plot(x,y2);
plot(x,y3);
title("MAX propagation");
xlabel("Budget");
ylabel("Mean score");
legend("Level 1","Level 2","Level 3");








