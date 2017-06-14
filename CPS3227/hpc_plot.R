library(ggplot2)
library(scales)

bodies <- c(64, 1024, 4096, 16384)
rep(bodies,c(3, 3, 3, 3))
parallel.type <- rep(c('MP S', 'MPI NS', 'Hybrid NS'), 4)
speedup <- c(1.14, 2.4, 1.09, 3.07, 3.95, 9.62, 11.57, 4.02, 10.98, 11.86, 4.01, 10.89)


df <- data.frame(bodies = rep(bodies,c(3, 3, 3, 3)), parallel.type = parallel.type, speedup=speedup)

ggplot(data = df, aes(x=bodies, y=speedup, group=parallel.type, colour=parallel.type)) +
  geom_line() +
  geom_point() +
  scale_x_continuous(trans='log2',
                     breaks = trans_breaks("log2", function(x) 2^x),
                     ) +
  ggtitle("Average Speedup for n Bodies") +
  labs(x="Bodies (Log 2 scale)", y="Speedup Factor")
