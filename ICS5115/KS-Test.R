library(ggplot2)

F10 <- ecdf(rnorm(10000))
summary(F10)
str(F10)
plot(F10)
plot(F10, verticals = TRUE, do.points = FALSE)


sample1 <- rnorm(10000, 10, 5)
sample2 <- rnorm(10000, 1, 5)


ks.test(sample1, sample2)
group <- c(rep("sample1", length(sample1)), rep("sample2", length(sample2)))
dat <- data.frame(KSD = c(sample1,sample2), group = group)
summary(dat)
# create ECDF of data
cdf1 <- ecdf(sample1) 
cdf2 <- ecdf(sample2) 

# find min and max statistics to draw line between points of greatest distance
minMax <- seq(min(sample1, sample2), max(sample1, sample2), length.out=length(sample1)) 
x0 <- minMax[which( abs(cdf1(minMax) - cdf2(minMax)) == max(abs(cdf1(minMax) - cdf2(minMax))) )] 
y0 <- cdf1(x0) 
y1 <- cdf2(x0) 

# if invalid graphics state then run dev.off()
ggplot(dat, aes(x = KSD, group = group, color = group))+
  stat_ecdf(size=1) +
 # theme_bw(base_size = 28) +
  theme(legend.position ="top") +
  xlab("Sample") +
  ylab("ECDF") +
  #geom_line(size=1) +
  geom_segment(aes(x = x0[1], y = y0[1], xend = x0[1], yend = y1[1]),
               linetype = "dashed", color = "red") +
  geom_point(aes(x = x0[1] , y= y0[1]), color="red", size=1) +
  geom_point(aes(x = x0[1] , y= y1[1]), color="red", size=1) +
  ggtitle("K-S Test: Sample 1 / Sample 2") 
  #theme(legend.title=element_blank())

##############################################################################
# As Stijn pointed out, the k-s test returns a D statistic and a 
# p-value corresponding to the D statistic. 
# The D statistic is the absolute max distance (supremum) between the CDFs 
# of the two samples. The closer this number is to 0 the more likely 
# it is that the two samples were drawn from the same distribution. 
# Check out the Wikipedia page for the k-s test. 
# The p-value returned by the k-s test has the same interpretation as other p-values. 
# You reject the null hypothesis that the two samples were drawn from the same 
# distribution if the p-value is less than your significance level. 
# You can find tables online for the conversion of the D statistic into a p-value 
# if you are interested in the procedure.

x <- rnorm(100)
y <- runif(30)

hist(x)
hist(y)
# Do x and y come from the same distribution?
ks.test(x, y)
# Does x come from a shifted gamma distribution with shape 3 and rate 2?
ks.test(x+2, "pgamma", 3, 2) # two-sided, exact
ks.test(x+2, "pgamma", 3, 2, exact = FALSE)
ks.test(x+2, "pgamma", 3, 2, alternative = "gr")
