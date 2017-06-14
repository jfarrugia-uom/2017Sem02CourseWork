library("ggplot2")

head(diamonds)
head(mtcars)
x <- runif(100)
plot(x)
qplot(x)


qplot(x, main="Uniform 1000", ylab="my ylab", xlab="")
# stacked histograms
qplot(clarity, data=diamonds, fill=cut, geom="bar")
ggplot(diamonds, aes(clarity, fill=cut)) + geom_bar()
# scatter with ggplot
qplot(hp, mpg, data=mtcars)
qplot(wt, mpg, data=mtcars)
qplot(mtcars$hp, geom="histogram", binwidth=50)

qplot(mtcars$hp,
      geom="histogram",
      binwidth = 50,
      main = "Histogram for Horse Power",
      xlab = "Age",
      fill=I("blue"),
      col=I("red"),
      alpha=I(.2),
      xlim=c(50,400))

qplot(wt, mpg, data=mtcars, color=qsec)